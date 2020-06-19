#include <string.h>
#include <ctype.h>
#include "utils/art_string.h"
#include "hal/hal_trng.h"
#include "utils/debug/log.h"
#include "utils/debug/art_assert.h"

#define LN_MAC_OUI          "00-50-C2"

/***************************************************************
 *  函 数 名：substring
 *  函数功能：获得字符串中指定位置的子串
 *  参    数：
 *            strSource：源字符串
 *            strDest：  子串的输出buffer
 *            startPos：子串在源串的起始位置
 *            endPos：  子串在源串的结束位置
 *  返 回 值：
 *            如果成功，则返回true
 *            如果失败，则返回false
***************************************************************/

bool substing(const char *strSource, char *strDest, const int startPos, const int endPos)
{
    bool ret = true;
    int len = 0;

    if(strSource && strDest && (startPos <= endPos)) {
        len = ((strlen(strSource) - startPos) > (endPos - startPos)) ? (endPos - startPos) : (strlen(strSource) - startPos);
        strncpy(strDest, strSource + startPos, len);
        strDest[len] = '\0';
        return ret;
    } else {
        return false;
    }
}

unsigned char char2hex(char c)  
{
    unsigned char ch = 0;
    switch (c) {
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '0':
                ch = c - '0';
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                ch = c - 'A' + 10;
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                ch = c - 'a' + 10;
                break;
            default:
                LOG(LOG_LVL_ERROR, "wrong char %c\n", c);
                //exit(0);
                break;
        }
    return ch;  
}  

bool str2mac(const unsigned char *macstr, unsigned char *mac)    
{    
    const unsigned char *tmp = macstr;    
    int i;  

    for (i = 0; i < 6; ++i){    
        mac[i] = char2hex(*tmp++) * 16;    
        mac[i] += char2hex(*tmp++);
        tmp++;
    }       
    return 0;  
}
unsigned char *str2hex(unsigned char *dest, const char *src)
{
    unsigned char *tmp = dest;
    unsigned char a;
    int i = 0;

    while ('\0' != src[i]) {
        a = char2hex(src[i]);
        if (0 == i++ % 2){
            *dest = (a << 4) & 0XF0;
        }else{
            *dest++ += a & 0X0F;
        }
    }

    return tmp;
}

bool is_valid_mac_string(const char *macstr)
{
    bool ret = true;
    int len = 0, num_count = 0, colon_count = 0;
    const char *start = NULL, *end = NULL;

    if (macstr == NULL || strlen(macstr) < 17) {
        return false;
    }

    len = strlen(macstr);
    start = macstr;
    end = macstr + len - 1;

    while (end > start && isspace(*end)) {
        --end;
    }

    while (start < end && isspace(*start)) {
        ++start;
    }

    if ((end - start) != 16 || isxdigit(*start) == 0 || isxdigit(*end) == 0) {
        return false;
    }

    while (start <= end) {
        while (start <= end && isxdigit(*start)) {
            ++start;
            if (++num_count > 2) {
                ret = false;
                break;
            }
        }

        if (start <= end ) {
            if (*start == ':' && num_count == 2) {
                ++colon_count;
                num_count = 0;
                ++start;
            } else {
                ret = false;
                break;
            }
        } else {
            break;
        }
    }

    if (ret == 0 && colon_count != 5) {
        ret = false;
    }

    return ret;
}

bool is_valid_mac(const char *mac)
{
    bool ret = true;
    unsigned char temp[6] = {0,};

    if (mac == NULL) {
        return false;
    }

    memset(temp, 0x00, 6);
    if(!memcmp(mac, temp, 6)) {
        return false;
    }
    memset(temp, 0xFF, 6);
    if(!memcmp(mac, temp, 6)) {
        return false;
    }

    return ret;
}

bool art_string_extract_domain_content(char *domain_begin, char *domain_end, char domain_separator, char *flag, char *dest_buffer)
{

    char *domain_separator_addr = NULL, *base = domain_begin;
    int start, end;

    ART_ASSERT(domain_begin && domain_end && dest_buffer && (domain_begin <= domain_end));

	if(domain_begin == domain_end){
		return true;
	}
	
    domain_separator_addr = strchr(domain_begin, domain_separator);

    if(!domain_separator_addr){
        domain_separator_addr = domain_end;
    }

	if(domain_separator_addr && domain_separator_addr == domain_end){
        if(flag){
            domain_begin = strchr(domain_begin, *flag);
            if(domain_begin > domain_separator_addr){
                goto out;
            }
            domain_begin = domain_begin + 1;
            if(domain_begin > domain_separator_addr){
                goto out;
            }
            domain_end = strchr(domain_begin, *flag);
            if(domain_begin > domain_separator_addr){
                goto out;
            }
        }
        start = domain_begin - base;
        end = domain_end - base;
        if(!substing(base, (char *)dest_buffer, start, end)){
            goto out;
        }
        return true;
    }
out:
    return false;
}
bool mem_is_zero(const void *addr, size_t size)
{
    unsigned char *_addr = (unsigned char *)addr;
    int i;

    ART_ASSERT(_addr);
    for(i = 0; i < size; i++){
        if(_addr[i]){
            return false;
        }
    }
    return false;
}

int generate_mac_randomly(unsigned char *addr)
{
    TRNG_InitStruct initStruct;
    uint32_t trngNumber[2] = {0};

    if(!addr){
        return -1;
    }
    initStruct.srcLength = Length_Long;
    initStruct.sampleCnt = 0x3F;
    HAL_TRNG_Init(TRNG, initStruct);
    HAL_TRNG_Start(TRNG);
    while(!HAL_TRNG_isDataReady(TRNG) || HAL_TRNG_isBusy(TRNG));
#ifdef LN_MAC_OUI
    uint32_t addr0, addr1, addr2;
    sscanf(LN_MAC_OUI, "%X-%X-%X", &addr0, &addr1, &addr2);
    *addr = addr0 & 0xFF;
    *(addr + 1) = addr1 & 0xFF;
    *(addr + 2) = addr2 & 0xFF;
    for (int i = 0; i < 1; i++) {
        trngNumber[i] = HAL_TRNG_GetRandomNumber(TRNG, i);
    }
    HAL_TRNG_Stop(TRNG);
    memcpy(addr + 3, trngNumber, 3);
#else
    for (int i = 0; i < 2; i++) {
        trngNumber[i] = HAL_TRNG_GetRandomNumber(TRNG, i);
    }
    HAL_TRNG_Stop(TRNG);
    memcpy(addr, trngNumber, 6);
    CLR_BIT(addr[0],0);
    CLR_BIT(addr[0],1);
#endif
    return 0;
}

