#ifndef __SOFT_I2C_H__
#define __SOFT_I2C_H__
#ifdef __cplusplus
    extern "C" {
#endif // __cplusplus

#include "types.h"

void I2C_Delay(uint16_t dly);
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_GetAck(void);
void I2C_PutAck(uint8_t ack);
void I2C_WriteByte(uint8_t data);
uint8_t I2C_ReadByte(uint8_t ack);

uint8_t OLED_Write_Command(uint8_t command);
uint8_t OLED_Write_Data(uint8_t data);
void OLED_Fill(uint8_t _8pixel);
void OLED_ClearScreen(void);
void OLED_FillDot(void);
void OLED_On(void);
void OLED_Off(void);
void OLED_StartDisplay(void);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __SOFT_I2C_H__
