/*
 * myEEPROM.h
 *
 *  Created on: Mar 11, 2021
 *      Author: Andi
 */

#ifndef INC_MYEEPROM_H_
#define INC_MYEEPROM_H_

#include "main.h"

#define PAGE_SIZE		32
#define EEPROM_ADDR		0xA0

I2C_HandleTypeDef hi2c3;

void EEPROM_Write(uint16_t page, uint16_t offset, uint8_t *pData, uint16_t size);
void EEPROM_Read(uint16_t page, uint16_t offset, uint8_t *pData, uint16_t size);

void EEPROM_WriteByte(uint16_t page, uint16_t address, uint8_t *pData);
void EEPROM_ReadByte(uint16_t page, uint16_t address, uint8_t *pData);

void EEPROM_ErasePage(uint16_t page);


#endif /* INC_MYEEPROM_H_ */
