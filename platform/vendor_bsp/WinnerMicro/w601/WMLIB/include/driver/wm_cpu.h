/**
 * @file    wm_cpu.h
 *
 * @brief   cpu driver module
 *
 * @author  dave
 *
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_CPU_H
#define WM_CPU_H
#ifdef __cplusplus
extern "C" {
#endif

/** cpu clock: 80Mhz */
#define CPU_CLK_80M     0
/** cpu clock: 40Mhz */
#define CPU_CLK_40M     1
#define CPU_CLK_16M		2

#define W600_PLL_CLK_MHZ  		160



#define UNIT_MHZ		(1000000)


typedef struct{
	u32 apbclk;
	u32 cpuclk;
	u32 wlanclk;
}tls_sys_clk;

/**
 * @defgroup Driver_APIs Driver APIs
 * @brief Driver APIs
 */

/**
 * @addtogroup Driver_APIs
 * @{
 */

/**
 * @defgroup CPU_CLOCK_Driver_APIs CPU_CLOCK Driver APIs
 * @brief CPU_CLOCK driver APIs
 */

/**
 * @addtogroup CPU_CLOCK_Driver_APIs
 * @{
 */

/**
 * @brief          This function is used to set cpu clock
 *
 * @param[in]      	clk    select cpu clock
 *                        	clk == CPU_CLK_80M	80M
 *				clk == CPU_CLK_40M	40M
 *
 * @return         None
 *
 * @note           None
 */
void tls_sys_clk_set(u32 clk);


/**
 * @brief          	This function is used to get cpu clock
 *
 * @param[out]     *sysclk	point to the addr for system clk output
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_sys_clk_get(tls_sys_clk *sysclk);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* end of WM_CPU_H */

/*** (C) COPYRIGHT 2014 Winner Microelectronics Co., Ltd. ***/

