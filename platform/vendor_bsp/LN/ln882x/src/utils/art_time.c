#include <stdio.h>
#include <string.h>
#include "utils/art_time.h"
//#include "utils/debug/log.h"
//#include "utils/debug/art_assert.h"

bool art_timestamp_to_string(time_t timestamp, char *timestr)
{
    struct tm *ptm = NULL;
    
    if(timestr == NULL){
        return false;
    }
    ptm = localtime(&timestamp);
    if(ptm == NULL){
        return false;
    }
    
    sprintf(timestr, "%04d-%02d-%02d %02d:%02d:%02d", (ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday, (ptm->tm_hour + 8), ptm->tm_min, ptm->tm_sec);
    return true;
}
time_t art_string_to_timestamp(char *timestr)
{
    struct tm tmp_tm = {0,};
    
    sscanf(timestr, "%04d-%02d-%02d %02d:%02d:%02d", &(tmp_tm.tm_year), &(tmp_tm.tm_mon), &(tmp_tm.tm_mday), &(tmp_tm.tm_hour), &(tmp_tm.tm_min), &(tmp_tm.tm_sec));
    tmp_tm.tm_year -= 1900;
    tmp_tm.tm_mon--;
    tmp_tm.tm_hour -= 8;
    return mktime(&tmp_tm);
}
time_t art_tm_to_timestamp(struct tm *ptm)
{
    return mktime(ptm);
}
bool art_timestamp_to_tm(time_t timestamp, struct tm *ptm)
{
    struct tm *tmp = NULL;
    tmp = localtime(&timestamp);
    if(tmp == NULL){
        return false;
    }
    memcpy(ptm, tmp, sizeof(struct tm));
    return true;
}

