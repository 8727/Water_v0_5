#include "can.h"

void USB_HP_CAN_TX_IRQHandler(void){

}

void USB_LP_CAN_RX0_IRQHandler(void){
  
  
}

void CAN1_RX1_IRQHandler(void){

}

void CAN1_SCE_IRQHandler(void){

}


uint8_t CanSendMsg(CanTxMsg* txMessage){
  uint8_t transmitMailbox = 0x00;
  if((CAN1->TSR&CAN_TSR_TME0) == CAN_TSR_TME0){
    transmitMailbox = 0x00;
  }else if((CAN1->TSR&CAN_TSR_TME1) == CAN_TSR_TME1){
    transmitMailbox = 0x01;
  }else if((CAN1->TSR&CAN_TSR_TME2) == CAN_TSR_TME2){
    transmitMailbox = 0x02;
  }else{
    transmitMailbox = 0x04;
  }
  if(transmitMailbox != 0x04){
    CAN1->sTxMailBox[transmitMailbox].TIR &= 0x01;
    if(txMessage->ide == 0x00){ 
      CAN1->sTxMailBox[transmitMailbox].TIR |= ((txMessage->stdId << 0x15) | \
                                                  txMessage->rtr);
    }else{
      CAN1->sTxMailBox[transmitMailbox].TIR |= ((txMessage->extId << 0x03) | \
                                                  txMessage->ide | \
                                                  txMessage->rtr);
    }
    txMessage->dlc &= (uint8_t)0x0F;
    CAN1->sTxMailBox[transmitMailbox].TDTR &= (uint32_t)0xFFFFFFF0;
    CAN1->sTxMailBox[transmitMailbox].TDTR |= txMessage->dlc;
    CAN1->sTxMailBox[transmitMailbox].TDHR = (((uint32_t)txMessage->data[0x07] << 0x18) |
                                             ((uint32_t)txMessage->data[0x06] << 0x10) |
                                             ((uint32_t)txMessage->data[0x05] << 0x08) |
                                             ((uint32_t)txMessage->data[0x04]));
    CAN1->sTxMailBox[transmitMailbox].TDLR = (((uint32_t)txMessage->data[0x03] << 0x18) |
                                             ((uint32_t)txMessage->data[0x02] << 0x10) |
                                             ((uint32_t)txMessage->data[0x01] << 0x08) |
                                             ((uint32_t)txMessage->data[0x00]));
    CAN1->sTxMailBox[transmitMailbox].TIR |= 0x01;
  }
  return transmitMailbox;
}

void CanReceiveMsg(uint8_t numberFIFO, CanRxMsg* rxMessage){
  rxMessage->ide = (uint8_t)0x04 & CAN1->sFIFOMailBox[numberFIFO].RIR;
  if(rxMessage->ide == 0x00){
    rxMessage->stdId = (uint32_t)0x000007FF & (CAN1->sFIFOMailBox[numberFIFO].RIR >> 0x15);
  }else{
    rxMessage->extId = (uint32_t)0x1FFFFFFF & (CAN1->sFIFOMailBox[numberFIFO].RIR >> 0x03);
  }
  rxMessage->rtr = (uint8_t)0x02 & CAN1->sFIFOMailBox[numberFIFO].RIR;
  rxMessage->dlc = (uint8_t)0x0F & CAN1->sFIFOMailBox[numberFIFO].RDTR;
  rxMessage->fmi = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDTR >> 0x08);
  rxMessage->data[0x00] = (uint8_t)0xFF & CAN1->sFIFOMailBox[numberFIFO].RDLR;
  rxMessage->data[0x01] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDLR >> 0x08);
  rxMessage->data[0x02] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDLR >> 0x10);
  rxMessage->data[0x03] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDLR >> 0x18);
  rxMessage->data[0x04] = (uint8_t)0xFF & CAN1->sFIFOMailBox[numberFIFO].RDHR;
  rxMessage->data[0x05] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDHR >> 0x08);
  rxMessage->data[0x06] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDHR >> 0x10);
  rxMessage->data[0x07] = (uint8_t)0xFF & (CAN1->sFIFOMailBox[numberFIFO].RDHR >> 0x18);
  if(numberFIFO == 0x00){
    CAN1->RF0R |= CAN_RF0R_RFOM0;
  }else{
    CAN1->RF1R |= CAN_RF1R_RFOM1;
  }
}

