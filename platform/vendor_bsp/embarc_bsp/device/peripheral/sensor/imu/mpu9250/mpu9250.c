/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
   --------------------------------------------- */
#include "arc/arc.h"
#include "arc/arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "device/ip_hal/dev_iic.h"
#include "mpu9250.h"
#include "board.h"

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include <math.h>
#define DEFAULT_MPU_HZ 200
// ****************************************
#define SMPLRT_DIV  0x19
#define CONFIG      0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG    0x1C
#define ACCEL_CONFIG_2  0x1D
#define INT_PIN_CFG 0x37
#define INT_ENABLE  0x38
#define USER_CTRL   0x6A
#define PWR_MGMT_1  0x6B
#define PWR_MGMT_2  0x6C
#define MAG_CTRL    0x0A

#define MPU_WIM     0x75
#define MPU_ID      0x71

#define MAG_WIM     0x00
#define MAG_ID      0x48

#define ACCEL_XOUT_H    0x3B
#define GYRO_XOUT_H 0x43
#define MAG_XOUT_L  0x03

#ifdef MPU9250_USE_DMP
MPU9250_DEF_PTR mpu9250_ptr;
static signed char gyro_orientation[9] = { 1, 0, 0,
					   0, 1, 0,
					   0, 0, 1 };
static inline unsigned short inv_row_2_scale(const signed char *row)
{
	unsigned short b;

	if (row[0] > 0) {
		b = 0;
	} else if (row[0] < 0) {
		b = 4;
	} else if (row[1] > 0) {
		b = 1;
	} else if (row[1] < 0) {
		b = 5;
	} else if (row[2] > 0) {
		b = 2;
	} else if (row[2] < 0) {
		b = 6;
	} else {
		b = 7;    // error
	}

	return b;
}

static inline unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
	unsigned short scalar;

	/*
	    XYZ  010_001_000 Identity Matrix
	    XZY  001_010_000
	    YXZ  010_000_001
	    YZX  000_010_001
	    ZXY  001_000_010
	    ZYX  000_001_010
	 */
	scalar = inv_row_2_scale(mtx);
	scalar |= inv_row_2_scale(mtx + 3) << 3;
	scalar |= inv_row_2_scale(mtx + 6) << 6;

	return scalar;
}
static inline void run_self_test(void)
{
	int result;
	long gyro[3], accel[3];
	unsigned char i = 0;

	result = mpu_run_6500_self_test(gyro, accel, 1);
	EMBARC_PRINTF("mpu run self test, result = %d\r\n");

	if (result == 0x7) {
		for (i = 0; i < 3; i++) {
			gyro[i] = (long)(gyro[i] * 32.8f);      // convert to +-1000dps
			accel[i] *= 2048.f;                     // convert to +-16G
			accel[i] = accel[i] >> 16;
			gyro[i] = (long)(gyro[i] >> 16);
		}

		mpu_set_gyro_bias_reg(gyro);
		mpu_set_accel_bias_6500_reg(accel);
	}
}
#endif

#define MPU9250_CHECK_EXP_NORTN(EXPR)       CHECK_EXP_NOERCD(EXPR, error_exit)

static int32_t _mpu_reg_write(MPU9250_DEF_PTR obj, uint32_t slaveaddr, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_LESS_INFO, "[%s]%d: obj 0x%x, regaddr 0x%x, val 0x%x\r\n", __FUNCTION__, __LINE__, obj, regaddr, *val);
	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj != NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slaveaddr));

	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_write(val, len);

error_exit:
	return ercd;
}

static int32_t _mpu_reg_read(MPU9250_DEF_PTR obj, uint32_t slaveaddr, uint8_t regaddr, uint8_t *val, uint8_t len)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj != NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slaveaddr));

	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_read(val, len);

error_exit:
	return ercd;
}

