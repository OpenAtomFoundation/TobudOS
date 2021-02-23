/*!
    \file    msc_bbb_scsi.h
    \brief   the header file of the msc_bbb_scsi.c

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef __MSC_BBB_SCSI_H
#define __MSC_BBB_SCSI_H

#include "usbd_enum.h"

/* signature of command block wrapper (CBW) */
#define BBB_CBW_SIGNATURE              0x43425355U
#define BBB_CBW_LENGTH                 31U

/* signature of command status wrapper (CSW) */
#define BBB_CSW_SIGNATURE              0x53425355U
#define BBB_CSW_LENGTH                 13U

#define SCSI_CMD_LENGTH                16U

/* transport stage define */
enum msc_stage {
    MSC_STAGE_CMD = 0U,
    MSC_STAGE_DATA_IN,
    MSC_STAGE_DATA_OUT,
    MSC_STAGE_STATUS
};

/* SCSI command define */
#define INQUIRY                        0x12U
#define READ_FORMAT_CAPACITIES         0x23U
#define READ_CAPACITY                  0x25U
#define READ_10                        0x28U
#define WRITE_10                       0x2AU
#define REQUEST_SENSE                  0x03U
#define TEST_UNIT_READY                0x00U
#define SCSI_MODE_SENSE6               0x1AU
#define SCSI_READ_TOC_DATA             0x43U
#define SCSI_ALLOW_MEDIUM_REMOVAL      0x1EU
#define SCSI_START_STOP_UNIT           0x1BU

/* the array length define */
#define DATA_LEN_DISK_INQUIRY          36U
#define DATA_LEN_FORMAT_CAPACITIES     12U
#define DATA_LEN_READ_CAPACITIES       8U
#define DATA_LEN_REQUEST_SENSE         18U
#define DATA_LEN_SENSE6                8U

typedef struct {
    uint32_t dCBWSignature;             /*!< signature of a CBW */
    uint32_t dCBWTag;                   /*!< a command block tag sent by the host */
    uint32_t dCBWDataTransferLength;    /*!< the number of bytes of data stage */
    uint8_t  bmCBWFlags;                /*!< maybe ignored or include direction */
    uint8_t  bCBWLUN;                   /*!< the device logical unit number (LUN) */
    uint8_t  bCBWCBLength;              /*!< the valid length of the CBWCB in bytes */
    uint8_t  CBWCB[16];                 /*!< the command block to be executed by the device */
} msc_bbb_cbw;

typedef struct {
    uint32_t dCSWSignature;             /*!< signature of a CSW */
    uint32_t dCSWTag;                   /*!< must be same as CBW */
    uint32_t dCSWDataResidue;           /*!< the difference between the amount of data and the actual amount */
    uint8_t  bCSWStatus;                /*!< indicates the success or failure of the command */
} msc_bbb_csw;

typedef struct{
    uint8_t usbd_msc_maxlun;

    /* the array to store the SCSI command */
    uint8_t scsi_cmd[SCSI_CMD_LENGTH];

    /* the global data to store the transport stage in BBB protocol */
    uint8_t bbb_transport_stage;

    /* the data that need to return the host when the command is not valid */
    uint8_t error_data;

    /* the array to store the data from or to host in read10 or write10 command */
    uint8_t bbb_data[MSC_MEDIA_PACKET_SIZE];

    msc_bbb_cbw bbb_cbw;
    msc_bbb_csw bbb_csw;

    uint32_t data_len;

    uint32_t block_addr;
    uint32_t block_len;
    uint32_t disk_pop;

    /* USB mass storage read capacities data */
    uint8_t read_capa_data[DATA_LEN_READ_CAPACITIES];
} usbd_msc_handler;

/* function declarations */
/* parse the SCSI commands from the CBW */
void scsi_command_parse (usb_dev *udev);
/* report array to host */
void bbb_report_array_to_host (usb_dev *udev, uint8_t *data, uint32_t report_len);
/* process the SCSI command */
void process_scsi_command (uint8_t lun, usb_dev *udev);

#endif /* __MSC_BBB_SCSI_H */