void CanFilterInit(CanFilterInitTypeDef* canFilterInitStruct){
  uint32_t filterNumberBitPos = 0x00;
  filterNumberBitPos = ((uint32_t)1) << canFilterInitStruct->canFilterNumber;
  CAN1->FMR |= CAN_FMR_FINIT;
  CAN1->FA1R &= ~(uint32_t)filterNumberBitPos;
  if(canFilterInitStruct->canFilterScale == CAN_FILTER_16bit){
    CAN1->FS1R &= ~(uint32_t)filterNumberBitPos;
    CAN1->sFilterRegister[canFilterInitStruct->canFilterNumber].FR1 = 
    ((0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterMaskIdLow) << 0x10) |
        (0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterIdLow);
    CAN1->sFilterRegister[canFilterInitStruct->canFilterNumber].FR2 = 
    ((0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterMaskIdHigh) << 0x10) |
        (0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterIdHigh);
  }
  if(canFilterInitStruct->canFilterScale == CAN_FILTER_32bit){
    CAN1->FS1R |= filterNumberBitPos;
    CAN1->sFilterRegister[canFilterInitStruct->canFilterNumber].FR1 = 
    ((0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterIdHigh) << 0x10) |
        (0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterIdLow);
    CAN1->sFilterRegister[canFilterInitStruct->canFilterNumber].FR2 = 
    ((0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterMaskIdHigh) << 0x10) |
        (0x0000FFFF & (uint32_t)canFilterInitStruct->canFilterMaskIdLow);
  }
  if(canFilterInitStruct->canFilterMode == CAN_FILTER_MASK){
    CAN1->FM1R &= ~(uint32_t)filterNumberBitPos;
  }else{
    CAN1->FM1R |= (uint32_t)filterNumberBitPos;
  }
  if(canFilterInitStruct->canFilterFIFOAssignment == 0x00){
    CAN1->FFA1R &= ~(uint32_t)filterNumberBitPos;
  }
  if(canFilterInitStruct->canFilterFIFOAssignment == 0x01){
    CAN1->FFA1R |= (uint32_t)filterNumberBitPos;
  }
  if(canFilterInitStruct->CanFilterActivation == ENABLE){
    CAN1->FA1R |= filterNumberBitPos;
  }

  /* Leave the initialisation mode for the filter */
  CAN1->FMR &= ~CAN_FMR_FINIT;
}
void CanInit(void){
  GPIOA->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_CNF12);
  GPIOA->CRH |= (GPIO_CRH_CNF11_0 | GPIO_CRH_CNF12_1 | GPIO_CRH_MODE12);
  
  RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
  CAN1->MCR &= ~CAN_MCR_SLEEP;
  
  CAN1->MCR |= CAN_MCR_INRQ;
  while((CAN1->MSR & CAN_MSR_INAK) != CAN_MSR_INAK);
  
  CAN1->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
  
  CAN1->MCR &= ~CAN_MCR_TTCM;      //Time triggered communication mode ВЫКЛ
  CAN1->MCR &= ~CAN_MCR_ABOM;      //ВЫКЛ автоматический выход из bus-off
  CAN1->MCR &= ~CAN_MCR_AWUM;      //Авто. выход из спящего режима ВЫКЛ
  CAN1->MCR |= CAN_MCR_NART;      //Модуль будет отправлять сообщение, пока не отправится
  CAN1->MCR &= ~CAN_MCR_RFLM;      //Фифо не лочится, если оно заполнено и принято лишнее сообщение
  CAN1->MCR &= ~CAN_MCR_TXFP;      //Приоритет передачи сообщений определяется идентификаторами
  
  CAN1->BTR = CAN_SPEED;
  CAN1->BTR &= ~(CAN_BTR_SILM | CAN_BTR_LBKM);
  CAN1->BTR |= CanModeNormal;
  
  CAN1->MCR &= ~CAN_MCR_INRQ;
  while((CAN1->MSR & CAN_MSR_INAK) == CAN_MSR_INAK);
  
  CAN1->IER |= CAN_IER_FFIE0 | CAN_IER_FFIE1;
  
  NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, PRIORITY_CAN_TX);
  NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, PRIORITY_CAN_RX0);
  NVIC_SetPriority(CAN1_RX1_IRQn, PRIORITY_CAN_RX1);
  NVIC_SetPriority(CAN1_SCE_IRQn, PRIORITY_CAN_ERROR);
  
  NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
  NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  NVIC_EnableIRQ(CAN1_RX1_IRQn);
  NVIC_EnableIRQ(CAN1_SCE_IRQn);
  
  #if defined DEBUG_CAN
    printf("< OK >    Initialization CAN\r\n");
  #endif
}

