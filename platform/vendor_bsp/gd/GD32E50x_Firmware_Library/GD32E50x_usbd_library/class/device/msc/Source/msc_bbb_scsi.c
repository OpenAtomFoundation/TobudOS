/*!
    \file    bbb_scsi_driver.c
    \brief   USB BBB and SCSI protocol driver functions

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

#include "msc_bbb_scsi.h"
#include "msc_mem.h"

/* USB mass storage format capacities data */
uint8_t format_capa_data[DATA_LEN_FORMAT_CAPACITIES] =
{
    0x00, 
    0x00, 
    0x00,                       /* reserved */
    0x08,                       /* capacity list length */
    0x00,
    0x00,
    0x00,
    0x00,
    0x02,                       /* bit0-bit1:descriptor code */
    0x00,
    0x00,
    0x00
};

/* USB mass storage request sense data */
uint8_t request_sense_data[DATA_LEN_REQUEST_SENSE] = 
{
    0x70,                                   /* bit0-bit6:error code (70h), bit7:valid */
    0x00,                                   /* reserved */
    0x05,                                   /* bit0-bit3:sense key */
    0x00, 0x00, 0x00, 0x00,                 /* information (MSB) */
    0x0A,                                   /* additional sense length (10) */
    0x00, 0x00, 0x00, 0x00,                 /* reserved */
    0x20,                                   /* additional sense code (mandatory) */
    0x00,                                   /* additional sense code qualifier (mandatory) */
    0x00, 0x00, 0x00, 0x00                  /* reserved */
};

/* USB mass storage sense 6  data */
uint8_t sense6_data[DATA_LEN_SENSE6] = 
{
    0x03,                                   /* mode data length */
    0x00,                                   /* medium type */
    0x00,                                   /* device specific parameter, bit7: 0(write and read), 1(read only) */
    0x00,                                   /* block descriptor length */
    0x00,                                   /* reserved */
    0x00,                                   /* reserved */
    0x00,                                   /* reserved */
    0x00                                    /* reserved */
};

/* local function prototypes ('static') */
static void bbb_set_csw (usb_dev *udev, uint32_t csw_tag, uint32_t csw_data_residue, uint8_t csw_status);

/*!
    \brief      parse the SCSI commands from the CBW
    \param[in]  udev: pointer to USB device instance
    \param[out] none
    \retval     none
*/
void scsi_command_parse (usb_dev *udev)
{
    uint8_t i;

    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    for(i = 0U; i < SCSI_CMD_LENGTH; i++){
        msc->scsi_cmd[i] = msc->bbb_cbw.CBWCB[i];
    }
}

/*!
    \brief      set the CSW structure
    \param[in]  udev: pointer to device instance
    \param[in]  csw_tag: signature that helps identify this data packet as a csw
    \param[in]  csw_data_residue: the device shall set this field to the value received in the cbwtag of the associated bbb_cbw
    \param[in]  csw_status: for data-out: the difference between the receive data and dCBWDataTransferLength
                            for data-in: the difference between the transmit data and dCBWDataTransferLength 
    \param[out] none
    \retval     none
*/
static void bbb_set_csw (usb_dev *udev, uint32_t csw_tag, uint32_t csw_data_residue, uint8_t csw_status)
{
    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    msc->bbb_csw = (msc_bbb_csw) {
        .dCSWSignature   = BBB_CSW_SIGNATURE,
        .dCSWTag         = csw_tag,
        .dCSWDataResidue = csw_data_residue,
        .bCSWStatus      = csw_status
    };
}

/*!
    \brief      report array to host
    \param[in]  udev: pointer to device instance
    \param[in]  data: pointer to the send array
    \param[in]  report_len: the report length
    \param[out] none
    \retval     none
*/
void bbb_report_array_to_host (usb_dev *udev, uint8_t *data, uint32_t report_len)
{
    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    switch (msc->bbb_transport_stage) {
    case MSC_STAGE_CMD:
        msc->bbb_transport_stage = MSC_STAGE_DATA_IN;

        msc->data_len = msc->bbb_cbw.dCBWDataTransferLength;

        if(msc->data_len > report_len){
            msc->data_len = report_len;
        }
        usbd_ep_send (udev, MSC_IN_EP, data, (uint16_t)msc->data_len);
        break;

    case MSC_STAGE_DATA_IN:
        bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, msc->bbb_cbw.dCBWDataTransferLength - msc->data_len, 0U);

        msc->data_len = BBB_CSW_LENGTH;

        msc->bbb_transport_stage = MSC_STAGE_STATUS;

        usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, (uint16_t)msc->data_len);
        break;

    case MSC_STAGE_STATUS:
        msc->bbb_transport_stage = MSC_STAGE_CMD;

        usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
        break;

    default:
        break;
    }
}

