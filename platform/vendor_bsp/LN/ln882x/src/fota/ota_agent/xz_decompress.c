
#include "xz_decompress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define XZ_BASE_SIZE    1846
#define XZ_LIT_SIZE     768

#define kNumTopBits              24
#define kTopValue               ((uint32_t)1 << kNumTopBits)
#define kNumBitModelTotalBits    11
#define kBitModelTotal          (1 << kNumBitModelTotalBits)
#define kNumMoveBits             5

typedef struct
{
    uint8_t* Buffer;
    uint8_t* BufferLim;
    uint32_t Range;
    uint32_t Code;
    read_cb_t* InCallback;
    int read_cb_ret;
    int ExtraBytes;
} CRangeDecoder;

typedef struct
{
    CRangeDecoder RangeDecoder;
    uint8_t* Dictionary;
    uint32_t DictionarySize;
    uint32_t DictionaryPos;
    uint32_t GlobalPos;
    uint32_t Reps[4];
    int lc;
    int lp;
    int pb;
    int State;
    int PreviousIsMatch;
    int RemainLen;
} XzVarState;

uint8_t RangeDecoderReadByte(CRangeDecoder* rd)
{
    uint32_t size = 0;
    if (rd->Buffer == rd->BufferLim)
    {
        rd->read_cb_ret = rd->InCallback->Read(&rd->Buffer, &size);
        rd->BufferLim = rd->Buffer + size;
        if (size == 0) {
            rd->ExtraBytes = 1;
            return 0xFF;
        }
    }
    return (*rd->Buffer++);
}

#define ReadByte (RangeDecoderReadByte(rd))

void RangeDecoderInit(CRangeDecoder* rd, read_cb_t* in_cb)
{
    rd->InCallback = in_cb;
    rd->Buffer = rd->BufferLim = 0;
    rd->ExtraBytes = 0;
    rd->Code = 0;
    rd->Range = (0xFFFFFFFF);
    for (int i = 0; i < 5; i++) {
        rd->Code = (rd->Code << 8) | ReadByte;
    }
}

uint32_t RangeDecoderDecodeDirectBits(CRangeDecoder* rd, int numTotalBits)
{
    uint32_t range = rd->Range;
    uint32_t code = rd->Code;
    uint32_t result = 0;

    for (int i = numTotalBits; i > 0; i--)
    {
        range >>= 1;
        result <<= 1;

        if (code >= range) {
            code -= range;
            result |= 1;
        }

        /* uint32_t t; */
        /*
        t = (code - range) >> 31;
        t &= 1;
        code -= range & (t - 1);
        result = (result + result) | (1 - t);
        */

        if (range < kTopValue) {
            range <<= 8;
            code = (code << 8) | ReadByte;
        }
    }

    rd->Range = range;
    rd->Code = code;
    return result;
}

int RangeDecoderBitDecode(prob_t* prob, CRangeDecoder* rd)
{
    uint32_t bound = (rd->Range >> kNumBitModelTotalBits)* (*prob);
    if (rd->Code < bound)
    {
        rd->Range = bound;
        *prob += (kBitModelTotal - *prob) >> kNumMoveBits;
        if (rd->Range < kTopValue)
        {
            rd->Code = (rd->Code << 8) | ReadByte;
            rd->Range <<= 8;
        }
        return 0;
    }
    else
    {
        rd->Range -= bound;
        rd->Code -= bound;
        *prob -= (*prob) >> kNumMoveBits;
        if (rd->Range < kTopValue)
        {
            rd->Code = (rd->Code << 8) | ReadByte;
            rd->Range <<= 8;
        }
        return 1;
    }
}

int RangeDecoderBitTreeDecode(prob_t* probs, int numLevels, CRangeDecoder* rd)
{
    int mi = 1;
    for (int i = numLevels; i > 0; i--) {
        mi = (mi + mi) + RangeDecoderBitDecode(probs + mi, rd);
    }
    return mi - (1 << numLevels);
}

