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
struct InInitTypeDef{
  _Bool hot;
  _Bool cold;
  _Bool open;
  _Bool closed;
  _Bool wait;
  _Bool det;
};

extern struct WaterInitTypeDef water;
extern struct InInitTypeDef in;

void InCounterWater(void);
void InUpdate10Hz(void);
void InInit(void);

#endif /* _FAN_H */
