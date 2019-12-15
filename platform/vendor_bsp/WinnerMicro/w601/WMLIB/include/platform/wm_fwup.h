/**
 * @file    wm_fwup.h
 *
 * @brief   Firmware upgrade
 *
 * @author  winnermicro
 *
 * Copyright (c) 2015 Winner Microelectronics Co., Ltd.
 */

#ifndef WM_FWUP_H
#define WM_FWUP_H
#include "wm_osal.h"
#include "list.h"

/** firmware update status */
#define TLS_FWUP_STATUS_OK      (0)
#define TLS_FWUP_STATUS_EINVALID      (1)
#define TLS_FWUP_STATUS_EMEM      (2)
#define TLS_FWUP_STATUS_EPERM      (3)
#define TLS_FWUP_STATUS_EBUSY      (4)
#define TLS_FWUP_STATUS_ESESSIONID      (5)
#define TLS_FWUP_STATUS_EIO      (6)
#define TLS_FWUP_STATUS_ESIGNATURE      (7)
#define TLS_FWUP_STATUS_ECRC      (8)
#define TLS_FWUP_STATUS_EUNDEF      (9)

/** firmware signature */
#define TLS_FWUP_IMAGE_SIGNATURE_WORD      0x64365577
#define TLS_FWUP_IMAGE_SIGNATURE_WORD_B      0x62161900

/** firmware block size for one time */
#define TLS_FWUP_BLK_SIZE      512

/** firmware update request status */
#define TLS_FWUP_REQ_STATUS_IDLE      (0)
#define TLS_FWUP_REQ_STATUS_BUSY      (1)
#define TLS_FWUP_REQ_STATUS_SUCCESS      (2)
#define TLS_FWUP_REQ_STATUS_FIO      (3)
#define TLS_FWUP_REQ_STATUS_FSIGNATURE      (4)
#define TLS_FWUP_REQ_STATUS_FMEM      (5)
#define TLS_FWUP_REQ_STATUS_FCRC      (6)
#define TLS_FWUP_REQ_STATUS_FCOMPLETE      (7)

/** firmware update state */
#define TLS_FWUP_STATE_UNDEF      (0xffff)
#define TLS_FWUP_STATE_BUSY      (1 << 0)
#define TLS_FWUP_STATE_COMPLETE      (1 << 1)
#define TLS_FWUP_STATE_ERROR_IO      (1 << 2)
#define TLS_FWUP_STATE_ERROR_SIGNATURE      (1 << 3)
#define TLS_FWUP_STATE_ERROR_MEM      (1 << 4)
#define TLS_FWUP_STATE_ERROR_CRC      (1 << 5)

#define TLS_FWUP_STATE_ERROR      (TLS_FWUP_STATE_ERROR_IO | TLS_FWUP_STATE_ERROR_SIGNATURE | TLS_FWUP_STATE_ERROR_MEM | TLS_FWUP_STATE_ERROR_CRC)

/**   update type 0:firmware, 1: data   */
#define TLS_FWUP_DEST_SPECIFIC_FIRMWARE      (0)
#define TLS_FWUP_DEST_SPECIFIC_DATA      (1)

enum IMAGE_TYPE_ENUM{
	IMG_TYPE_OLD_PLAIN = 0,
	IMG_TYPE_FLASHBIN = 1,
	IMG_TYPE_SECBOOT = 2,
	IMG_TYPE_NEW_PLAIN = 3
};
enum {
	NOT_ZIP_FILE = 0,
	ZIP_FILE = 1
};
typedef struct __T_BOOTER
{
	unsigned int   	magic_no;
	unsigned short 	img_type;			
	unsigned short 	zip_type;				/** image type zip flag, 0: non-zip, 1:zip*/
	unsigned int   	run_img_addr;         	/** run area image start address */
	unsigned int   	run_img_len;			/** run area image length */
	unsigned int	run_org_checksum; 		/** run area image checksum */
	unsigned int    upd_img_addr;			/** upgrade area image start address*/
	unsigned int    upd_img_len;			/** upgrade area image length*/
	unsigned int 	upd_checksum;			/** upgrade area image checksum */
	unsigned int   	upd_no;
	unsigned char  	ver[16];
	unsigned int 	hd_checksum;
} T_BOOTER;

/**   Structure for firmware image header   */
struct tls_fwup_image_hdr {
	u32 magic;
	u8 crc8;
	u8 dest_specific;
	u16 dest_offset;  // unit: 4KB, valid when dest_specific is TRUE
	u32 file_len;
	char time[4];
};

/**   Structure for one packet data   */
struct  tls_fwup_block {
	u16 number;	//0~Sum-1
	u16 sum;
	u8 data[TLS_FWUP_BLK_SIZE];
	u32 crc32;
	u8 pad[8];
};

/**   Enumeration for image soure when firmware update   */
enum tls_fwup_image_src {
	TLS_FWUP_IMAGE_SRC_LUART = 0,    /**< LOW SPEED UART */
	TLS_FWUP_IMAGE_SRC_HUART,		 /**< HIGH SPEED UART */
	TLS_FWUP_IMAGE_SRC_HSPI,		 /**< HIGH SPEED SPI */
	TLS_FWUP_IMAGE_SRC_WEB           /**< WEB SERVER */
};

/**   Structure for firmware update request   */
struct tls_fwup_request {
	struct dl_list list;
	u8 *data;
	u32 data_len;
	int status;
	void (*complete)(struct tls_fwup_request *request, void *arg);
	void *arg;
};

/**   Structure for firmware update   */
struct tls_fwup {
	struct dl_list wait_list;
	tls_os_sem_t *list_lock;

	bool busy;
	enum tls_fwup_image_src current_image_src;
	u16 current_state;
	u32 current_session_id;

