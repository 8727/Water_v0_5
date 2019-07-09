#ifndef _CAN_H
#define _CAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"


/* Define --------------------------------------------------------------------*/
#define CAN_FILTER_16bit       0x00
#define CAN_FILTER_32bit       0x01
#define CAN_FILTER_MASK        0x00


typedef struct{
  uint16_t stdId;
  uint32_t extId;
  uint8_t ide;
  uint8_t rtr;
  uint8_t dlc;
  uint8_t data[8];
}CanTxMsg;

typedef struct{
  uint32_t stdId;
  uint32_t extId;
  uint8_t ide;
  uint8_t rtr;
  uint8_t dlc;
  uint8_t data[8];
  uint8_t fmi;
}CanRxMsg;

typedef struct{
  uint16_t canFilterIdHigh;
  uint16_t canFilterIdLow;
  uint16_t canFilterMaskIdHigh;
  uint16_t canFilterMaskIdLow;
  uint16_t canFilterFIFOAssignment;
  uint8_t canFilterNumber;
  uint8_t canFilterMode;
  uint8_t canFilterScale;
  FunctionalState CanFilterActivation;
}CanFilterInitTypeDef;


uint8_t CanSendMsg(CanTxMsg* txMessage);
void CanReceiveMsg(uint8_t numberFIFO, CanRxMsg* rxMessage);
void CanFilterInit(CanFilterInitTypeDef* canFilterInitStruct);

void CanInit(void);

#endif /* _CAN_H */