int RangeDecoderReverseBitTreeDecode(prob_t* probs, int numLevels, CRangeDecoder* rd)
{
    int mi = 1;
    int symbol = 0;
    for (int i = 0; i < numLevels; i++)
    {
        int bit = RangeDecoderBitDecode(probs + mi, rd);
        mi = mi + mi + bit;
        symbol |= (bit << i);
    }
    return symbol;
}

static uint8_t xz_literal_decode(prob_t* probs, CRangeDecoder* rd)
{
    int symbol = 1;
    do
    {
        symbol = (symbol + symbol) | RangeDecoderBitDecode(probs + symbol, rd);
    } while (symbol < 0x100);

    return symbol;
}

static uint8_t xz_literal_decode_match(prob_t* probs, CRangeDecoder* rd, uint8_t matchByte)
{
    int symbol = 1;
    int bit = 0, matchBit = 0;
    do
    {
        matchBit = (matchByte >> 7) & 1;
        matchByte <<= 1;
        bit = RangeDecoderBitDecode(probs + ((1 + matchBit) << 8) + symbol, rd);
        symbol = (symbol << 1) | bit;

        if (matchBit != bit)
        {
            while (symbol < 0x100) {
                symbol = (symbol + symbol) | RangeDecoderBitDecode(probs + symbol, rd);
            }
            break;
        }
    } while (symbol < 0x100);

    return symbol;
}

#define kNumPosBitsMax      4
#define kNumPosStatesMax   (1 << kNumPosBitsMax)
#define kLenNumLowBits      3
#define kLenNumLowSymbols  (1 << kLenNumLowBits)
#define kLenNumMidBits      3
#define kLenNumMidSymbols  (1 << kLenNumMidBits)
#define kLenNumHighBits     8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenChoice     0
#define LenChoice2   (LenChoice + 1)
#define LenLow       (LenChoice2 + 1)
#define LenMid       (LenLow + (kNumPosStatesMax << kLenNumLowBits))
#define LenHigh      (LenMid + (kNumPosStatesMax << kLenNumMidBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols) 

static int xz_len_decode(prob_t* p, CRangeDecoder* rd, int posState)
{
    if (RangeDecoderBitDecode(p + LenChoice, rd) == 0)
        return RangeDecoderBitTreeDecode(p + LenLow + (posState << kLenNumLowBits), kLenNumLowBits, rd);
    if (RangeDecoderBitDecode(p + LenChoice2, rd) == 0)
        return (kLenNumLowSymbols + RangeDecoderBitTreeDecode(p + LenMid + (posState << kLenNumMidBits), kLenNumMidBits, rd));

    return (kLenNumLowSymbols + kLenNumMidSymbols + RangeDecoderBitTreeDecode(p + LenHigh, kLenNumHighBits, rd));
}

#define kNumStates           12
#define kStartPosModelIndex  4
#define kEndPosModelIndex    14
#define kNumFullDistances   (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits      6
#define kNumLenToPosStates   4
#define kNumAlignBits        4
#define kAlignTableSize     (1 << kNumAlignBits)
#define kMatchMinLen         2

#define IsMatch      0
#define IsRep       (IsMatch + (kNumStates << kNumPosBitsMax))
#define IsRepG0     (IsRep + kNumStates)
#define IsRepG1     (IsRepG0 + kNumStates)
#define IsRepG2     (IsRepG1 + kNumStates)
#define IsRep0Long  (IsRepG2 + kNumStates)
#define PosSlot     (IsRep0Long + (kNumStates << kNumPosBitsMax))
#define SpecPos     (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define Align       (SpecPos + kNumFullDistances - kEndPosModelIndex)
#define LenCoder    (Align + kAlignTableSize)
#define RepLenCoder (LenCoder + kNumLenProbs)
#define Literal     (RepLenCoder + kNumLenProbs)

#if Literal != XZ_BASE_SIZE
    #error
#endif