	u32 received_len;
	u32 total_len;
	u32 updated_len;
	u32 program_base;
	u32 program_offset;
	s32 received_number;
};

/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup FWUP_APIs FWUP APIs
 * @brief firmware upgrade APIs
 */

/**
 * @addtogroup FWUP_APIs
 * @{
 */

/**
 * @brief          This function is used to initialize firmware update task
 *
 * @param[in]      None
 *
 * @retval         TLS_FWUP_STATUS_OK     initial success
 * @retval         TLS_FWUP_STATUS_EBUSY  already initialed
 * @retval         TLS_FWUP_STATUS_EMEM	  memory error
 * @note           None
 */
int tls_fwup_init(void);

/**
 * @brief          This function is used to enter firmware update progress.
 *
 * @param[in]      image_src   image file's source,
 							   from TLS_FWUP_IMAGE_SRC_LUART,
 							   TLS_FWUP_IMAGE_SRC_WEB,TLS_FWUP_IMAGE_SRC_HUART,
 							   TLS_FWUP_IMAGE_SRC_HSPI
 *
 * @retval         non-zero    successfully, return session id
 * @retval         0           failed
 *
 * @note           None
 */
u32 tls_fwup_enter(enum tls_fwup_image_src image_src);

/**
 * @brief          This function is used to exit firmware update progress.
 *
 * @param[in]      session_id    session identity of firmware update progress
 *
 * @retval         TLS_FWUP_STATUS_OK				exit success
 * @retval         TLS_FWUP_STATUS_EPERM			globle param is not initialed
 * @retval         TLS_FWUP_STATUS_ESESSIONID		error session id
 * @retval         TLS_FWUP_STATUS_EBUSY			update state is busy
 *
 * @note           None
 */
int tls_fwup_exit(u32 session_id);

/**
 * @brief          This function is used to start update progress
 *
 * @param[in]      session_id    current sessin id
 * @param[in]      *data         the data want to update
 * @param[in]      data_len      data length
 *
 * @retval 		   TLS_FWUP_STATUS_OK			updade success
 * @retval		   TLS_FWUP_STATUS_EPERM		globle param is not initialed
 * @retval		   TLS_FWUP_STATUS_ESESSIONID	error session id
 * @retval		   TLS_FWUP_STATUS_EINVALID		invalid param
 * @retval		   TLS_FWUP_STATUS_EMEM			memory error
 * @retval		   TLS_FWUP_STATUS_EIO			write flash error
 * @retval		   TLS_FWUP_STATUS_ECRC			crc error
 * @retval		   TLS_FWUP_STATUS_ESIGNATURE	signature error
 * @retval		   TLS_FWUP_STATUS_EUNDEF		other error
 *
 * @note           None
 */
int tls_fwup_request_sync(u32 session_id, u8 *data, u32 data_len);

/**
 * @brief          This function is used to get current update status
 *
 * @param[in]      session_id     current sessin id
 *
 * @retval         current state  TLS_FWUP_STATUS_OK to TLS_FWUP_STATUS_EUNDEF
 *
 * @note           None
 */
u16 tls_fwup_current_state(u32 session_id);

/**
 * @brief          This function is used to reset the update information
 *
 * @param[in]      session_id    current sessin id
 *
 * @retval         TLS_FWUP_STATUS_OK         reset success
 * @retval         TLS_FWUP_STATUS_EPERM      globle param is not initialed
 * @retval         TLS_FWUP_STATUS_ESESSIONID error session id
 * @retval         TLS_FWUP_STATUS_EBUSY      update state is busy
 *
 * @note           None
 */
int tls_fwup_reset(u32 session_id);

/**
 * @brief          This function is used to clear error update state
 *
 * @param[in]      session_id    current sessin id
 *
 * @retval         TLS_FWUP_STATUS_OK   reset success
 *
 * @note           None
 */
int tls_fwup_clear_error(u32 session_id);

/**
 * @brief          This function is used to set update state to
 				   TLS_FWUP_STATE_ERROR_CRC
 *
 * @param[in]      session_id    current sessin id
 *
 * @retval         TLS_FWUP_STATUS_OK	      set success
 * @retval         TLS_FWUP_STATUS_EPERM      globle param is not initialed
 * @retval         TLS_FWUP_STATUS_ESESSIONID error session id
 *
 * @note           None
 */
int tls_fwup_set_crc_error(u32 session_id);

/**
 * @brief          This function is used to get progress's status
 *
 * @param[in]      None
 *
 * @retval         TRUE  busy
 * @retval         FALSE idle
 *
 * @note           None
 */
int tls_fwup_get_status(void);

/**
 * @brief          This function is used to set update packet number
 *
 * @param[in]      number
 *
 * @retval         TLS_FWUP_STATUS_OK     success
 * @retval         TLS_FWUP_STATE_UNDEF   failed
 *
 * @note           None
 */
int tls_fwup_set_update_numer(int number);

/**
 * @brief          This function is used to get received update packet number

 *
 * @param[in]      None
 *
 * @retval         return current packet number
 *
 * @note           None
 */
int tls_fwup_get_current_update_numer(void);


/**
 * @brief          This function is used to get current session id
 *
 * @param[in]      None
 *
 * @retval         non-zoro session id
 * @retval         0        error
 *
 * @note           None
 */
int tls_fwup_get_current_session_id(void);

/**
 * @brief          This function is used to check image header
 *
 * @param[in]      None
 *
 * @retval         TRUE:   success
 * @retval         FALSE:  failure
 *
 * @note           None
 */
int tls_fwup_img_header_check(T_BOOTER *img_param);

/**
 * @}
 */

/**
 * @}
 */

#endif /* WM_FWUP_H */

