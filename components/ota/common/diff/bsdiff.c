/*-
 * Copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsdiff.h"

#include "wstream.h"
#include "lzma_compress.h"

#define ERROR(msg)                                                                          \
    printf("ERROR: line number[%d], function[%s] msg[%s]\n", __LINE__, __FUNCTION__, msg);  \
    rc = -1;                                                                                \
    goto OUT;

#define MIN(x, y)           (((x) < (y)) ? (x) : (y))

#define FOLD_N(x, n)        ((n) * x)

static void split(int32_t *I, int32_t *V, int32_t start, int32_t len, int32_t h)
{
    int32_t i, j, k, x, tmp, jj, kk;

    if (len < 16) {
        for (k = start; k < start + len; k += j) {
            j = 1; x = V[I[k] + h];
            for (i = 1; k + i < start + len; i++) {
                if (V[I[k + i] + h] < x) {
                    x = V[I[k + i] + h];
                    j = 0;
                }
                if(V[I[k + i] + h] == x) {
                    tmp = I[k + j]; I[k + j] = I[k + i]; I[k + i] = tmp;
                    j++;
                }
            }
            for (i = 0; i < j; i++) {
                V[I[k + i]] = k + j - 1;
            }
            if (j == 1) {
                I[k] = -1;
            }
        }
        return;
    }

    x = V[I[start + len / 2] + h];
    jj = 0; kk = 0;
    for (i = start; i < start + len; i++) {
        if (V[I[i] + h] < x) {
            jj++;
        }
        if (V[I[i] + h] == x) {
            kk++;
        }
    }
    jj += start; kk += jj;

    i = start; j = 0; k = 0;
    while (i < jj) {
        if (V[I[i] + h] < x) {
            i++;
        } else if (V[I[i] + h] == x) {
            tmp = I[i]; I[i] = I[jj + j]; I[jj + j] = tmp;
            j++;
        } else {
            tmp = I[i]; I[i] = I[kk + k]; I[kk + k] = tmp;
            k++;
        }
    }

    while (jj + j < kk) {
        if (V[I[jj + j] + h] == x) {
            j++;
        } else {
            tmp = I[jj + j]; I[jj + j] = I[kk + k]; I[kk + k] = tmp;
            k++;
        }
    }

    if (jj > start) {
        split(I, V, start, jj - start, h);
    }

    for (i = 0; i < kk - jj; i++) {
        V[I[jj + i]] = kk - 1;
    }
    if (jj == kk - 1) {
        I[jj] = -1;
    }

    if (start + len > kk) {
        split(I, V, kk, start + len - kk, h);
    }
}

static void qsufsort(int32_t *I, int32_t *V, uint8_t *old, int32_t oldsize)
{
    int32_t buckets[256];
    int32_t i, h, len;

    for (i = 0; i < 256; i++) {
        buckets[i] = 0;
    }
    for (i = 0; i < oldsize; i++) {
        buckets[old[i]]++;
    }
    for (i = 1; i < 256; i++) {
        buckets[i] += buckets[i - 1];
    }
    for (i = 255; i > 0; i--) {
        buckets[i] = buckets[i - 1];
    }
    buckets[0] = 0;

    for (i = 0; i < oldsize; i++) {
        I[++buckets[old[i]]] = i;
    }
    I[0] = oldsize;
    for (i = 0; i < oldsize; i++) {
        V[i] = buckets[old[i]];
    }
    V[oldsize] = 0;
    for (i = 1; i < 256; i++) {
        if (buckets[i] == buckets[i - 1] + 1) {
            I[buckets[i]] = -1;
        }
    }
    I[0] = -1;

    for (h = 1; I[0] != -(oldsize + 1); h += h) {
        len = 0;
        for (i = 0; i < oldsize + 1; ) {
            if (I[i] < 0) {
                len -= I[i];
                i -= I[i];
            } else {
                if (len) {
                    I[i - len] = -len;
                }
                len = V[I[i]] + 1 - i;
                split(I, V, i, len, h);
                i += len;
                len = 0;
            };
        };
        if (len) {
            I[i - len] = -len;
        }
    };

    for (i = 0; i < oldsize + 1; i++) {
        I[V[i]] = i;
    }
}

static int32_t matchlen(uint8_t *old, int32_t oldsize, uint8_t *new, int32_t newsize)
{
    int32_t i;

    for (i = 0; (i < oldsize) && (i < newsize); i++) {
        if (old[i] != new[i]) {
            break;
        }
    }

    return i;
}

static int32_t search(int32_t *I, uint8_t *old, int32_t oldsize, uint8_t *new, int32_t newsize, int32_t st, int32_t en, int32_t *pos)
{
    int32_t x,y;

    if (en - st < 2) {
        x = matchlen(old + I[st], oldsize - I[st], new, newsize);
        y = matchlen(old + I[en], oldsize - I[en], new, newsize);

        if (x > y) {
            *pos = I[st];
            return x;
        } else {
            *pos = I[en];
            return y;
        }
    };

    x = st + (en - st) / 2;
    if (memcmp(old + I[x], new, MIN(oldsize - I[x], newsize)) < 0) {
        return search(I, old, oldsize, new, newsize, x, en, pos);
    } else {
        return search(I, old, oldsize, new, newsize, st, x, pos);
    }
}

static void offtout(int32_t x, uint8_t *buf)
{
    int32_t y;

    if (x < 0) {
        y = -x;
    } else {
        y = x;
    }

    buf[0] = y % 256; y -= buf[0];
    y = y / 256; buf[1] = y % 256; y -= buf[1];
    y = y / 256; buf[2] = y % 256; y -= buf[2];
    y = y / 256; buf[3] = y % 256;

    if (x < 0) {
        buf[3] |= 0x80;
    }
}

int bsdiff(uint8_t *old, size_t oldsize, uint8_t *new, size_t newsize, uint8_t **patch, size_t *patchsize)
{
    int rc = 0;
    int32_t i;

    int32_t *I = NULL, *V;
    int32_t scan, pos = 0, len;
    int32_t lastscan, lastpos, lastoffset;
    int32_t oldscore, scsc;
    int32_t s, Sf, lenf, Sb, lenb;
    int32_t overlap, Ss, lens;
    int32_t dblen, eblen;
    uint8_t *db = NULL, *eb = NULL;
    uint8_t buf[FOLD_N(sizeof(int32_t) / sizeof(uint8_t), 1)];
    uint8_t header[FOLD_N(sizeof(int32_t) / sizeof(uint8_t), 3)];

    wstream_t wstream;
    uint8_t *wstream_buf, *the_patch;
    size_t wstream_buf_size, the_patchsize;

    wstream_buf_size = FOLD_N(newsize, 5);

    if ((wstream_buf = malloc(wstream_buf_size)) == NULL) {
        ERROR("malloc failed");
    }

    wstream_create(&wstream, wstream_buf, wstream_buf_size);

    if (((I = malloc((oldsize + 1) * sizeof(int32_t))) == NULL) ||
            ((V = malloc((oldsize + 1) * sizeof(int32_t))) == NULL)) {
        ERROR("malloc failed");
    }

    qsufsort(I, V, old, oldsize);

    free(V);

    if (((db = malloc(newsize + 1)) == NULL) ||
        ((eb = malloc(newsize + 1)) == NULL)) {
        ERROR("malloc failed");
    }
    dblen = 0;
    eblen = 0;

    /* Header is
       8    8   length of ctrl block
       16   8   length of diff block
       24   8   length of new file */
    /* File is
       0    32  Header
       32   ??  ctrl block
       ??   ??  diff block
       ??   ??  extra block */
    offtout(0,          header + FOLD_N(sizeof(int32_t), 0));
    offtout(0,          header + FOLD_N(sizeof(int32_t), 1));
    offtout(newsize,    header + FOLD_N(sizeof(int32_t), 2));

    if (wstream_write_stream(&wstream, header, sizeof(header)) != 0) {
        ERROR("wstream full");
    }

    /* Compute the differences, writing ctrl as we go */
    scan = 0; len = 0;
    lastscan = 0; lastpos = 0; lastoffset = 0;
    while (scan < newsize) {
        oldscore = 0;

        for (scsc = scan += len; scan < newsize; scan++) {
            len = search(I, old, oldsize, new + scan, newsize - scan, 0, oldsize, &pos);

            for (; scsc < scan + len; scsc++) {
                if ((scsc + lastoffset < oldsize) &&
                    (old[scsc + lastoffset] == new[scsc])) {
                    oldscore++;
                }
            }

            if (((len == oldscore) && (len != 0)) ||
                (len > oldscore + 8)) {
                    break;
            }

            if ((scan + lastoffset < oldsize) &&
                (old[scan + lastoffset] == new[scan])) {
                oldscore--;
            }
        }

        if ((len != oldscore) || (scan == newsize)) {
            s = 0; Sf = 0; lenf = 0;
            for (i = 0; (lastscan + i < scan) && (lastpos + i < oldsize); ) {
                if (old[lastpos + i] == new[lastscan + i]) {
                    s++;
                }
                i++;
                if (s * 2 - i > Sf * 2 - lenf) {
                    Sf = s; lenf = i;
                }
            }

            lenb = 0;
            if (scan < newsize) {
                s = 0; Sb = 0;
                for (i = 1; (scan >= lastscan + i) && (pos >= i); i++) {
                    if (old[pos - i] == new[scan - i]) {
                        s++;
                    }
                    if (s * 2 - i > Sb * 2 - lenb) {
                        Sb = s; lenb = i;
                    }
                }
            }

            if (lastscan + lenf > scan - lenb) {
                overlap = (lastscan + lenf) - (scan - lenb);
                s = 0; Ss = 0; lens = 0;
                for (i = 0; i < overlap; i++) {
                    if (new[lastscan + lenf - overlap + i] == old[lastpos + lenf - overlap + i]) {
                        s++;
                    }
                    if (new[scan - lenb + i] == old[pos - lenb + i]) {
                        s--;
                    }
                    if (s > Ss) {
                        Ss = s; lens = i + 1;
                    }
                }

                lenf += lens - overlap;
                lenb -= lens;
            }

            for (i = 0; i < lenf; i++) {
                db[dblen + i] = new[lastscan + i] - old[lastpos + i];
            }
            for (i = 0; i < (scan - lenb) - (lastscan + lenf); i++) {
                eb[eblen + i] = new[lastscan + lenf + i];
            }

            dblen += lenf;
            eblen += (scan - lenb) - (lastscan + lenf);

            offtout(lenf, buf);
            if (wstream_write_stream(&wstream, buf, sizeof(buf)) != 0) {
                ERROR("wstream full");
            }

            offtout((scan - lenb) - (lastscan + lenf), buf);
            if (wstream_write_stream(&wstream, buf, sizeof(buf)) != 0) {
                ERROR("wstream full");
            }

            offtout((pos - lenb) - (lastpos + lenf), buf);
            if (wstream_write_stream(&wstream, buf, sizeof(buf)) != 0) {
                ERROR("wstream full");
            }

            lastscan    = scan  - lenb;
            lastpos     = pos   - lenb;
            lastoffset  = pos   - scan;
        };
    };

    /* Compute size of ctrl data */
    /* ATTENTION: we DONNOT do data compress here */
    len = wstream_length_get(&wstream);
    if (len == -1) {
        ERROR("wstream invalid");
    }

    /* write size of ctrl data */
    offtout(len - sizeof(header), header + FOLD_N(sizeof(int32_t), 0));

    /* Write diff data */
    /* ATTENTION: we DONNOT do data compress here */
    if (wstream_write_stream(&wstream, db, dblen) != 0) {
        ERROR("wstream full");
    }

    /* Compute size of diff data */
    /* ATTENTION: we DONNOT do data compress here */
    newsize = wstream_length_get(&wstream);
    if (newsize == -1) {
        ERROR("wstream invalid");
    }

    /* write size of diff data */
    offtout(newsize - len, header + FOLD_N(sizeof(int32_t), 1));

    /* Write extra data */
    /* ATTENTION: we DONNOT do data compress here */
    if (wstream_write_stream(&wstream, eb, eblen) != 0) {
        ERROR("wstream full");
    }

    /* write header */
    if (wstream_write_stream_at(&wstream, 0, header, sizeof(header)) != 0) {
        ERROR("wsream full");
    }

    the_patchsize = wstream_length_get(&wstream);
    if (the_patchsize == -1) {
        ERROR("wstream invalid!\n");
    }

    if ((the_patch = malloc(the_patchsize)) == NULL) {
        ERROR("malloc failed");
    }

    memcpy(the_patch, wstream_buf_get(&wstream), the_patchsize);
    *patch      = the_patch;
    *patchsize  = the_patchsize;

OUT:
    /* Free the memory we used */
    if (db) {
        free(db);
    }
    if (eb) {
        free(eb);
    }
    if (I) {
        free(I);
    }

    if (wstream_buf) {
        free(wstream_buf);
        wstream_destroy(&wstream);
    }

    if (rc) {
        *patch      = NULL;
        *patchsize  = 0;
    }

    return rc;
}