static int xz_decode(uint8_t* buf, uint8_t* out_stream, uint32_t out_size, uint32_t* out_size_processed)
{
    XzVarState* vs = (XzVarState*)buf;
    prob_t* p = (prob_t*)(buf + sizeof(XzVarState));
    CRangeDecoder rd = vs->RangeDecoder;
    int state = vs->State;
    int previousIsMatch = vs->PreviousIsMatch;
    uint32_t rep0 = vs->Reps[0], rep1 = vs->Reps[1], rep2 = vs->Reps[2], rep3 = vs->Reps[3];
    uint32_t nowPos = 0, pos = 0;
    uint8_t previousByte = 0;

    uint32_t posStateMask = (1 << (vs->pb)) - 1;
    uint32_t literalPosMask = (1 << (vs->lp)) - 1;
    int lc = vs->lc;
    int remain_len = vs->RemainLen;
    uint32_t globalPos = vs->GlobalPos;

    uint8_t* dict = vs->Dictionary;
    uint32_t dict_size = vs->DictionarySize;
    uint32_t dictionaryPos = vs->DictionaryPos;

    if (remain_len == -1) {
        *out_size_processed = 0;
        return XZ_RET_OK;
    }

    while ((remain_len > 0) && (nowPos < out_size))
    {
        pos = dictionaryPos - rep0;
        if (pos >= dict_size)
            pos += dict_size;

        out_stream[nowPos++] = dict[dictionaryPos] = dict[pos];

        if (++dictionaryPos == dict_size)
            dictionaryPos = 0;

        remain_len--;
    }

    if (dictionaryPos == 0)
        previousByte = dict[dict_size - 1];
    else
        previousByte = dict[dictionaryPos - 1];

    *out_size_processed = 0;
    while (nowPos < out_size)
    {
        int posState = (int)((nowPos + globalPos) & posStateMask);

        if (rd.read_cb_ret != XZ_RET_OK)
            return rd.read_cb_ret;

        if (rd.ExtraBytes != 0)
            return XZ_RET_DATA_ERR;

        if (RangeDecoderBitDecode(p + IsMatch + (state << kNumPosBitsMax) + posState, &rd) == 0)
        {
            prob_t* probs = p + Literal + (XZ_LIT_SIZE * ((((nowPos + globalPos) & literalPosMask) << lc) + (previousByte >> (8 - lc))));

            if (state < 4) {
                state = 0;
            }
            else if (state < 10) {
                state -= 3;
            }
            else {
                state -= 6;
            }

            if (previousIsMatch)
            {
                pos = dictionaryPos - rep0;
                if (pos >= dict_size)
                    pos += dict_size;

                uint8_t matchByte = dict[pos];
                previousByte = xz_literal_decode_match(probs, &rd, matchByte);
                previousIsMatch = 0;
            }
            else {
                previousByte = xz_literal_decode(probs, &rd);
            }

            out_stream[nowPos++] = previousByte;

            dict[dictionaryPos] = previousByte;

            if (++dictionaryPos == dict_size)
                dictionaryPos = 0;
        }
        else
        {
            previousIsMatch = 1;
            if (RangeDecoderBitDecode(p + IsRep + state, &rd) == 1)
            {
                if (RangeDecoderBitDecode(p + IsRepG0 + state, &rd) == 0)
                {
                    if (RangeDecoderBitDecode(p + IsRep0Long + (state << kNumPosBitsMax) + posState, &rd) == 0)
                    {
                        if ((nowPos + globalPos) == 0)
                            return XZ_RET_DATA_ERR;

                        state = state < 7 ? 9 : 11;

                        pos = dictionaryPos - rep0;
                        if (pos >= dict_size)
                            pos += dict_size;

                        previousByte = dict[pos];
                        dict[dictionaryPos] = previousByte;

                        if (++dictionaryPos == dict_size)
                            dictionaryPos = 0;

                        out_stream[nowPos++] = previousByte;
                        continue;
                    }
                }
                else
                {
                    uint32_t distance;
                    if (RangeDecoderBitDecode(p + IsRepG1 + state, &rd) == 0)
                        distance = rep1;
                    else
                    {
                        if (RangeDecoderBitDecode(p + IsRepG2 + state, &rd) == 0)
                            distance = rep2;
                        else
                        {
                            distance = rep3;
                            rep3 = rep2;
                        }

                        rep2 = rep1;
                    }
                    rep1 = rep0;
                    rep0 = distance;
                }
                remain_len = xz_len_decode(p + RepLenCoder, &rd, posState);
                state = state < 7 ? 8 : 11;
            }
            else
            {
                int posSlot;
                rep3 = rep2;
                rep2 = rep1;
                rep1 = rep0;
                state = (state < 7) ? 7 : 10;
                remain_len = xz_len_decode(p + LenCoder, &rd, posState);

                posSlot = RangeDecoderBitTreeDecode(p + PosSlot + \
                    ((remain_len < kNumLenToPosStates ? remain_len : kNumLenToPosStates - 1) << kNumPosSlotBits), kNumPosSlotBits, &rd);

                if (posSlot >= kStartPosModelIndex)
                {
                    int numDirectBits = ((posSlot >> 1) - 1);
                    rep0 = ((2 | ((uint32_t)posSlot & 1)) << numDirectBits);

                    if (posSlot < kEndPosModelIndex) {
                        rep0 += RangeDecoderReverseBitTreeDecode((p + SpecPos + rep0 - posSlot - 1), numDirectBits, &rd);
                    }
                    else {
                        rep0 += RangeDecoderDecodeDirectBits(&rd, numDirectBits - kNumAlignBits) << kNumAlignBits;
                        rep0 += RangeDecoderReverseBitTreeDecode(p + Align, kNumAlignBits, &rd);
                    }
                }
                else {
                    rep0 = posSlot;
                }

                rep0++;
            }

            if (rep0 == (uint32_t)(0)) {
                remain_len = -1;/* it's for stream version */
                break;
            }

            if (rep0 > nowPos + globalPos) {
                return XZ_RET_DATA_ERR;
            }

            remain_len += kMatchMinLen;

            do
            {
                pos = dictionaryPos - rep0;
                if (pos >= dict_size) {
                    pos += dict_size;
                }

                previousByte = dict[pos];
                dict[dictionaryPos] = previousByte;

                if (++dictionaryPos == dict_size) {
                    dictionaryPos = 0;
                }

                out_stream[nowPos++] = previousByte;
                remain_len--;
            } while ((remain_len > 0) && (nowPos < out_size));
        }
    }

    vs->RangeDecoder = rd;
    vs->DictionaryPos = dictionaryPos;
    vs->GlobalPos = globalPos + nowPos;
    vs->Reps[0] = rep0;
    vs->Reps[1] = rep1;
    vs->Reps[2] = rep2;
    vs->Reps[3] = rep3;
    vs->State = state;
    vs->PreviousIsMatch = previousIsMatch;
    vs->RemainLen = remain_len;

    *out_size_processed = nowPos;
    return XZ_RET_OK;
}