int32_t mpu9250_sensor_init(MPU9250_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj != NULL);

	ercd = iic_obj->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);

	if ((ercd == E_OK) || (ercd == E_OPNED)) {
#ifndef MPU9250_USE_DMP
		uint8_t config;
		uint8_t data[0];
		config = 0x80;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, PWR_MGMT_1, &config, 1);// 0x6B
		board_delay_ms(100);

		/*
		 * get stable time source; Auto select clock source to be PLL gyroscope reference if ready
		 * else use the internal oscillator
		 */
		config = 0x01;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, PWR_MGMT_1, &config, 1);
		config = 0x00;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, PWR_MGMT_2, &config, 1);
		/* no i2c master */
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, USER_CTRL, &config, 1);
		board_delay_ms(200);

		ercd = _mpu_reg_read(obj, obj->mpu_slvaddr, MPU_WIM, data, 1);

		if (data[0] != MPU_ID) {
			dbg_printf(DBG_MORE_INFO, "mpu init failed\r\n");
			return E_SYS;
		}

		config = 0x07;                                                          // SAMPLE_RATE=Internal_Sample_Rate(1khz) / (1 + SMPLRT_DIV)
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, SMPLRT_DIV, &config, 1);   // Sample Rate Divider

		config = 0x06;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, CONFIG, &config, 1);// DLPF config: 5Hz

		config = 0x18;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, GYRO_CONFIG, &config, 1);// +2000dps

		config = 0x00;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, ACCEL_CONFIG, &config, 1);// +-2g

		config = 0x08;
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, ACCEL_CONFIG_2, &config, 1);       // 1.13kHz

		config = 0x2;                                                                   // set passby
		ercd = _mpu_reg_write(obj, obj->mpu_slvaddr, INT_PIN_CFG, &config, 1);
		board_delay_ms(100);

		ercd = _mpu_reg_read(obj, obj->mag_slvaddr, MAG_WIM, data, 1);// read mag who i am;

		if (data[0] != MAG_ID) {
			dbg_printf(DBG_MORE_INFO, "mpu init failed\r\n");
			return E_SYS;
		}

		config = 0x01;
		ercd = _mpu_reg_write(obj, obj->mag_slvaddr, MAG_CTRL, &config, 1);// mag single measurement mode
#else
		mpu9250_ptr = obj;

		if (!mpu_init()) {
			if (!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
				EMBARC_PRINTF("mpu_set_sensor complete ......\r\n");
			}

			mpu_delay_ms(50);

			if (!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
				EMBARC_PRINTF("mpu_configure_fifo complete ......\r\n");
			}

			mpu_delay_ms(50);

			if (!mpu_set_sample_rate(DEFAULT_MPU_HZ)) {
				EMBARC_PRINTF("mpu_set_sample_rate complete ......\r\n");
			}

			mpu_delay_ms(50);
			// if(!mpu_set_gyro_bias_reg(gyroZero))
			//	EMBARC_PRINTF("mpu_set_gyro_bias_reg complete ......\r\n");
			mpu_delay_ms(50);

			if (!dmp_load_motion_driver_firmware()) {
				EMBARC_PRINTF("dmp_load_motion_driver_firmware complete ......\r\n");
			}

			mpu_delay_ms(50);

			if (!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation))) {
				EMBARC_PRINTF("dmp_set_orientation complete ......\r\n");
			}

			mpu_delay_ms(50);

			if (!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL |
						DMP_FEATURE_SEND_RAW_GYRO)) {
				// DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL))
				EMBARC_PRINTF("dmp_enable_feature complete ......\r\n");
			}

			mpu_delay_ms(50);

			if (!dmp_set_fifo_rate(DEFAULT_MPU_HZ)) {
				EMBARC_PRINTF("dmp_set_fifo_rate complete ......\r\n");
			}

			mpu_delay_ms(50);
			// run_self_test();
			mpu_delay_ms(50);

			if (!mpu_set_dmp_state(1)) {
				EMBARC_PRINTF("mpu_set_dmp_state complete ......\r\n");
			}

			mpu_delay_ms(50);
		}

#endif
	}

error_exit:
	return ercd;
}

int32_t mpu9250_sensor_deinit(MPU9250_DEF_PTR obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_PTR iic_obj = iic_get_dev(obj->i2c_id);

	ercd = iic_obj->iic_close();
	MPU9250_CHECK_EXP_NORTN(ercd == E_OK);

error_exit:
	return ercd;
}

