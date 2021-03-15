/*
 * myEEPROM.c
 *
 *  Created on: Mar 11, 2021
 *      Author: Andi
 */
#include "myEEPROM.h"

uint16_t bytesToWrite(uint16_t size, uint16_t offset){
	if((size+offset)<32){
		return size;
	} else{
		return 32 - offset;
	}
}

void EEPROM_Write(uint16_t page, uint16_t offset, uint8_t *pData, uint16_t size){
	int pAddrPos = log(32)/log(2);

	uint16_t startPage = page;
	uint16_t endPage = page + ((size+offset)/32);

	uint16_t numOfPages = endPage - startPage;
	uint16_t pos = 0;

	for(int16_t i=0; i<numOfPages; i++){
		uint16_t MemAddress = startPage << (pAddrPos | offset);
		uint16_t bytesRemaining = bytesToWrite(size, offset);

		HAL_I2C_Mem_Write(&hi2c3, 0xA0, MemAddress, I2C_MEMADD_SIZE_16BIT, &pData[pos], bytesRemaining, 1000);

		startPage += 1;
		offset = 0;

		size = size - bytesRemaining;
		pos += bytesRemaining;

		HAL_Delay(5);
	}
}

void EEPROM_Read(uint16_t page, uint16_t offset, uint8_t *pData, uint16_t size){
	int16_t pAddrPos = log(PAGE_SIZE)/log(2);

	uint16_t startPage = page;
	uint16_t endPage = page + ((size+offset)/PAGE_SIZE);

	uint16_t numOfPages = endPage - startPage;
	uint16_t pos = 0;

	for(int16_t i=0; i<numOfPages; i++){
		uint16_t MemAddress = startPage << (pAddrPos | offset);
		uint16_t bytesRemaining = bytesToWrite(size, offset);

		HAL_I2C_Mem_Read(&hi2c3, EEPROM_ADDR, MemAddress, I2C_MEMADD_SIZE_16BIT, &pData[pos], bytesRemaining, 1000);

		startPage += 1;
		offset = 0;
		size = size - bytesRemaining;
		pos  = pos + bytesRemaining;
	}
}

void EEPROM_WriteByte(uint16_t page, uint16_t address, uint8_t *pData){
	int16_t pAddrPos = log(PAGE_SIZE)/log(2);

	uint16_t MemAddress = page << pAddrPos;

	HAL_I2C_Mem_Write(&hi2c3, EEPROM_ADDR, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, 1, 1000);
	HAL_Delay(5);
}

void EEPROM_ReadByte(uint16_t page, uint16_t address, uint8_t *pData){
	int16_t pAddrPos = log(PAGE_SIZE)/log(2);

	uint16_t MemAddress = page << pAddrPos;

	HAL_I2C_Mem_Read(&hi2c3, EEPROM_ADDR, MemAddress, I2C_MEMADD_SIZE_16BIT, pData, 1, 1000);
}

void EEPROM_ErasePage(uint16_t page){
	int16_t pAddrPos = log(PAGE_SIZE)/log(2);
	uint16_t MemAddress = page<<pAddrPos;

	uint8_t data[PAGE_SIZE];
	memset(data, 0xFF, PAGE_SIZE);

	HAL_I2C_Mem_Write(&hi2c3, 0xA0, MemAddress, I2C_MEMADD_SIZE_16BIT, data, PAGE_SIZE, 1000);
	HAL_Delay(5);
}
