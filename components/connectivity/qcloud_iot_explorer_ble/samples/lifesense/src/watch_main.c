/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Watch sample application for the following profiles:
 *            Time profile client
 *            Alert Notification profile client
 *            Phone Alert Status profile client
 *
 *  Copyright (c) 2011-2019 Arm Ltd.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#include <string.h>
#include "wsf_types.h"
#include "util/bstream.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "wsf_assert.h"
#include "hci_api.h"
#include "dm_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_cfg.h"
#include "app_api.h"
#include "app_db.h"
#include "app_ui.h"
#include "app_main.h"
#include "svc_core.h"
#include "svc_ch.h"
#include "gatt/gatt_api.h"
#include "tipc/tipc_api.h"
#include "anpc/anpc_api.h"
#include "paspc/paspc_api.h"
#include "hrpc/hrpc_api.h"
#include "dis/dis_api.h"

#include "ble_qiot_llsync_device.h"
#include "ble_qiot_service.h"
#include "ble_qiot_import.h"
#include "ble_qiot_config.h"
#include "apollo3_service.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/
typedef enum
{
    WATCH_SCAN_NONE,
    WATCH_SCAN_START,
    WATCH_SCANNING
} eWatchScanState;
/*! application control block */
static struct
{
    uint16_t hdlMasterList[APP_DB_HDL_LIST_LEN]; /*! Cached handle list in master role */
    uint16_t hdlSlaveList[APP_DB_HDL_LIST_LEN];  /*! Cached handle list in slave role */
    wsfHandlerId_t handlerId;                    /*! WSF hander ID */
    uint8_t discState;                           /*! Service discovery state */
    eWatchScanState scan_state;
    bool_t autoConnect; /*! TRUE if auto-connecting */
} watchCb;

/*! \brief application connection information */
typedef struct
{
    appDbHdl_t dbHdl; /*! Device database record handle type */
    uint8_t addrType; /*! Type of address of device to connect to */
    bdAddr_t addr;    /*! Address of device to connect to */
    bool_t doConnect; /*! TRUE to issue connect on scan complete */
    int8_t rssi;      /*! RSSI of discovered remote device  */
} watchConnInfo_t;

watchConnInfo_t watchConnInfo;

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/

