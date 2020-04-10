#ifndef __EEPROM_H
#define __EEPROM_H


int EE_Init(uint16_t *buffer, uint32_t size);
int EE_DeInit(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#endif /* __EEPROM_H */