char XZINTERNALDATA_MEMORY_ADDR[INTERNAL_DATA_BUF_SIZE] = { 0 };
char DICTIONARY_MEMORY_ADDR[DICT_BUF_SIZE] = { 0 };
char BLOCK_MEMORY_ADDR[BLOCK_BUF_SIZE] = { 0 };

int xz_decompress_buf_to_buf(uint8_t* header, write_cb_t* out_cb, read_cb_t* in_cb)
{
    uint32_t nowPos = 0, out_size = 0, out_size_processed = 0;
    uint8_t properties[5];
    uint8_t prop0;
    int lc, lp, pb;
    uint8_t b = 0;
    int ret = 0;

    uint8_t* dict = NULL;
    uint32_t dict_size = 0;
    uint8_t* xzInternalData = NULL;
    uint32_t xzInternalSize = 0;

    memcpy(properties, header, sizeof(properties));
    header += sizeof(properties);

    for (int i = 0; i < 4; i++)
    {
        b = *(uint8_t*)header;
        header += sizeof(b);
        out_size += (uint32_t)(b) << (i * 8);
    }

    if (out_size == 0xFFFFFFFF) {
        return XZ_RET_STREAM_VER_NOT_SUPP;
    }

    for (int i = 0; i < 4; i++)
    {
        b = *(uint8_t*)header;
        header += sizeof(b);
        if (b != 0) {
            return XZ_RET_FILE_TOO_LONG;
        }
    }

    prop0 = properties[0];
    if (prop0 >= (9 * 5 * 5)) {
        return XZ_RET_PROPERTIES_ERR;
    }

    for (pb = 0; prop0 >= (9 * 5); pb++, prop0 -= (9 * 5));
    for (lp = 0; prop0 >= 9; lp++, prop0 -= 9);
    lc = prop0;

    for (int i = 0; i < 4; i++) {
        dict_size += (uint32_t)(properties[1 + i]) << (i * 8);
    }

    //os_malloc(dict_size);
    dict = (void*)DICTIONARY_MEMORY_ADDR;
    if (dict == NULL) {
        return XZ_RET_MEM_ALLOC_FILED;
    }
    if (dict_size > sizeof(DICTIONARY_MEMORY_ADDR)) {
        return XZ_RET_NOT_ENOUGH_MEM;
    }
    memset(dict, 0, sizeof(DICTIONARY_MEMORY_ADDR));

    //os_malloc(xzInternalSize);
    xzInternalData = (void*)XZINTERNALDATA_MEMORY_ADDR;
    if (xzInternalData == NULL) {
        return XZ_RET_MEM_ALLOC_FILED;
    }
    xzInternalSize = (XZ_BASE_SIZE + (XZ_LIT_SIZE << (lc + lp))) * sizeof(prob_t);
    xzInternalSize += sizeof(XzVarState);
    if (xzInternalSize >= sizeof(XZINTERNALDATA_MEMORY_ADDR)) {
        return XZ_RET_NOT_ENOUGH_MEM;
    }
    memset(xzInternalData, 0, sizeof(XZINTERNALDATA_MEMORY_ADDR));

    //init param
    {
        XzVarState * vs = (XzVarState*)xzInternalData;
        prob_t * p = (prob_t*)(xzInternalData + sizeof(XzVarState));
        uint32_t numProbs = Literal + ((uint32_t)XZ_LIT_SIZE << (lc + lp));

        vs->Dictionary = dict;
        vs->DictionarySize = dict_size;
        vs->DictionaryPos = 0;
        vs->GlobalPos = 0;
        vs->Reps[0] = vs->Reps[1] = vs->Reps[2] = vs->Reps[3] = 1;
        vs->lc = lc;
        vs->lp = lp;
        vs->pb = pb;
        vs->State = 0;
        vs->PreviousIsMatch = 0;
        vs->RemainLen = 0;

        for (uint32_t i = 0; i < numProbs; i++) {
            p[i] = kBitModelTotal >> 1;
        }
        RangeDecoderInit(&vs->RangeDecoder, in_cb);
    }

    //decode
    for (nowPos = 0; nowPos < out_size;)
    {
        uint8_t* dest = (uint8_t*)BLOCK_MEMORY_ADDR;
        uint32_t blockSize = out_size - nowPos;
        if (blockSize > sizeof(BLOCK_MEMORY_ADDR)) {
            blockSize = sizeof(BLOCK_MEMORY_ADDR);
        }

        if (XZ_RET_OK != (ret = xz_decode((uint8_t*)xzInternalData, ((uint8_t*)dest), blockSize, &out_size_processed))) {
            return ret;
        }
        if (out_size_processed == 0) {
            out_size = nowPos;
            break;
        }
        out_cb->Write(nowPos, dest, out_size_processed);

        nowPos += out_size_processed;
    }

    return 0;
}





