#ifndef _DS18B20_H
#define _DS18B20_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"
#include <string.h>

/* Define --------------------------------------------------------------------*/
#define DS18B20_SEARCH                      0xF0 //команда поиска ROM
#define DS18B20_SKIP_ROM                    0xCC //команда пропуска ROM
#define DS18B20_MATCH_ROM                   0x55 //команда совпадение ROM позволяет мастеру обращаться к конкретному устройству
#define DS18B20_CONVERT_TEMPERATURE         0x44
#define DS18B20_READ_SCRATCHPAD             0xBE //команда для чтения памяти датчика
#define DS18B20_WRITE_SCRATCHPAD            0x4E //команда запись в память дтачика

#define DS18B20_MAX_DEVICES                 0x08

struct Ds18b20InitTypeDef{
  uint8_t deviceID[0x08];
  int8_t  temperature;
  uint8_t fraction;
};

extern struct Ds18b20InitTypeDef ds18b20[DS18B20_MAX_DEVICES];
extern uint8_t ds18b20Device;

void Ds18b20Read(void);
void Ds18b20Init(void);


#endif /* _DS18B20_H */
