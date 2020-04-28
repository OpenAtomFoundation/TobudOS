#ifndef __LL_SDIO_H__
#define __LL_SDIO_H__

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum {
    SDIO_FUNC0 = 0,
    SDIO_FUNC1,
    SDIO_FUNC2,
    SDIO_FUNC3,
    SDIO_FUNC4,
    SDIO_FUNC5,
    SDIO_FUNC6,
    SDIO_FUNC7
} sdio_func_enum_t;

enum {
    Fn1_Write_Over_Interrupt_Disabled = 0,
    Fn1_Write_Over_Interrupt_Enabled
};
enum {
    Fn1_Read_Over_Interrupt_Disabled = 0,
    Fn1_Read_Over_Interrupt_Enabled
};
enum {
    Read_Error_Fn1_Interrupt_Disabled = 0,
    Read_Error_Fn1_Interrupt_Enabled
};
enum {
    Write_Error_Fn1_Interrupt_Disabled = 0,
    Write_Error_Fn1_Interrupt_Enabled
};
enum {
    Write_Abort_Fn1_Interrupt_Disabled = 0,
    Write_Abort_Fn1_Interrupt_Enabled
};
enum {
    Reset_Fn1_Interrupt_Disabled = 0,
    Reset_Fn1_Interrupt_Enabled
};
enum {
    Fn1_Enable_Interrupt_Disabled = 0,
    Fn1_Enable_Interrupt_Enabled
};
enum {
    Fn1_Status_Pcrrt_Interrupt_Disabled = 0,
    Fn1_Status_Pcrrt_Interrupt_Enabled
};
enum {
    Fn1_Status_Pcwrt_Interrupt_Disabled = 0,
    Fn1_Status_Pcwrt_Interrupt_Enabled
};
enum {
    Fn1_Rtc_Set_Interrupt_Disabled = 0,
    Fn1_Rtc_Set_Interrupt_Enabled
};
enum {
    Fn1_Clintrd_Interrupt_Disabled = 0,
    Fn1_Clintrd_Interrupt_Enabled
};
enum {
    Fn1_Int_En_Up_Interrupt_Disabled = 0,
    Fn1_Int_En_Up_Interrupt_Enabled
};
enum {
    Fn1_M2s_Int_Interrupt_Disabled = 0,
    Fn1_M2s_Int_Interrupt_Enabled
};
enum {
    SDIO_IO_Disabled = 0,
    SDIO_IO_Enabled
};
enum {
    Clr_Busy_SD_Disabled = 0,
    Clr_Busy_SD_Enabled
};
enum {
    Supp_High_Speed_Disabled = 0,
    Supp_High_Speed_Enabled
};
enum {
    Power_Selection_Disabled = 0,
    Power_Selection_Enabled
};

#define CLEAR_BUSY_SD               (0)
#define SET_BUSY_SD                 (1)
#define FN1_CSA_SUPPORT             (1 << 0)
#define FN2_CSA_SUPPORT             (1 << 1)
#define FN3_CSA_SUPPORT             (1 << 2)
#define FN4_CSA_SUPPORT             (1 << 3)
#define FN5_CSA_SUPPORT             (1 << 4)
#define FN6_CSA_SUPPORT             (1 << 5)
#define FN7_CSA_SUPPORT             (1 << 6)

#define  SDIO_CCCR_CAP_SCSI  (1 << 0)
#define  SDIO_CCCR_CAP_SDC  (1 << 1)    /* can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB  (1 << 2)    /* can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW  (1 << 3)    /* supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS  (1 << 4)    /* supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI (1 << 5)    /* interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI (1 << 6)    /* enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC  (1 << 7)    /* low speed card */
#define  SDIO_CCCR_CAP_4BLS (1 << 8)    /* 4 bit low speed card */

#define  REVISION_REG_SD    (0x232)

#define SDIO_IO_READY                   (1)
#define SDIO_DEVICE_READY               (1 << 0)
#define SDIO_CARD_READY                 (1 << 1)
#define SDIO_CPU_IN_ACTIVE              (1 << 2)
#define SDIO_FBR1_REG                   (0xFF)
#define SDIO_INTERFACE_CODE             (0xF)
#define SDIO_HOST_IIR_REG_CLEAR         (0)
#define SDIO_HOST_IIR_REG_SET           (1)
#define SDIO_SUPPORT_FUNC_NUM           (1 << 4)    //support 1 func

