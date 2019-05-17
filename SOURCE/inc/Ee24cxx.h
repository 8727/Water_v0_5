#ifndef _EE24CXX_H
#define _EE24CXX_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"


/* Define --------------------------------------------------------------------*/

void Ee24cxxRead(uint8_t addr, uint8_t* data, uint16_t len);
void Ee24cxxWritePage(uint8_t page, uint8_t *data, uint8_t len);
void Ee24cxxWriteByte(uint8_t addr, uint8_t data);
void Ee24cxxInit(void);

#endif /* _EEPROM24CXX_H */
