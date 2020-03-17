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
#ifndef H_MPU9250
#define H_MPU9250
// MPU9250 (Nine-Axis (Gyro + Accelerometer + Compass) sensor using I2C bus) is on IOTDK board
#include "device/ip_hal/dev_iic.h"

#define MPU9250_USE_DMP

#define MPU9250_AD0_PIN     0   /*!< I2C Serial Bus Address Selection Pin */
#define MPU9250_IIC_ADDRESS (0x68 + (MPU9250_AD0_PIN << 1))

#define MAG_IIC_ADDRESS     0x0C
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
	int16_t mag_x;
	int16_t mag_y;
	int16_t mag_z;
	float pitch;
	float roll;
	float yaw;
} MPU9250_DATA, *MPU9250_DATA_PTR;

typedef struct {
	uint32_t i2c_id;
	uint32_t mpu_slvaddr;
	uint32_t mag_slvaddr;

} MPU9250_DEF, *MPU9250_DEF_PTR;

#define MPU9250_DEFINE(NAME, I2C_ID, SLAVE_ADDRESS) \
	MPU9250_DEF __ ## NAME = {		    \
		.i2c_id = I2C_ID,		    \
		.mpu_slvaddr = SLAVE_ADDRESS,	    \
		.mag_slvaddr = MAG_IIC_ADDRESS	    \
	};					    \

	MPU9250_DEF_PTR NAME = &__ ## NAME

extern int32_t mpu9250_sensor_init(MPU9250_DEF_PTR obj);
extern int32_t mpu9250_sensor_deinit(MPU9250_DEF_PTR obj);
extern int32_t mpu9250_sensor_read(MPU9250_DEF_PTR obj, MPU9250_DATA_PTR mp_data);
extern int32_t mpu_iic_read(uint32_t slaveaddr, uint8_t regaddr, uint8_t len, uint8_t *val);
extern int32_t mpu_iic_write(uint32_t slaveaddr, uint8_t regaddr, uint8_t len, uint8_t *val);
int mpu_get_ms(unsigned long *count);
void mpu_delay_ms(uint32_t ms);
#ifdef __cplusplus
}
#endif

#endif /* H_MPU9250 */