/*!
    \brief      process the SCSI command
    \param[in]  lun: logical block number
    \param[in]  udev: pointer to device instance
    \param[out] none
    \retval     none
*/
void process_scsi_command (uint8_t lun, usb_dev *udev)
{
    uint32_t l_csw_data_residue = 0U;

    usbd_msc_handler *msc = (usbd_msc_handler *)udev->class_data[USBD_MSC_INTERFACE];

    switch(msc->scsi_cmd[0]){
    /* process the INQUIRY command */
    case INQUIRY:
        bbb_report_array_to_host(udev, usbd_mem_fops->mem_inquiry_data[lun], DATA_LEN_DISK_INQUIRY);
        break;

    /* process the TEST_UNIT_READY command */
    case TEST_UNIT_READY:
        if(1U == msc->disk_pop){
             gpio_bit_reset(USB_PULLUP, USB_PULLUP_PIN);
        }

        if (MSC_STAGE_CMD == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_STATUS;

            bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, 0U, 0U);

            usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);
        } else {
            msc->bbb_transport_stage = MSC_STAGE_CMD;

            usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
        }
        break;

    /* process the SCSI_START_STOP_UNIT command */
    case SCSI_START_STOP_UNIT:
        msc->disk_pop = 1U;

        if (MSC_STAGE_CMD == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_STATUS;

            bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, 0U, 0U);

            usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);
        } else {
            msc->bbb_transport_stage = MSC_STAGE_CMD;

            usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
        }
        break;

    /* process the SCSI_ALLOW_MEDIUM_REMOVAL command */
    case SCSI_ALLOW_MEDIUM_REMOVAL:
        if (MSC_STAGE_CMD == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_STATUS;

            bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, 0U, 0U);

            usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);
        } else {
            msc->bbb_transport_stage = MSC_STAGE_CMD;

            usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
        }
        break;

    /* process the READ_FORMAT_CAPACITIES command */
    case READ_FORMAT_CAPACITIES:
        format_capa_data[4] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 24);
        format_capa_data[5] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 16);
        format_capa_data[6] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 8);
        format_capa_data[7] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 0);

        format_capa_data[9] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 16);
        format_capa_data[10] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 8);
        format_capa_data[11] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 0);

        bbb_report_array_to_host(udev, format_capa_data, DATA_LEN_FORMAT_CAPACITIES);
        break;

    /* process the READ_CAPACITY command */
    case READ_CAPACITY:
        msc->read_capa_data[0] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 24);
        msc->read_capa_data[1] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 16);
        msc->read_capa_data[2] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 8);
        msc->read_capa_data[3] = (uint8_t)((usbd_mem_fops->mem_block_len[lun] - 1U) >> 0);

        msc->read_capa_data[4] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 24);
        msc->read_capa_data[5] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 16);
        msc->read_capa_data[6] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 8);
        msc->read_capa_data[7] = (uint8_t)((usbd_mem_fops->mem_block_size[lun]) >> 0);

        bbb_report_array_to_host(udev, msc->read_capa_data, DATA_LEN_READ_CAPACITIES);
        break;

    /* process the READ_10 command */
    case READ_10:
        /* read the first block */
        if (MSC_STAGE_CMD == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_DATA_IN;

            msc->block_addr = ((uint32_t)msc->scsi_cmd[2] << 24) | \
                               ((uint32_t)msc->scsi_cmd[3] << 16) | \
                               ((uint32_t)msc->scsi_cmd[4] << 8) | \
                               (uint32_t)msc->scsi_cmd[5];
            msc->block_len = ((uint32_t)msc->scsi_cmd[7] << 8) | (uint32_t)msc->scsi_cmd[8];
            msc->block_addr *= usbd_mem_fops->mem_block_size[lun];
            msc->block_len *= usbd_mem_fops->mem_block_size[lun];

            usbd_mem_fops->mem_read(lun, msc->bbb_data, msc->block_addr, 1U);

            usbd_ep_send (udev, MSC_IN_EP, msc->bbb_data, (uint16_t)usbd_mem_fops->mem_block_size[lun]);

            msc->block_addr += usbd_mem_fops->mem_block_size[lun];
            msc->block_len -= usbd_mem_fops->mem_block_size[lun];
        } else if (MSC_STAGE_DATA_IN == msc->bbb_transport_stage){
            /* end of the read, send bbb_csw to host */
            if (0U == msc->block_len) {
                msc->bbb_transport_stage = MSC_STAGE_STATUS;

                l_csw_data_residue = msc->bbb_cbw.dCBWDataTransferLength - \
                                    (((uint32_t)msc->scsi_cmd[7] << 8) | (uint32_t)msc->scsi_cmd[8]) * usbd_mem_fops->mem_block_size[lun];

                bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, l_csw_data_residue, 0U);

                usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);
            /* read the nest block */
            } else {
                usbd_mem_fops->mem_read(lun, msc->bbb_data, msc->block_addr, 1U);

                usbd_ep_send (udev, MSC_IN_EP, msc->bbb_data, (uint16_t)usbd_mem_fops->mem_block_size[lun]);

                msc->block_addr += usbd_mem_fops->mem_block_size[lun];
                msc->block_len -= usbd_mem_fops->mem_block_size[lun];
            }
        /* enter the MSC_STAGE_CMD */
        } else if (MSC_STAGE_STATUS == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_CMD;

            usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
        } else {
            /* no operation */
        }
        break;

    /* process the WRITE_10 command */
    case WRITE_10:
        /* write the first block */
        if (MSC_STAGE_CMD == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_DATA_OUT;

            msc->block_addr = ((uint32_t)msc->scsi_cmd[2] << 24) | \
                               ((uint32_t)msc->scsi_cmd[3] << 16) | \
                               ((uint32_t)msc->scsi_cmd[4] << 8) | \
                               (uint32_t)msc->scsi_cmd[5];
            msc->block_len = ((uint32_t)msc->scsi_cmd[7] << 8) | (uint32_t)msc->scsi_cmd[8];
            msc->block_addr *= usbd_mem_fops->mem_block_size[lun];
            msc->block_len *= usbd_mem_fops->mem_block_size[lun];

            usbd_ep_recev (udev, MSC_OUT_EP, msc->bbb_data, (uint16_t)usbd_mem_fops->mem_block_size[lun]);
        } else if (MSC_STAGE_DATA_OUT == msc->bbb_transport_stage) {
            usbd_mem_fops->mem_write(lun, msc->bbb_data, msc->block_addr, 1U);

            msc->block_addr += usbd_mem_fops->mem_block_size[lun];
            msc->block_len -= usbd_mem_fops->mem_block_size[lun];

            /* end of the write, send bbb_csw to host */
            if (0U == msc->block_len) {
                msc->bbb_transport_stage = MSC_STAGE_STATUS;

                l_csw_data_residue = msc->bbb_cbw.dCBWDataTransferLength - \
                          (((uint32_t)msc->scsi_cmd[7] << 8) | (uint32_t)msc->scsi_cmd[8]) * usbd_mem_fops->mem_block_size[lun];

                bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, l_csw_data_residue, 0U);

                usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);
            /* receive the next block data */
            } else {
                usbd_ep_recev (udev, MSC_OUT_EP, msc->bbb_data, (uint16_t)usbd_mem_fops->mem_block_size[lun]);
            }
        /* enter the MSC_STAGE_CMD */
        } else if (MSC_STAGE_STATUS == msc->bbb_transport_stage) {
            msc->bbb_transport_stage = MSC_STAGE_CMD;

            usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
        } else {
            /* no operation */
        }
        break;

    /* process the REQUEST_SENSE command */
    case REQUEST_SENSE:
        bbb_report_array_to_host(udev, request_sense_data, DATA_LEN_REQUEST_SENSE);
        break;

    /* process the SCSI_MODE_SENSE6 command */
    case SCSI_MODE_SENSE6:
        bbb_report_array_to_host(udev, sense6_data, DATA_LEN_SENSE6);
        break;

    /* process the SCSI_READ_TOC_DATA command */
    case SCSI_READ_TOC_DATA:
        bbb_report_array_to_host(udev, usbd_mem_fops->mem_toc_data, 20U);
        break;

    /* default process: the SCSI command is not valid */
    default:
        /* if the data stage is DATA_IN, return the error_data to host in MSC_STAGE_DATA_IN */
        if (0x80U & msc->bbb_cbw.bmCBWFlags) {
            if (MSC_STAGE_CMD == msc->bbb_transport_stage){
                msc->bbb_transport_stage = MSC_STAGE_DATA_IN;

                msc->data_len = 1U;

                usbd_ep_send (udev, MSC_IN_EP, &msc->error_data, 1U);
            } else if (MSC_STAGE_DATA_IN == msc->bbb_transport_stage){
                msc->bbb_transport_stage = MSC_STAGE_STATUS;

                bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, msc->bbb_cbw.dCBWDataTransferLength - 1U, 1U);

                usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);
            } else if (MSC_STAGE_STATUS == msc->bbb_transport_stage){
                msc->bbb_transport_stage = MSC_STAGE_CMD;

                usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
            } else {
                /* no operation */
            }

        /* if the data stage is DATA_OUT, return the bbb_csw to host immediately */
        } else{
            if (MSC_STAGE_CMD == msc->bbb_transport_stage) {
                msc->data_len = 0U;

                bbb_set_csw(udev, msc->bbb_cbw.dCBWTag, msc->bbb_cbw.dCBWDataTransferLength, 1U);

                usbd_ep_send (udev, MSC_IN_EP, (uint8_t *)&msc->bbb_csw, BBB_CSW_LENGTH);

                msc->bbb_transport_stage = MSC_STAGE_STATUS;
            } else if (MSC_STAGE_STATUS == msc->bbb_transport_stage) {
                msc->bbb_transport_stage = MSC_STAGE_CMD;

                usbd_ep_recev (udev, MSC_OUT_EP, (uint8_t *)&msc->bbb_cbw, BBB_CBW_LENGTH);
            } else {
                /* no operation */
            }
        }
        break;
    }
}