/*! configurable parameters for advertising */
static const appAdvCfg_t watchAdvCfg =
    {
        {0, 0, 0},    /*! Advertising durations in ms */
        {160, 800, 0} /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static const appSlaveCfg_t watchSlaveCfg =
    {
        1, /*! Maximum connections */
};

/*! configurable parameters for security */
static const appSecCfg_t watchSecCfg =
    {
        DM_AUTH_BOND_FLAG, /*! Authentication and bonding flags */
        0,                 /*! Initiator key distribution flags */
        DM_KEY_DIST_LTK,   /*! Responder key distribution flags */
        FALSE,             /*! TRUE if Out-of-band pairing data is present */
        FALSE              /*! TRUE to initiate security upon connection */
};

/*! configurable parameters for connection parameter update */
static const appUpdateCfg_t watchUpdateCfg =
    {
        0,   /*! Connection idle period in ms before attempting
                                              connection parameter update; set to zero to disable */
        600, /*! Minimum connection interval in 1.25ms units */
        800, /*! Maximum connection interval in 1.25ms units */
        3,   /*! Connection latency */
        600, /*! Supervision timeout in 10ms units */
        5    /*! Number of update attempts before giving up */
};

/*! Connection parameters */
static const hciConnSpec_t watchConnCfg =
    {
        160, /*! Minimum connection interval in 1.25ms units */
        160, /*! Maximum connection interval in 1.25ms units */
        0,   /*! Connection latency */
        600, /*! Supervision timeout in 10ms units */
        0,   /*! Unused */
        0    /*! Unused */
};

/*! SMP security parameter configuration */
/* Configuration structure */
static const smpCfg_t watchSmpCfg =
    {
        3000,                /*! 'Repeated attempts' timeout in msec */
        SMP_IO_NO_IN_NO_OUT, /*! I/O Capability */
        7,                   /*! Minimum encryption key length */
        16,                  /*! Maximum encryption key length */
        3,                   /*! Attempts to trigger 'repeated attempts' timeout */
        0,                   /*! Device authentication requirements */
        64000,               /*! Maximum repeated attempts timeout in msec */
        64000,               /*! Time msec before attemptExp decreases */
        2                    /*! Repeated attempts multiplier exponent */
};

/*! Configurable parameters for service and characteristic discovery */
static const appDiscCfg_t watchDiscCfg =
    {
        FALSE, /*! TRUE to wait for a secure connection before initiating discovery */
        FALSE  /*! TRUE to fall back on database hash to verify handles when no bond exists. */
};

static const appCfg_t watchAppCfg =
    {
        TRUE, /*! TRUE to abort service discovery if service not found */
        TRUE  /*! TRUE to disconnect if ATT transaction times out */
};

/*! configurable parameters for master */
static const appMasterCfg_t watchMasterCfg =
    {
        96,                 /*! The scan interval, in 0.625 ms units */
        48,                 /*! The scan window, in 0.625 ms units  */
        4000,               /*! The scan duration in ms */
        DM_DISC_MODE_NONE,  /*! The GAP discovery mode */
        DM_SCAN_TYPE_ACTIVE /*! The scan type (active or passive) */
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! advertising data, discoverable mode */
static const uint8_t watchAdvDataDisc[] =
    {
        /*! flags */
        2,                        /*! length */
        DM_ADV_TYPE_FLAGS,        /*! AD type */
        DM_FLAG_LE_LIMITED_DISC | /*! flags */
            DM_FLAG_LE_BREDR_NOT_SUP,

        /*! tx power */
        2,                    /*! length */
        DM_ADV_TYPE_TX_POWER, /*! AD type */
        0,                    /*! tx power */

        /*! manufacturer specific data */
        3,                           /*! length */
        DM_ADV_TYPE_MANUFACTURER,    /*! AD type */
        UINT16_TO_BYTES(HCI_ID_ARM), /*! company ID */

        /*! service solicitation UUID list */
        7,                      /*! length */
        DM_ADV_TYPE_16_SOLICIT, /*! AD type */
        UINT16_TO_BYTES(ATT_UUID_CURRENT_TIME_SERVICE),
        UINT16_TO_BYTES(ATT_UUID_ALERT_NOTIF_SERVICE),
        UINT16_TO_BYTES(ATT_UUID_PHONE_ALERT_SERVICE)};

/*! scan data, discoverable mode */
static const uint8_t watchScanDataDisc[] =
    {
        /*! device name */
        6,                      /*! length */
        DM_ADV_TYPE_LOCAL_NAME, /*! AD type */
        'W',
        'a',
        't',
        'c',
        'h'};

/**************************************************************************************************
  ATT Client Discovery Data
**************************************************************************************************/

/*! Discovery states:  enumeration of services to be discovered */
enum
{
    WATCH_DISC_SLAVE_GATT_SVC, /* GATT service */
    WATCH_DISC_SLAVE_CTS_SVC,  /* Current Time service */
    WATCH_DISC_SLAVE_ANS_SVC,  /* Alert Notification service */
    WATCH_DISC_SLAVE_PASS_SVC, /* Phone Alert Status service */
    WATCH_DISC_SLAVE_SVC_MAX   /* Discovery complete */
};

/*! Discovery states:  enumeration of services to be discovered in the master role */
enum
{
    WATCH_DISC_MASTER_GATT_SVC, /* GATT service */
    WATCH_DISC_MASTER_DIS_SVC,  /* Device Information service */
    WATCH_DISC_MASTER_HRS_SVC,  /* Heart Rate service */
};

/*! the Client handle list, watchCb.hdlList[], is set as follows:
 *
 *  ------------------------------- <- WATCH_DISC_GATT_START
 *  | GATT handles                |
 *  |...                          |
 *  ------------------------------- <- WATCH_DISC_CTS_START
 *  | CTS handles                 |
 *  | ...                         |
 *  ------------------------------- <- WATCH_DISC_ANS_START
 *  | ANS handles                 |
 *  | ...                         |
 *  ------------------------------- <- WATCH_DISC_PASS_START
 *  | PASS handles                |
 *  | ...                         |
 *  -------------------------------
 */

/*! Start of each service's handles in the the handle list */
#define WATCH_DISC_GATT_START 0
#define WATCH_DISC_CTS_START (WATCH_DISC_GATT_START + GATT_HDL_LIST_LEN)
#define WATCH_DISC_ANS_START (WATCH_DISC_CTS_START + TIPC_CTS_HDL_LIST_LEN)
#define WATCH_DISC_PASS_START (WATCH_DISC_ANS_START + ANPC_ANS_HDL_LIST_LEN)
#define WATCH_DISC_SLAVE_HDL_LIST_LEN (WATCH_DISC_PASS_START + PASPC_PASS_HDL_LIST_LEN)

/*! Pointers into handle list for each service's handles */
static uint16_t *pWatchSlvGattHdlList = &watchCb.hdlSlaveList[WATCH_DISC_GATT_START];
static uint16_t *pWatchCtsHdlList = &watchCb.hdlSlaveList[WATCH_DISC_CTS_START];
static uint16_t *pWatchAnsHdlList = &watchCb.hdlSlaveList[WATCH_DISC_ANS_START];
static uint16_t *pWatchPassHdlList = &watchCb.hdlSlaveList[WATCH_DISC_PASS_START];

/* Start of cached heart rate service handles; begins after GATT - Master Role */
#define WATCH_DISC_DISC_START (WATCH_DISC_GATT_START + GATT_HDL_LIST_LEN)
#define WATCH_DISC_HRS_START (WATCH_DISC_DISC_START + DIS_HDL_LIST_LEN)
#define WATCH_DISC_MASTER_HDL_LIST_LEN (WATCH_DISC_HRS_START + HRPC_HRS_HDL_LIST_LEN)

/*! Pointers into handle list heart rate service handles - Master Role */
static uint16_t *pWatchMstGattHdlList = &watchCb.hdlMasterList[WATCH_DISC_GATT_START];
static uint16_t *pWatchDisHdlList = &watchCb.hdlMasterList[WATCH_DISC_DISC_START];
static uint16_t *pWatchHrsHdlList = &watchCb.hdlMasterList[WATCH_DISC_HRS_START];

/* sanity check:  make sure handle list length is <= app db handle list length */
WSF_CT_ASSERT(WATCH_DISC_SLAVE_HDL_LIST_LEN <= APP_DB_HDL_LIST_LEN);
WSF_CT_ASSERT(WATCH_DISC_MASTER_HDL_LIST_LEN <= APP_DB_HDL_LIST_LEN);

/**************************************************************************************************
  ATT Client Configuration Data
**************************************************************************************************/

/*
 * Data for configuration after service discovery
 */

/* Default value for CCC indications */
static const uint8_t watchCccIndVal[] = {UINT16_TO_BYTES(ATT_CLIENT_CFG_INDICATE)};

/* Default value for CCC notifications */
static const uint8_t watchCccNtfVal[] = {UINT16_TO_BYTES(ATT_CLIENT_CFG_NOTIFY)};

/* Default value for Client Supported Features (enable Robust Caching) */
static const uint8_t watchCsfVal[1] = {ATTS_CSF_ROBUST_CACHING};

/* ANS Control point value for "Enable New Alert Notification" */
static const uint8_t watchAncpEnNewVal[] = {CH_ANCP_ENABLE_NEW, CH_ALERT_CAT_ID_ALL};

/* ANS Control point value for "Notify New Alert Immediately" */
static const uint8_t watchAncpNotNewVal[] = {CH_ANCP_NOTIFY_NEW, CH_ALERT_CAT_ID_ALL};

/* ANS Control point value for "Enable Unread Alert Status Notification" */
static const uint8_t watchAncpEnUnrVal[] = {CH_ANCP_ENABLE_UNREAD, CH_ALERT_CAT_ID_ALL};

/* ANS Control point value for "Notify Unread Alert Status Immediately" */
static const uint8_t watchAncpNotUnrVal[] = {CH_ANCP_NOTIFY_UNREAD, CH_ALERT_CAT_ID_ALL};

/* HRS Control point "Reset Energy Expended" */
static const uint8_t watchHrsRstEnExp[] = {CH_HRCP_RESET_ENERGY_EXP};

/* List of characteristics to configure after service discovery */
static const attcDiscCfg_t watchDiscSlaveCfgList[] =
    {
        /* Read:  CTS Current time */
        {NULL, 0, (TIPC_CTS_CT_HDL_IDX + WATCH_DISC_CTS_START)},

        /* Read:  CTS Local time information */
        {NULL, 0, (TIPC_CTS_LTI_HDL_IDX + WATCH_DISC_CTS_START)},

        /* Read:  CTS Reference time information */
        {NULL, 0, (TIPC_CTS_RTI_HDL_IDX + WATCH_DISC_CTS_START)},

        /* Read:  ANS Supported new alert category */
        {NULL, 0, (ANPC_ANS_SNAC_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Read:  ANS Supported unread alert category */
        {NULL, 0, (ANPC_ANS_SUAC_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Read:  PASS Alert status */
        {NULL, 0, (PASPC_PASS_AS_HDL_IDX + WATCH_DISC_PASS_START)},

        /* Read:  PASS Ringer setting */
        {NULL, 0, (PASPC_PASS_RS_HDL_IDX + WATCH_DISC_PASS_START)},

        /* Write:  GATT service changed ccc descriptor */
        {watchCccIndVal, sizeof(watchCccIndVal), (GATT_SC_CCC_HDL_IDX + WATCH_DISC_GATT_START)},

        /* Write:  GATT client supported features */
        {watchCsfVal, sizeof(watchCsfVal), (GATT_CSF_HDL_IDX + WATCH_DISC_GATT_START)},

        /* Write:  CTS Current time ccc descriptor */
        {watchCccNtfVal, sizeof(watchCccNtfVal), (TIPC_CTS_CT_CCC_HDL_IDX + WATCH_DISC_CTS_START)},

        /* Write:  ANS New alert ccc descriptor */
        {watchCccNtfVal, sizeof(watchCccNtfVal), (ANPC_ANS_NA_CCC_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Unread alert status ccc descriptor */
        {watchCccNtfVal, sizeof(watchCccNtfVal), (ANPC_ANS_UAS_CCC_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  PASS Alert status ccc descriptor */
        {watchCccNtfVal, sizeof(watchCccNtfVal), (PASPC_PASS_AS_CCC_HDL_IDX + WATCH_DISC_PASS_START)},

        /* Write:  PASS Ringer setting ccc descriptor */
        {watchCccNtfVal, sizeof(watchCccNtfVal), (PASPC_PASS_RS_CCC_HDL_IDX + WATCH_DISC_PASS_START)},

        /* Write:  ANS Control point "Enable New Alert Notification" */
        {watchAncpEnNewVal, sizeof(watchAncpEnNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Control point "Notify New Alert Immediately" */
        {watchAncpNotNewVal, sizeof(watchAncpNotNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Control point "Enable Unread Alert Status Notification" */
        {watchAncpEnUnrVal, sizeof(watchAncpEnUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Control point "Notify Unread Alert Status Immediately" */
        {watchAncpNotUnrVal, sizeof(watchAncpNotUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)}};

/* Characteristic configuration list length */
#define WATCH_DISC_SLAVE_CFG_LIST_LEN (sizeof(watchDiscSlaveCfgList) / sizeof(attcDiscCfg_t))

/* sanity check:  make sure configuration list length is <= handle list length */
WSF_CT_ASSERT(WATCH_DISC_SLAVE_CFG_LIST_LEN <= WATCH_DISC_SLAVE_HDL_LIST_LEN);

/* List of characteristics to configure after service discovery - Master Role */
static const attcDiscCfg_t watchDiscMasterCfgList[] =
    {
        /* Write:  GATT client supported features */
        {watchCsfVal, sizeof(watchCsfVal), (GATT_CSF_HDL_IDX + WATCH_DISC_GATT_START)},

        /* Read:  DIS Manufacturer name string */
        {NULL, 0, DIS_MFNS_HDL_IDX},

        /* Read:  DIS Model number string */
        {NULL, 0, DIS_MNS_HDL_IDX},

        /* Read:  DIS Serial number string */
        {NULL, 0, DIS_SNS_HDL_IDX},

        /* Read:  DIS Hardware revision string */
        {NULL, 0, DIS_HRS_HDL_IDX},

        /* Read:  DIS Firmware revision string */
        {NULL, 0, DIS_FRS_HDL_IDX},

        /* Read:  DIS Software revision string */
        {NULL, 0, DIS_SRS_HDL_IDX},

        /* Read:  DIS System ID */
        {NULL, 0, DIS_SID_HDL_IDX},

        /* Read:  HRS Body sensor location */
        {NULL, 0, HRPC_HRS_BSL_HDL_IDX},

        /* Write:  HRS Control point "Reset Energy Expended"  */
        {watchHrsRstEnExp, sizeof(watchHrsRstEnExp), HRPC_HRS_HRCP_HDL_IDX + WATCH_DISC_HRS_START},

        /* Write:  HRS Heart rate measurement CCC descriptor  */
        {watchCccNtfVal, sizeof(watchCccNtfVal), HRPC_HRS_HRM_CCC_HDL_IDX + WATCH_DISC_HRS_START},
};

/* Characteristic configuration list length */
#define WATCH_DISC_MASTER_CFG_LIST_LEN (sizeof(watchDiscMasterCfgList) / sizeof(attcDiscCfg_t))

/* sanity check:  make sure configuration list length is <= handle list length */
WSF_CT_ASSERT(WATCH_DISC_MASTER_CFG_LIST_LEN <= WATCH_DISC_MASTER_CFG_LIST_LEN);

/*
 * Data for configuration on connection setup
 */

/* List of characteristics to configure on connection setup */
static const attcDiscCfg_t watchDiscConnCfgList[] =
    {
        /* Read:  ANS Supported new alert category */
        {NULL, 0, (ANPC_ANS_SNAC_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Read:  ANS Supported unread alert category */
        {NULL, 0, (ANPC_ANS_SUAC_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Read:  PASS Alert status */
        {NULL, 0, (PASPC_PASS_AS_HDL_IDX + WATCH_DISC_PASS_START)},

        /* Write:  ANS Control point "Enable New Alert Notification" */
        {watchAncpEnNewVal, sizeof(watchAncpEnNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Control point "Notify New Alert Immediately" */
        {watchAncpNotNewVal, sizeof(watchAncpNotNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Control point "Enable Unread Alert Status Notification" */
        {watchAncpEnUnrVal, sizeof(watchAncpEnUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

        /* Write:  ANS Control point "Notify Unread Alert Status Immediately" */
        {watchAncpNotUnrVal, sizeof(watchAncpNotUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)}};

/* Characteristic configuration list length */
#define WATCH_DISC_CONN_CFG_LIST_LEN (sizeof(watchDiscConnCfgList) / sizeof(attcDiscCfg_t))

/* sanity check:  make sure configuration list length is <= handle list length */
WSF_CT_ASSERT(WATCH_DISC_CONN_CFG_LIST_LEN <= WATCH_DISC_SLAVE_HDL_LIST_LEN);

/**************************************************************************************************
  ATT Server Data
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors used in local ATT server */
enum
{
    WATCH_GATT_SC_CCC_IDX, /*! GATT service, service changed characteristic */
    QIOT_EVENT_CCC_IDX,   // add QIOT_EVENT_CCC_IDX
    WATCH_NUM_CCC_IDX      /*! Number of ccc's */
};

/*! client characteristic configuration descriptors settings, indexed by ccc enumeration */
static const attsCccSet_t watchCccSet[WATCH_NUM_CCC_IDX] =
{
    /* cccd handle         value range                 security level */
    {GATT_SC_CH_CCC_HDL, ATT_CLIENT_CFG_INDICATE, DM_SEC_LEVEL_ENC}, /* WATCH_GATT_SC_CCC_IDX */

    {QIOT_EVENT_CCC_HDL, ATT_CLIENT_CFG_NOTIFY, DM_SEC_LEVEL_NONE}, // QIOT_EVENT_CCC_IDX
};

extern ble_timer_cb p_ble_timer_cb;

enum
{
    BLE_QIOT_TIMER_IND = 0x600, // be careful do not repeat with other event handle
};

/**
 * @brief Get the handler object of WSF event handler for the application
 * 
 * @return wsfHandlerId_t 
 */
wsfHandlerId_t get_wsf_evt_handler(void)
{
    return watchCb.handlerId;
}


/*************************************************************************************************/
/*!
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchDmCback(dmEvt_t *pDmEvt)
{
    dmEvt_t *pMsg;
    uint16_t len;
    uint16_t reportLen;

    len = DmSizeOfEvt(pDmEvt);

    if (pDmEvt->hdr.event == DM_SCAN_REPORT_IND)
    {
        reportLen = pDmEvt->scanReport.len;
    }
    else
    {
        reportLen = 0;
    }

    if ((pMsg = WsfMsgAlloc(len + reportLen)) != NULL)
    {
        memcpy(pMsg, pDmEvt, len);

        if (pDmEvt->hdr.event == DM_SCAN_REPORT_IND)
        {
            pMsg->scanReport.pData = (uint8_t *)((uint8_t *)pMsg + len);
            memcpy(pMsg->scanReport.pData, pDmEvt->scanReport.pData, reportLen);
        }

        WsfMsgSend(watchCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application  ATT callback.
 *
 *  \param  pEvt    ATT callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchAttCback(attEvt_t *pEvt)
{
    attEvt_t *pMsg;

    if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL)
    {
        memcpy(pMsg, pEvt, sizeof(attEvt_t));
        pMsg->pValue = (uint8_t *)(pMsg + 1);
        memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
        WsfMsgSend(watchCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Perform actions on scan start.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchScanStart(dmEvt_t *pMsg)
{
    if (pMsg->hdr.status == HCI_SUCCESS)
    {
        watchCb.scan_state = WATCH_SCANNING;
        // set it to lowest possible value of RSSI
        watchConnInfo.rssi = -128;
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Perform actions on scan stop.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchScanStop(dmEvt_t *pMsg)
{
    if (pMsg->hdr.status == HCI_SUCCESS)
    {
        watchCb.scan_state = WATCH_SCAN_NONE;
        watchCb.autoConnect = FALSE;

        /* Open connection */
        if (watchConnInfo.doConnect)
        {
            AppConnOpen(watchConnInfo.addrType, watchConnInfo.addr, watchConnInfo.dbHdl);
            watchConnInfo.doConnect = FALSE;
        }
        else
        {
            AppAdvStart(APP_MODE_AUTO_INIT);
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Handle a scan report.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchScanReport(dmEvt_t *pMsg)
{
    uint8_t device_name[32] = {0};
    uint32_t name_length;
    uint8_t *pData;
    appDbHdl_t dbHdl;
    bool_t connect = FALSE;

    /* disregard if not scanning or autoconnecting */
    if ((watchCb.scan_state == WATCH_SCAN_NONE) || !watchCb.autoConnect)
    {
        return;
    }

    if ((pData = DmFindAdType(DM_ADV_TYPE_LOCAL_NAME, pMsg->scanReport.len,
                              pMsg->scanReport.pData)))
    {
        name_length = *pData;
        name_length--;

        memcpy(device_name, pData + 2, name_length);
    }
    else if ((pData = DmFindAdType(DM_ADV_TYPE_SHORT_NAME, pMsg->scanReport.len,
                                   pMsg->scanReport.pData)))
    {
        name_length = *pData;
        name_length--;

        memcpy(device_name, pData + 2, name_length);
    }

    WsfTrace("Found: 0x%02x:%02x:%02x:%02x:%02x:%02x rssi: %d %s",
             pMsg->scanReport.addr[5],
             pMsg->scanReport.addr[4],
             pMsg->scanReport.addr[3],
             pMsg->scanReport.addr[2],
             pMsg->scanReport.addr[1],
             pMsg->scanReport.addr[0],
             pMsg->scanReport.rssi,
             device_name);

    /* if we already have a bond with this device then connect to it */
    if ((dbHdl = AppDbFindByAddr(pMsg->scanReport.addrType, pMsg->scanReport.addr)) != APP_DB_HDL_NONE)
    {
        /* if this is a directed advertisement where the initiator address is an RPA */
        if (DM_RAND_ADDR_RPA(pMsg->scanReport.directAddr, pMsg->scanReport.directAddrType))
        {
            /* resolve direct address to see if it's addressed to us */
            AppMasterResolveAddr(pMsg, dbHdl, APP_RESOLVE_DIRECT_RPA);
        }
        else
        {
            connect = TRUE;
        }
    }
    /* if the peer device uses an RPA */
    else if (DM_RAND_ADDR_RPA(pMsg->scanReport.addr, pMsg->scanReport.addrType))
    {
        /* resolve advertiser's RPA to see if we already have a bond with this device */
        AppMasterResolveAddr(pMsg, APP_DB_HDL_NONE, APP_RESOLVE_ADV_RPA);
    }
    else
    {
        /* find Service UUID list; if full list not found search for partial */
        if ((pData = DmFindAdType(DM_ADV_TYPE_16_UUID, pMsg->scanReport.len,
                                  pMsg->scanReport.pData)) == NULL)
        {
            pData = DmFindAdType(DM_ADV_TYPE_16_UUID_PART, pMsg->scanReport.len,
                                 pMsg->scanReport.pData);
        }

        /* if found and length checks out ok */
        if (pData != NULL && pData[DM_AD_LEN_IDX] >= (ATT_16_UUID_LEN + 1))
        {
            uint8_t len = pData[DM_AD_LEN_IDX] - 1;
            pData += DM_AD_DATA_IDX;

            while ((!connect) && (len >= ATT_16_UUID_LEN))
            {
                /* Connect if heart rate service is included */
                if (BYTES_UINT16_CMP(pData, ATT_UUID_HEART_RATE_SERVICE))
                {
                    connect = TRUE;
                    break;
                }

                pData += ATT_16_UUID_LEN;
                len -= ATT_16_UUID_LEN;
            }
        }
    }

    if (connect)
    {

        if (pMsg->scanReport.rssi > watchConnInfo.rssi)
        {
            watchConnInfo.rssi = pMsg->scanReport.rssi;

            /* connect will start after scanning stops with expiration
       * of scan timer.
       */

            /* Store peer information for connect on scan stop */
            watchConnInfo.addrType = DmHostAddrType(pMsg->scanReport.addrType);
            memcpy(watchConnInfo.addr, pMsg->scanReport.addr, sizeof(bdAddr_t));
            watchConnInfo.dbHdl = dbHdl;
            watchConnInfo.doConnect = TRUE;
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application ATTS client characteristic configuration callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchCccCback(attsCccEvt_t *pEvt)
{
    attsCccEvt_t *pMsg;
    appDbHdl_t dbHdl;

    /* If CCC not set from initialization and there's a device record and currently bonded */
    if ((pEvt->handle != ATT_HANDLE_NONE) &&
        ((dbHdl = AppDbGetHdl((dmConnId_t)pEvt->hdr.param)) != APP_DB_HDL_NONE) &&
        AppCheckBonded((dmConnId_t)pEvt->hdr.param))
    {
        /* Store value in device database. */
        AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
    }

    if ((pMsg = WsfMsgAlloc(sizeof(attsCccEvt_t))) != NULL)
    {
        memcpy(pMsg, pEvt, sizeof(attsCccEvt_t));
        WsfMsgSend(watchCb.handlerId, pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Perform UI actions on connection open.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchOpen(dmEvt_t *pMsg)
{
    if (pMsg->connOpen.role == DM_ROLE_SLAVE)
    {
        /* if not bonded send a security request on connection open; devices must pair before
     * service discovery will be initiated
     */
        if (AppDbCheckBonded() == FALSE)
        {
            DmSecSlaveReq((dmConnId_t)pMsg->hdr.param, pAppSecCfg->auth);
        }
    }
    else
    {

        AppAdvStart(APP_MODE_AUTO_INIT);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Set up advertising and other procedures that need to be performed after
 *          device reset.
 *
 *  \param  pMsg    Pointer to DM callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchSetup(dmEvt_t *pMsg)
{
    /* set advertising and scan response data for discoverable mode */
    AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, sizeof(watchAdvDataDisc), (uint8_t *)watchAdvDataDisc);
    AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, sizeof(watchScanDataDisc), (uint8_t *)watchScanDataDisc);

    /* set advertising and scan response data for connectable mode */
    AppAdvSetData(APP_ADV_DATA_CONNECTABLE, 0, NULL);
    AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, 0, NULL);

    /* start advertising; automatically set connectable/discoverable mode and bondable mode */
    AppAdvStart(APP_MODE_AUTO_INIT);

    DmConnSetConnSpec((hciConnSpec_t *)&watchConnCfg);
}

/*************************************************************************************************/
/*!
 *  \brief  Process a received ATT read response, notification, or indication when connected in
 *          the slave role.
 *
 *  \param  pMsg    Pointer to ATT callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchSlaveValueUpdate(attEvt_t *pMsg)
{
    if (pMsg->hdr.status == ATT_SUCCESS)
    {
        /* determine which profile the handle belongs to; start with most likely */

        /* alert notification */
        if (AnpcAnsValueUpdate(pWatchAnsHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }
        /* phone alert status */
        if (PaspcPassValueUpdate(pWatchPassHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }

        /* current time */
        if (TipcCtsValueUpdate(pWatchCtsHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }

        /* GATT */
        if (GattValueUpdate(pWatchSlvGattHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Process a received ATT read response, notification, or indication when connected in
 *          the master role to a heart rate profile sensor.
 *
 *  \param  pMsg    Pointer to ATT callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchMasterValueUpdate(attEvt_t *pMsg)
{
    if (pMsg->hdr.status == ATT_SUCCESS)
    {
        /* determine which profile the handle belongs to; start with most likely */

        /* heart rate */
        if (HrpcHrsValueUpdate(pWatchHrsHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }

        /* device information */
        if (DisValueUpdate(pWatchDisHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }

        /* GATT */
        if (GattValueUpdate(pWatchMstGattHdlList, pMsg) == ATT_SUCCESS)
        {
            return;
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Loop through the open connection ID list and find the first connection for the given role
 *
 *  \param  pConnIdList    Buffer with the connection IDs (must be DM_CONN_MAX bytes).
 *  \param  role           Role to look for.
 *
 *  \return None.
 */
/*************************************************************************************************/
static dmConnId_t watchGetConnIdByRole(dmConnId_t *pConnIdList, uint8_t role)
{
    uint8_t i;

    for (i = 0; i < DM_CONN_MAX; i++)
    {
        if (pConnIdList[i] != DM_CONN_ID_NONE)
        {
            if (DmConnRole(pConnIdList[i]) == role)
            {
                return pConnIdList[i];
            }
        }
    }

    return DM_CONN_ID_NONE;
}

/*************************************************************************************************/
/*!
 *  \brief  Button press callback.
 *
 *  \param  btn    Button press.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchBtnCback(uint8_t btn)
{
    dmConnId_t slaveConnId, masterConnId;
    static uint8_t ringer = CH_RCP_SILENT;
    dmConnId_t connIdList[DM_CONN_MAX];

    AppConnOpenList(connIdList);

    slaveConnId = watchGetConnIdByRole(connIdList, DM_ROLE_SLAVE);
    masterConnId = watchGetConnIdByRole(connIdList, DM_ROLE_MASTER);

    APP_TRACE_INFO3("btn: %d - master conn id: %d - slave conn id: %d", btn, masterConnId, slaveConnId);

    // as long as button 1 is short pressed, we'll scan and connect to
    // a  new fit device.
    if (1)
    {
        /* No connection as a master */
        switch (btn)
        {
        case APP_UI_BTN_1_SHORT:
            /* if scanning cancel scanning */
            if (watchCb.scan_state != WATCH_SCAN_NONE)
            {
                AppScanStop();
            }
            /* else auto connect */
            else if (!watchCb.autoConnect)
            {
                watchCb.autoConnect = TRUE;
                watchConnInfo.doConnect = FALSE;
                watchCb.scan_state = WATCH_SCAN_START;

                if (AppSlaveIsAdvertising())
                {
                    AppAdvStop();
                }
                else
                {
                    AppScanStart(watchMasterCfg.discMode, watchMasterCfg.scanType,
                                 watchMasterCfg.scanDuration);
                }
            }
            return;

        case APP_UI_BTN_1_LONG:
            /* clear bonded device info */
            AppClearAllBondingInfo();
            return;

        default:
            break;
        }
    }

    if (slaveConnId == DM_CONN_ID_NONE)
    {
        /* No connection as a slave */
        switch (btn)
        {
        case APP_UI_BTN_2_SHORT:
            /* start or restart advertising */
            AppAdvStart(APP_MODE_AUTO_INIT);
            return;

        case APP_UI_BTN_2_MED:
            /* enter discoverable and bondable mode mode */
            AppSetBondable(TRUE);
            AppAdvStart(APP_MODE_DISCOVERABLE);
            return;

        case APP_UI_BTN_2_LONG:
            /* clear all bonding info */
            AppSlaveClearAllBondingInfo();

            /* restart advertising */
            AppAdvStart(APP_MODE_AUTO_INIT);
            return;

        default:
            break;
        }
    }
    else
    {
        /* Active connection to a slave */
        switch (btn)
        {
        case APP_UI_BTN_2_SHORT:
            /* mute ringer once */
            PaspcPassControl(slaveConnId, pWatchPassHdlList[PASPC_PASS_RCP_HDL_IDX], CH_RCP_MUTE_ONCE);
            return;

        case APP_UI_BTN_2_MED:
            /* toggle between silencing ringer and enabling ringer */
            PaspcPassControl(slaveConnId, pWatchPassHdlList[PASPC_PASS_RCP_HDL_IDX], ringer);
            ringer = (ringer == CH_RCP_SILENT) ? CH_RCP_CANCEL_SILENT : CH_RCP_SILENT;
            return;

        case APP_UI_BTN_2_LONG:
            /* disconnect slave connection */
            AppConnClose(slaveConnId);
            break;

        default:
            break;
        }
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Discovery callback.
 *
 *  \param  connId    Connection identifier.
 *  \param  status    Service or configuration status.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchDiscCback(dmConnId_t connId, uint8_t status)
{
    switch (status)
    {
    case APP_DISC_INIT:
        /* set handle list when initialization requested */
        if (DmConnRole(connId) == DM_ROLE_MASTER)
        {
            AppDiscSetHdlList(connId, WATCH_DISC_MASTER_HDL_LIST_LEN, watchCb.hdlMasterList);
        }
        else
        {
            AppDiscSetHdlList(connId, WATCH_DISC_SLAVE_HDL_LIST_LEN, watchCb.hdlSlaveList);
        }
        break;

    case APP_DISC_READ_DATABASE_HASH:
        /* Read peer's database hash */
        AppDiscReadDatabaseHash(connId);
        break;

    case APP_DISC_SEC_REQUIRED:
        /* request security */
        if (DmConnRole(connId) == DM_ROLE_MASTER)
        {
            AppMasterSecurityReq(connId);
        }
        else
        {
            AppSlaveSecurityReq(connId);
        }
        break;

    case APP_DISC_START:
        /* initialize discovery state */
        if (DmConnRole(connId) == DM_ROLE_MASTER)
        {
            /* discover GATT service - master role */
            watchCb.discState = WATCH_DISC_MASTER_GATT_SVC;
            GattDiscover(connId, pWatchMstGattHdlList);
        }
        else
        {
            /* discover GATT service - slave role */
            watchCb.discState = WATCH_DISC_SLAVE_GATT_SVC;
            GattDiscover(connId, pWatchSlvGattHdlList);
        }

        break;

    case APP_DISC_FAILED:
    case APP_DISC_CMPL:
        /* next discovery state */
        watchCb.discState++;

        if (DmConnRole(connId) == DM_ROLE_MASTER)
        {
            if (watchCb.discState == WATCH_DISC_MASTER_DIS_SVC)
            {
                /* discover device information service */
                DisDiscover(connId, pWatchDisHdlList);
            }
            else if (watchCb.discState == WATCH_DISC_MASTER_HRS_SVC)
            {
                /* discover heart rate service */
                HrpcHrsDiscover(connId, pWatchHrsHdlList);
            }
            else
            {
                /* discovery complete */
                AppDiscComplete(connId, APP_DISC_CMPL);

                /* start configuration */
                AppDiscConfigure(connId, APP_DISC_CFG_START, WATCH_DISC_MASTER_CFG_LIST_LEN,
                                 (attcDiscCfg_t *)watchDiscMasterCfgList,
                                 WATCH_DISC_MASTER_HDL_LIST_LEN, watchCb.hdlMasterList);
            }
        }
        else
        {
            if (watchCb.discState == WATCH_DISC_SLAVE_CTS_SVC)
            {
                /* discover current time service */
                TipcCtsDiscover(connId, pWatchCtsHdlList);
            }
            else if (watchCb.discState == WATCH_DISC_SLAVE_ANS_SVC)
            {
                /* discover alert notification service */
                AnpcAnsDiscover(connId, pWatchAnsHdlList);
            }
            else if (watchCb.discState == WATCH_DISC_SLAVE_PASS_SVC)
            {
                /* discover phone alert status service */
                PaspcPassDiscover(connId, pWatchPassHdlList);
            }
            else
            {
                /* discovery complete */
                AppDiscComplete(connId, APP_DISC_CMPL);

                /* start configuration */
                AppDiscConfigure(connId, APP_DISC_CFG_START, WATCH_DISC_SLAVE_CFG_LIST_LEN,
                                 (attcDiscCfg_t *)watchDiscSlaveCfgList,
                                 WATCH_DISC_SLAVE_HDL_LIST_LEN, watchCb.hdlSlaveList);
            }
        }
        break;

    case APP_DISC_CFG_START:
        if (DmConnRole(connId) == DM_ROLE_MASTER)
        {
            /* start configuration */
            AppDiscConfigure(connId, APP_DISC_CFG_START, WATCH_DISC_MASTER_CFG_LIST_LEN,
                             (attcDiscCfg_t *)watchDiscMasterCfgList,
                             WATCH_DISC_MASTER_HDL_LIST_LEN, watchCb.hdlMasterList);
        }
        else
        {
            /* start configuration */
            AppDiscConfigure(connId, APP_DISC_CFG_START, WATCH_DISC_SLAVE_CFG_LIST_LEN,
                             (attcDiscCfg_t *)watchDiscSlaveCfgList,
                             WATCH_DISC_SLAVE_HDL_LIST_LEN, watchCb.hdlSlaveList);
        }
        break;

    case APP_DISC_CFG_CMPL:
        AppDiscComplete(connId, status);
        break;

    case APP_DISC_CFG_CONN_START:
        if (DmConnRole(connId) == DM_ROLE_SLAVE)
        {
            /* start connection setup configuration */
            AppDiscConfigure(connId, APP_DISC_CFG_CONN_START, WATCH_DISC_CONN_CFG_LIST_LEN,
                             (attcDiscCfg_t *)watchDiscConnCfgList,
                             WATCH_DISC_CONN_CFG_LIST_LEN, watchCb.hdlSlaveList);
        }
        break;

    default:
        break;
    }
}


void qiot_notify_enable(dmConnId_t connId)
{
    extern dmConnId_t ind_conn_id;
    ind_conn_id = connId;
    return;
}

void qiot_notify_disable(dmConnId_t connId)
{
    extern dmConnId_t ind_conn_id;
    ind_conn_id = DM_CONN_ID_NONE;
    return;
}

static void qiot_proc_ccc_state(attsCccEvt_t *pMsg)
{
    // APP_TRACE_INFO3("ccc state ind value:%d handle:%d idx:%d", pMsg->ccc.value, pMsg->ccc.handle, pMsg->ccc.idx);
    SEGGER_RTT_printf(0, "ccc state ind value:%d handle:%d idx:%d \r\n", pMsg->value, pMsg->handle, pMsg->idx);

    if (pMsg->idx == QIOT_EVENT_CCC_IDX)
    {
        if (pMsg->value == ATT_CLIENT_CFG_NOTIFY)
        {
            qiot_notify_enable((dmConnId_t)pMsg->hdr.param);
        }
        else
        {
            qiot_notify_disable((dmConnId_t)pMsg->hdr.param);
        }
        return;
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchProcMsg(dmEvt_t *pMsg)
{
    uint8_t uiEvent = APP_UI_NONE;

    SEGGER_RTT_printf(0, "get event %d\r\n", __LINE__, pMsg->hdr.event);

    switch (pMsg->hdr.event)
    {
    case ATTC_READ_RSP:
    case ATTC_HANDLE_VALUE_NTF:
    case ATTC_HANDLE_VALUE_IND:
        if (DmConnRole((dmConnId_t)pMsg->hdr.param) == DM_ROLE_MASTER)
        {
            watchMasterValueUpdate((attEvt_t *)pMsg);
        }
        else
        {
            watchSlaveValueUpdate((attEvt_t *)pMsg);
        }
        break;

    case BLE_QIOT_TIMER_IND:
        SEGGER_RTT_printf(0, "get timer event\r\n");
        if(NULL != p_ble_timer_cb)
        {
            p_ble_timer_cb(NULL);
        }
        break;

    case ATTS_CCC_STATE_IND:
        qiot_proc_ccc_state((attsCccEvt_t *)pMsg);
        break;
    case ATTS_HANDLE_VALUE_CNF:
        if(ATT_SUCCESS != pMsg->hdr.status)
        {
            // see att_defs.h
            SEGGER_RTT_printf(0, "event %d, error status %d \r\n", pMsg->hdr.event, pMsg->hdr.status);
        }
        break;

    case ATT_MTU_UPDATE_IND:
        // APP_TRACE_INFO1("Negotiated MTU %d", ((attEvt_t *)pMsg)->mtu);
        SEGGER_RTT_printf(0, "Negotiated MTU %d \r\n", ((attEvt_t *)pMsg)->mtu);
        break;

    case DM_PHY_UPDATE_IND:
        SEGGER_RTT_printf(0, "GAP PHY UPDATE REQUEST not support yet!\r\n");
        break;

    case DM_RESET_CMPL_IND:
        AttsCalculateDbHash();
        // watchSetup(pMsg);

        ble_qiot_advertising_start();
        DmConnSetConnSpec((hciConnSpec_t *)&watchConnCfg);

        uiEvent = APP_UI_RESET_CMPL;
        break;

    case DM_ADV_START_IND:
        uiEvent = APP_UI_ADV_START;
        break;

    case DM_ADV_STOP_IND:
        if (watchCb.scan_state == WATCH_SCAN_START)
        {
            AppScanStart(watchMasterCfg.discMode, watchMasterCfg.scanType,
                         watchMasterCfg.scanDuration);
        }
        else
        {
            AppAdvStart(APP_MODE_AUTO_INIT);
            uiEvent = APP_UI_ADV_STOP;
        }
        break;

    case DM_SCAN_START_IND:
        watchScanStart(pMsg);
        uiEvent = APP_UI_SCAN_START;
        break;

    case DM_SCAN_STOP_IND:
        watchScanStop(pMsg);
        uiEvent = APP_UI_SCAN_STOP;
        break;

    case DM_SCAN_REPORT_IND:
        watchScanReport(pMsg);
        break;

    case DM_CONN_OPEN_IND:
        watchOpen(pMsg);
        uiEvent = APP_UI_CONN_OPEN;
        ble_gap_connect_cb();
        break;

    case DM_CONN_CLOSE_IND:
        // only when the connection we as slave is closed
        ble_gap_disconnect_cb();
        if (pMsg->connOpen.role == DM_ROLE_SLAVE)
        {
            AppAdvStart(APP_MODE_AUTO_INIT);
        }
        uiEvent = APP_UI_CONN_CLOSE;
        break;

    case DM_SEC_PAIR_CMPL_IND:
        DmSecGenerateEccKeyReq();
        uiEvent = APP_UI_SEC_PAIR_CMPL;
        break;

    case DM_SEC_PAIR_FAIL_IND:
        DmSecGenerateEccKeyReq();
        uiEvent = APP_UI_SEC_PAIR_FAIL;
        break;

    case DM_SEC_ENCRYPT_IND:
        uiEvent = APP_UI_SEC_ENCRYPT;
        break;

    case DM_SEC_ENCRYPT_FAIL_IND:
        uiEvent = APP_UI_SEC_ENCRYPT_FAIL;
        break;

    case DM_SEC_AUTH_REQ_IND:
        AppHandlePasskey(&pMsg->authReq);
        break;

    case DM_PRIV_CLEAR_RES_LIST_IND:
        APP_TRACE_INFO1("Clear resolving list status 0x%02x", pMsg->hdr.status);
        break;

    case DM_VENDOR_SPEC_CMD_CMPL_IND:
    {
#if defined(AM_PART_APOLLO) || defined(AM_PART_APOLLO2)

        uint8_t *param_ptr = &pMsg->vendorSpecCmdCmpl.param[0];

        switch (pMsg->vendorSpecCmdCmpl.opcode)
        {
        case 0xFC20: //read at address
        {
            uint32_t read_value;

            BSTREAM_TO_UINT32(read_value, param_ptr);

            APP_TRACE_INFO3("VSC 0x%0x complete status %x param %x",
                            pMsg->vendorSpecCmdCmpl.opcode,
                            pMsg->hdr.status,
                            read_value);
        }

        break;
        default:
            APP_TRACE_INFO2("VSC 0x%0x complete status %x",
                            pMsg->vendorSpecCmdCmpl.opcode,
                            pMsg->hdr.status);
            break;
        }

#endif
    }
    break;

    default:
        break;
    }

    if (uiEvent != APP_UI_NONE)
    {
        AppUiAction(uiEvent);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WatchHandlerInit(wsfHandlerId_t handlerId)
{
    APP_TRACE_INFO0("WatchHandlerInit");

    /* store handler ID */
    watchCb.handlerId = handlerId;

    /* Set configuration pointers */
    pAppMasterCfg = (appMasterCfg_t *)&watchMasterCfg;
    pAppSlaveCfg = (appSlaveCfg_t *)&watchSlaveCfg;
    pAppAdvCfg = (appAdvCfg_t *)&watchAdvCfg;
    pAppSecCfg = (appSecCfg_t *)&watchSecCfg;
    pAppUpdateCfg = (appUpdateCfg_t *)&watchUpdateCfg;
    pAppDiscCfg = (appDiscCfg_t *)&watchDiscCfg;
    pAppCfg = (appCfg_t *)&watchAppCfg;

    /* Set stack configuration pointers */
    pSmpCfg = (smpCfg_t *)&watchSmpCfg;

    /* Initialize application framework */
    AppMasterInit();
    AppSlaveInit();
    AppDiscInit();
}

/*************************************************************************************************/
/*!
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WatchHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
    if (pMsg != NULL)
    {
        APP_TRACE_INFO1("Watch got evt %d", pMsg->event);

        /* process ATT messages */
        if (pMsg->event <= ATT_CBACK_END)
        {
            /* process discovery-related ATT messages */
            AppDiscProcAttMsg((attEvt_t *)pMsg);

            /* process server-related ATT messages */
            AppServerProcAttMsg(pMsg);
        }
        /* process DM messages */
        else if (pMsg->event <= DM_CBACK_END)
        {
            if (pMsg->param == DM_CONN_ID_NONE || DmConnRole((dmConnId_t)pMsg->param) == DM_ROLE_MASTER)
            {
                /* process advertising and connection-related messages */
                AppMasterProcDmMsg((dmEvt_t *)pMsg);

                /* process security-related messages */
                AppMasterSecProcDmMsg((dmEvt_t *)pMsg);
            }

            if (pMsg->param == DM_CONN_ID_NONE || DmConnRole((dmConnId_t)pMsg->param) == DM_ROLE_SLAVE)
            {
                /* process advertising and connection-related messages */
                AppSlaveProcDmMsg((dmEvt_t *)pMsg);

                /* process security-related messages */
                AppSlaveSecProcDmMsg((dmEvt_t *)pMsg);
            }

            /* process discovery-related messages */
            AppDiscProcDmMsg((dmEvt_t *)pMsg);
        }

        /* perform profile and user interface-related operations */
        watchProcMsg((dmEvt_t *)pMsg);
    }
}

/*************************************************************************************************/
/*!
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void WatchStart(void)
{
    /* Register for stack callbacks */
    DmRegister(watchDmCback);
    DmConnRegister(DM_CLIENT_ID_APP, watchDmCback);
    AttRegister(watchAttCback);
    AttConnRegister(AppServerConnCback);
    AttsCccRegister(WATCH_NUM_CCC_IDX, (attsCccSet_t *)watchCccSet, watchCccCback);

    /* Register for app framework button callbacks */
    AppUiBtnRegister(watchBtnCback);

    /* Register for app framework discovery callbacks */
    AppDiscRegister(watchDiscCback);

    /* Initialize attribute server database */
    SvcCoreAddGroup();

    // AttsAddGroup(NULL);
    // ble_services_add(NULL);
    ble_qiot_explorer_init();

    /* Reset the device */
    DmDevReset();
}