int32_t mpu9250_sensor_read(MPU9250_DEF_PTR obj, MPU9250_DATA_PTR mp_data)
{
	int32_t ercd = E_OK;

	MPU9250_CHECK_EXP_NORTN(mp_data != NULL);
#ifndef MPU9250_USE_DMP
	uint8_t data[6];
	uint8_t config;
	ercd = _mpu_reg_read(obj, obj->mpu_slvaddr, GYRO_XOUT_H, data, 6);

	if (ercd != 6) {
		ercd = E_OBJ;
		goto error_exit;
	} else {
		ercd = E_OK;

		mp_data->gyro_x = ((int16_t)data[0] << 8) + ((int16_t)data[1]);
		mp_data->gyro_y = ((int16_t)data[2] << 8) + ((int16_t)data[3]);
		mp_data->gyro_z = ((int16_t)data[4] << 8) + ((int16_t)data[5]);
	}

	ercd = _mpu_reg_read(obj, obj->mpu_slvaddr, ACCEL_XOUT_H, data, 6);

	if (ercd != 6) {
		ercd = E_OBJ;
		goto error_exit;
	} else {
		ercd = E_OK;

		mp_data->accel_x = ((int16_t)data[0] << 8) + ((int16_t)data[1]);
		mp_data->accel_y = ((int16_t)data[2] << 8) + ((int16_t)data[3]);
		mp_data->accel_z = ((int16_t)data[4] << 8) + ((int16_t)data[5]);
	}

	ercd = _mpu_reg_read(obj, obj->mag_slvaddr, MAG_XOUT_L, data, 6);

	if (ercd != 6) {
		ercd = E_OBJ;
	} else {
		ercd = E_OK;

		mp_data->mag_x = ((int16_t)data[1] << 8) + ((int16_t)data[0]);
		mp_data->mag_y = ((int16_t)data[3] << 8) + ((int16_t)data[2]);
		mp_data->mag_z = ((int16_t)data[5] << 8) + ((int16_t)data[4]);
	}

	config = 0x01;
	ercd = _mpu_reg_write(obj, obj->mag_slvaddr, MAG_CTRL, &config, 1);// mag single measurement mode
#else
	short gyro[3], accel[3], sensors;
	float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
	unsigned long sensor_timestamp;
	unsigned char more;
	long quat[4];
	float q30 = (float)(1 << 30);

	if (dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more) == 0) {
		mp_data->gyro_x = gyro[0];
		mp_data->gyro_y = gyro[1];
		mp_data->gyro_z = gyro[2];
		mp_data->accel_x = accel[0];
		mp_data->accel_y = accel[1];
		mp_data->accel_z = accel[2];

		if (sensors & INV_WXYZ_QUAT) {
			q0 = quat[0] / q30;
			q1 = quat[1] / q30;
			q2 = quat[2] / q30;
			q3 = quat[3] / q30;
			mp_data->pitch = (float)asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3f;
			mp_data->roll = (float)atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3f;
			mp_data->yaw = (float)atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3f;
			return 0;
		}
	}

#endif
error_exit:
	return ercd;
}

#ifdef MPU9250_USE_DMP
int32_t mpu_iic_write(uint32_t slaveaddr, uint8_t regaddr, uint8_t len, uint8_t *val)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(mpu9250_ptr->i2c_id);

	dbg_printf(DBG_LESS_INFO, "[%s]%d: obj 0x%x, regaddr 0x%x, val 0x%x\r\n", __FUNCTION__, __LINE__, obj, regaddr, *val);
	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj != NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slaveaddr));

	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_write(val, len);

	if (ercd == len) {
		ercd = E_OK;
	}

error_exit:
	return ercd;
}

int32_t mpu_iic_read(uint32_t slaveaddr, uint8_t regaddr, uint8_t len, uint8_t *val)
{
	int32_t ercd = E_PAR;
	uint8_t data[1];
	DEV_IIC_PTR iic_obj = iic_get_dev(mpu9250_ptr->i2c_id);

	dbg_printf(DBG_MORE_INFO, "[%s]%d: iic_obj 0x%x -> 0x%x\r\n", __FUNCTION__, __LINE__, iic_obj, *iic_obj);
	MPU9250_CHECK_EXP_NORTN(iic_obj != NULL);

	data[0] = (uint8_t)(regaddr & 0xff);

	iic_obj->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(slaveaddr));

	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_RESTART));
	ercd = iic_obj->iic_write(data, 1);
	ercd = iic_obj->iic_control(IIC_CMD_MST_SET_NEXT_COND, CONV2VOID(IIC_MODE_STOP));
	ercd = iic_obj->iic_read(val, len);

	if (ercd == len) {
		ercd = E_OK;
	}

error_exit:
	return ercd;
}

int mpu_get_ms(unsigned long *count)
{
	*count = (unsigned long)OSP_GET_CUR_MS();
	return 0;
}
void mpu_delay_ms(uint32_t ms)
{
	board_delay_ms(ms);
}

#endif
