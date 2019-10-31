/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_PACKET_H__
#define __AWSS_PACKET_H__
#include "infra_sha1.h"

#define MSG_REQ_ID_LEN                       (16)
#define TOPIC_LEN_MAX                        (128)
#define DEV_INFO_LEN_MAX                     (512)
#define DEV_SIGN_SIZE                        (SHA1_DIGEST_SIZE)

#define ILOP_VER                             "1.0"
#define AWSS_VER                             "{\"smartconfig\":\"2.0\",\"zconfig\":\"2.0\",\"router\":\"2.0\",\"ap\":\"2.0\",\"softap\":\"2.0\"}"

#define TOPIC_AWSS_WIFILIST                  "/sys/%s/%s/awss/event/wifilist/get"
#define TOPIC_AWSS_SWITCHAP                  "/sys/%s/%s/awss/device/switchap"
#define TOPIC_AWSS_GETDEVICEINFO_MCAST       "/sys/awss/device/info/get"
#define TOPIC_AWSS_GETDEVICEINFO_UCAST       "/sys/%s/%s/awss/device/info/get"
#define TOPIC_AWSS_GET_CONNECTAP_INFO_MCAST  "/sys/awss/device/connectap/info/get"
#define TOPIC_AWSS_GET_CONNECTAP_INFO_UCAST  "/sys/%s/%s/awss/device/connectap/info/get"
#define TOPIC_GETDEVICEINFO_MCAST            "/sys/device/info/get"
#define TOPIC_GETDEVICEINFO_UCAST            "/sys/%s/%s/device/info/get"
#define TOPIC_POST_STATIS                    "/sys/%s/%s/thing/log/post"
#define TOPIC_AWSS_NOTIFY                    "/sys/awss/device/info/notify"
#define TOPIC_AWSS_CONNECTAP_NOTIFY          "/sys/awss/event/connectap/notify"
#define TOPIC_NOTIFY                         "/sys/device/info/notify"
#define TOPIC_SWITCHAP                       "/sys/%s/%s/thing/awss/device/switchap"
#define TOPIC_SWITCHAP_REPLY                 "/sys/%s/%s/thing/awss/device/switchap_reply"
#define TOPIC_ZC_ENROLLEE                    "/sys/%s/%s/thing/awss/enrollee/found"
#define TOPIC_ZC_ENROLLEE_REPLY              "/sys/%s/%s/thing/awss/enrollee/found_reply"
#define TOPIC_ZC_CHECKIN                     "/sys/%s/%s/thing/awss/enrollee/checkin"
#define TOPIC_ZC_CHECKIN_REPLY               "/sys/%s/%s/thing/awss/enrollee/checkin_reply"
#define TOPIC_ZC_CIPHER                      "/sys/%s/%s/thing/cipher/get"
#define TOPIC_ZC_CIPHER_REPLY                "/sys/%s/%s/thing/cipher/get_reply"
#define TOPIC_MATCH_REPORT                   "/sys/%s/%s/thing/awss/enrollee/match"
#define TOPIC_MATCH_REPORT_REPLY             "/sys/%s/%s/thing/awss/enrollee/match_reply"
#define TOPIC_AWSS_DEV_AP_SWITCHAP           "/sys/awss/device/softap/switchap"

#define METHOD_DEV_INFO_NOTIFY               "device.info.notify"
#define METHOD_AWSS_DEV_INFO_NOTIFY          "awss.device.info.notify"
#define METHOD_AWSS_CONNECTAP_NOTIFY         "awss.event.connectap.notify"
#define METHOD_AWSS_DEV_AP_SWITCHAP          "awss.device.softap.switchap"
#define METHOD_EVENT_ZC_SWITCHAP             "thing.awss.device.switchap"
#define METHOD_EVENT_ZC_ENROLLEE             "thing.awss.enrollee.found"
#define METHOD_EVENT_ZC_CHECKIN              "thing.awss.enrollee.checkin"
#define METHOD_EVENT_ZC_CIPHER               "thing.cipher.get"
#define METHOD_MATCH_REPORT                  "thing.awss.enrollee.match"
#define METHOD_LOG_POST                      "things.log.post"

#define AWSS_ACK_FMT                         "{\"id\":%s,\"code\":%d,\"data\":%s}"
#define AWSS_REQ_FMT                         "{\"id\":%s,\"version\":\"%s\",\"method\":\"%s\",\"params\":%s}"
#define AWSS_JSON_PARAM                      "params"
#define AWSS_JSON_CODE                       "code"
#define AWSS_JSON_ID                         "id"
#define AWSS_STATIS_FMT                      "{\"template\":\"timestamp logLevel module traceContext logContent\",\"contents\":[\"%u %s %s %u %s\"]}"

enum {
    AWSS_CMP_PKT_TYPE_REQ = 1,
    AWSS_CMP_PKT_TYPE_RSP,
};

void produce_random(uint8_t *random, uint32_t len);
char *awss_build_sign_src(char *sign_src, int *sign_src_len);
void *awss_build_dev_info(int type, void *dev_info, int info_len);
const char *awss_build_topic(const char *topic_fmt, char *topic, uint32_t tlen);
int awss_build_packet(int type, void *id, void *ver, void *method, void *data, int code, void *pkt, int *pkt_len);

#endif
