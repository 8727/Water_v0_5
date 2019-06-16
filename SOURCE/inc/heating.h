#ifndef _HEATING_H
#define _HEATING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/
#define R1_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR5  // R1
#define R1_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS5  // R1

#define R2_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR4  // R2
#define R2_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS4  // R2

#define R3_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR3  // R3
#define R3_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS3  // R3

#define R4_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR2  // R4
#define R4_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS2  // R4

#define R5_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR1  // R5
#define R5_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS1  // R5

#define HEATING_MAX_DEVICES          0x05

struct heatingInitTypeDef{
  uint8_t  valveCounter;
  uint8_t  valveDefDelay;      // 10 секунд работаем (питание на клапан), 40 секунд отдыхаем
  uint16_t valveInterval;      // общее время работы с клапаном 50 секунд.
  uint8_t  counter;
  uint16_t interval;           // 60  каждые 60 секунд определяем - надо ли изменить продолжительность питания/отдыха
  uint8_t  gistTemperature;    // 0.3 гистерезис. греть/охлаждать перестаем на 0.3 градуса раньше
  uint8_t  stepDelay;          // 0.2 шаг изменения ширины импульса (за раз изменяем на 1 секунду)
  uint16_t topTemperature;     // 50  если температура выше критической - сбрасываем настройку $delay
  uint8_t  maxDelay;           // 20  не греем клапан дольше 20 сек
};

struct heatingValveInitTypeDef{
  uint8_t  status;
  uint16_t temperature;         // 30 целевая температура
  uint8_t  delay;               // 0  время работы с клапаном
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
  uint8_t  freezeCounter;       //    пока идёт обратный отсчет freezeCounter (до нуля) мы не пытаемся влиять на клапан, а просто наблюдаем (кроме аварийных случаев)
  uint16_t oldTemperature[0x03];//    старая температура 3x сек назад, 2x сек назад, 1x сек назад
  uint16_t realTemperature;     //    температура
};

extern struct heatingInitTypeDef heating;
extern struct heatingValveInitTypeDef heatingValve[HEATING_MAX_DEVICES];

void HeatingInit(void);

#endif /* _HEATING_H */
