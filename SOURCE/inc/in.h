#ifndef _IN_H
#define _IN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/


struct WaterInitTypeDef{
  uint32_t hotCounter;
  uint32_t coldCounter;
  uint16_t calibHot;
  uint16_t calibCold;
  uint16_t calibOutHot;
  uint16_t calibOutCold;
};

extern struct WaterInitTypeDef water;

void InInit(void);

#endif /* _FAN_H */