#define FN1_WRITE_OVER_INTERRPT         (1 << 0)
#define FN1_READ_OVER_INTERRPT          (1 << 1)
#define READ_ERROR_FN1_INTERRPT         (1 << 2)
#define WRITE_ERROR_FN1_INTERRPT        (1 << 3)
#define WRITE_ABORT_FN1_INTERRPT        (1 << 4)
#define RESET_FN1_INTERRPT              (1 << 5)
#define FN1_ENABLE_INTERRPT             (1 << 6)
#define FN1_STATUS_PCRRT_INTERRPT       (1 << 7)
#define FN1_STATUS_PCWRT_INTERRPT       (1 << 8)
#define FN1_RTC_SET_INTERRPT            (1 << 9)
#define FN1_CLINTRD_INTERRPT            (1 << 10)
#define FN1_INT_EN_UP_INTERRPT          (1 << 11)
#define FN1_M2S_INT_INTERRPT            (1 << 20)

#define SDIO_FUN_CIS_LENGTH_OFFSET          4
#define SDIO_FUNC_CIS_CHECKSUM_OFFSET       6
#define SDIO_FUNC_BLOCK_SIZE                512
#define SET_CISTPL_CHECKSUM                 0   //If Host check CIS tuple checksum, set this to 1.

typedef union {
    uint32_t val;
    struct {
        uint32_t                     fn1_wr_ovr :  1; /* 0: 0,          interrupt enable bit */
        uint32_t                     fn1_rd_ovr :  1; /* 1: 1,          interrupt enable bit */
        uint32_t                     rd_err_fn1 :  1; /* 2: 2,          interrupt enable bit */
        uint32_t                     wr_err_fn1 :  1; /* 3: 3,          interrupt enable bit */
        uint32_t                   wr_abort_fn1 :  1; /* 4: 4,          interrupt enable bit */
        uint32_t                        rst_fn1 :  1; /* 5: 5,          interrupt enable bit */
        uint32_t                         fn1_en :  1; /* 6: 6,          interrupt enable bit */
        uint32_t               fn1_status_pcrrt :  1; /* 7: 7,          interrupt enable bit */
        uint32_t               fn1_status_pcwrt :  1; /* 8: 8,          interrupt enable bit */
        uint32_t                    fn1_rtc_set :  1; /* 9: 9,          interrupt enable bit */
        uint32_t                    fn1_clintrd :  1; /*10:10,          interrupt enable bit */
        uint32_t                  fn1_int_en_up :  1; /*11:11,          interrupt enable bit */
        uint32_t                     reserved_1 :  8; /*19:12,                             NA*/
        uint32_t                    fn1_m2s_int :  1; /*20:20,          interrupt enable bit */
        uint32_t                     reserved_0 : 11; /*31:21,                             NA*/
    } bit_field;
} sdio_interrupt_enable_ctrl_t;

typedef struct {
    uint8_t                         supp_func_num;
    uint8_t                         clr_busy_sd;
    uint8_t                         csa_support;
    uint8_t                         supp_high_speed;
    uint16_t                        card_cap_sd;
    uint8_t                         *cis_fn0_base;
    uint8_t                         *cis_fn1_base;
    uint8_t                         *from_host_buffer;
    sdio_interrupt_enable_ctrl_t    int_en_ctrl;
} sdio_config_t;

uint8_t *ll_sdio_cis_func_get(sdio_func_enum_t fn);
bool ll_sdio_cis_fn_set(sdio_func_enum_t fn, uint32_t offset, uint8_t value);
uint32_t *ll_sdio_receive_from_host_buffer_get(void);
void ll_sdio_receive_from_host_buffer_set(uint8_t *addr);
uint16_t ll_sdio_receive_from_host_buffer_size_get(void);
uint32_t *ll_sdio_xfer_to_host_buffer_get(void);
void ll_sdio_xfer_to_host_buffer_set(uint8_t *addr);
void ll_sdio_xfer_to_host_buffer_size_set(uint32_t size);
void ll_sdio_triggle_data1_interrupt_to_host(void);
void ll_sdio_init(sdio_config_t *config);
uint32_t ll_sdio_get_interrupt_status(void);
void ll_sdio_set_interrupt_status(uint32_t st);
void ll_sdio_clear_busy_sd(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __LL_SDIO_H__


