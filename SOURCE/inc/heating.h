#ifndef _HEATING_H
#define _HEATING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/
#define R1_OPEN                          GPIOE->BSRR = GPIO_BSRR_BR5  // R1
#define R1_CLOSED                        GPIOE->BSRR = GPIO_BSRR_BS5  // R1

#define R2_OPEN                          GPIOE->BSRR = GPIO_BSRR_BR4  // R2
#define R2_CLOSED                        GPIOE->BSRR = GPIO_BSRR_BS4  // R2

#define R3_OPEN                          GPIOE->BSRR = GPIO_BSRR_BR3  // R3
#define R3_CLOSED                        GPIOE->BSRR = GPIO_BSRR_BS3  // R3

#define R4_OPEN                          GPIOE->BSRR = GPIO_BSRR_BR2  // R4
#define R4_CLOSED                        GPIOE->BSRR = GPIO_BSRR_BS2  // R4

#define R5_OPEN                          GPIOE->BSRR = GPIO_BSRR_BR1  // R5
#define R5_CLOSED                        GPIOE->BSRR = GPIO_BSRR_BS1  // R5

#define HEATING_MAX_DEVICES              0x05

struct heatingInitTypeDef{
  _Bool    job;
  uint16_t realTemperature;     //    температура
  uint16_t oldTemperature[0x03];//    старая температура 3x сек назад, 2x сек назад, 1x сек назад
  uint16_t temperature;         // 30 установленная температура
  uint16_t jobDelay;            // 0  время работы с клапаном
  int8_t   direction;           // 0  направление к целевой температуре. +1 хотим повысить, -1  хотим понизить. 0 - ничего не хотим
  uint8_t  hotAlarm;            // 0  один раз при перегреве снижаем $delay
  uint8_t  coldAlarm;           // 0  один раз если температура резко снижается больше 1.5 градусов за 4 минуты - увеличиваем $delay (пытаемся замедлить снижение, чтобы клапан не закрылся целиком)
  uint8_t  growCounter;         //    счетчик шансов на рост. когда температура сама растет - просто ждем, вдруг дорастёт сама
                                //    но если 3 раза подряд роста до целевой температуры не произошло - значит нужно
                                //    усилить воздействие (ширину импульса)
  uint8_t  growTimer;           // 0  таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
                                //    если (таймер > 10 x interval && delay < valveDefDelay) тогда delay = valveDefDelay 
                                //    для случаев когда после сильного падения delay (например, до нуля) замучаешься ждать роста
                                //    ведь с шагом 0.2 необходимо 12 / 0.2 * 2мин = 96 минут
  uint16_t freezeCounter;       //    пока идёт обратный отсчет freezeCounter (до нуля) мы не пытаемся влиять на клапан, а просто наблюдаем (кроме аварийных случаев)
};

extern uint16_t timerHeating;
extern uint16_t timerSensor;
extern struct heatingInitTypeDef heating[HEATING_MAX_DEVICES];

void HeatingPWM(void);
void HeatingInit(void);

#endif /* _HEATING_H */
