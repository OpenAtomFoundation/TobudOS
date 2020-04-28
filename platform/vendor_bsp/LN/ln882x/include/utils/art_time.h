#ifndef __ART_TIME_H_
#define __ART_TIME_H_
#include <stdbool.h>
#include <time.h>
bool art_timestamp_to_string(time_t timestamp, char *timestr);
time_t art_string_to_timestamp(char *timestr);
time_t art_tm_to_timestamp(struct tm *ptm);
bool art_timestamp_to_tm(time_t timestamp, struct tm *ptm);
#endif /* __ART_TIME_H_ */

