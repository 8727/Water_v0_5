#include "heating.h"

uint16_t timerHeating;
uint16_t timerSensor;
struct heatingInitTypeDef heating[HEATING_MAX_DEVICES];

void HeatingPWM(void){ // 10Hz
  for(uint8_t i = 0x00; i < HEATING_MAX_DEVICES; i++){
    if(0x00 == timerSensor){ 
      timerSensor = settings.heatSensorInterval;
      heating[i].oldTemperature[0x02] = heating[i].oldTemperature[0x01];
      heating[i].oldTemperature[0x01] = heating[i].oldTemperature[0x00];
      heating[i].oldTemperature[0x00] = heating[i].realTemperature;
    }
    // управляем воздействием в зависимости от текущей температуры
    if(heating[i].freezeCounter > 0x00){ // между точками воздействия реагируем на аварийные ситуации 
     heating[i].freezeCounter--;
     // вдруг мы пытаемся нагревать, а температура превысила уставку?
     // нужно срочно остудить на одну ступень, а не ждать кучу времени ($freezeCounter) пока перегреется
     // но только один раз
      if(heating[i].realTemperature > heating[i].temperature + settings.heatGistTemperature &&
          heating[i].direction == 0x01 && heating[i].hotAlarm == 0x00){
        heating[i].direction = -0x01;
        heating[i].growCounter = 0x00; // обнуляем счетчик шансов на прогрев (перегрелись, куда еще :)
        heating[i].hotAlarm++;
        heating[i].jobDelay -= settings.heatStepDelay;
      }
      // резкое падение нужно один раз замедлить
      if(heating[i].coldAlarm < 0x01 && heating[i].oldTemperature[0x02] > heating[i].oldTemperature[0x01] &&
          heating[i].oldTemperature[0x01] > heating[i].realTemperature &&
          heating[i].oldTemperature[0x02] - heating[i].realTemperature > 0x0F){ // 1.5C
        heating[i].jobDelay += settings.heatStepDelay;  
        heating[i].coldAlarm++;
      } 
    }else{
      // противоаварийный сброс настроек, если температура выше критической 4 минуты подряд
      if(heating[i].realTemperature > settings.heatTopTemperature && 
          heating[i].oldTemperature[0x01] > settings.heatTopTemperature && 
          heating[i].oldTemperature[0x02] > settings.heatTopTemperature){ heating[i].jobDelay = settings.heatJobDefDelay;}
          
      // если температура выше критической и delay на максимуме - сбрасываем настройки, что-то резко изменилось
      if(heating[i].realTemperature > settings.heatTopTemperature &&
          heating[i].jobDelay >= (settings.heatMaxDelay - settings.heatStepDelay)){ heating[i].jobDelay = settings.heatJobDefDelay;}
          
      // а если температура = установке $temperature, тогда вообще ничего делать не нужно
      // тогда остановим $freezeCounter, пока температура не выйдет за пределы
      if(heating[i].realTemperature < (heating[i].temperature - settings.heatGistTemperature)){ // если температура ниже заданной
        heating[i].growTimer++;
//          #if defined DEBUG_HEATING
//            printf("HEATING growTimer %d - %d;  jobDelay %d\r\n", i, heating[i].growTimer, heating[i].jobDelay);
//          #endif
        // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
        // если (таймер > 10 x interval && delay < valveDefDelay) тогда delay = valveDefDelay  
        // для случаев когда после сильного падения delay (например, до нуля) замучаешься ждать роста
        // ведь с шагом 0.25 необходимо 12 / 0.25 * 2мин = 96 минут
        if(heating[i].growTimer > (settings.heatJobInterval / 0x64) && heating[i].jobDelay < settings.heatJobDefDelay){
          heating[i].jobDelay = settings.heatJobDefDelay;
        }
        // нужно сильнее открывать клапан (увеличить delay)
        heating[i].coldAlarm = 0x00; // обнуляем счетчик попыток замедлить падение температуры
        heating[i].direction = 0x01; 
        // минуточку, может быть температура уже растёт? зачем еще сильнее открывать клапан?
        if(heating[i].oldTemperature[0x02] < heating[i].oldTemperature[0x01] && heating[i].oldTemperature[0x01] < heating[i].realTemperature){
        if(heating[i].growCounter < 0x04){
          heating[i].growCounter++; // температура и так растет. даём шанс
        }else{
          if(heating[i].realTemperature - heating[i].oldTemperature[0x02] >= 0x0F){ //  шансы на рост уже исчерпаны. но вдруг температура уже растёт хорошими темпами?
          // растёт хорошо! 1.5 градуса за 4 минуты, будем ждать сколько потребуется!
          }else{ heating[i].jobDelay += settings.heatStepDelay;}// рост слишком медленный, увеличиваем ширину импульса на термопривод
        }
       }else{
        heating[i].jobDelay += settings.heatStepDelay;
        // если до уставки осталось более 3 градусов - ускоряем разогрев (увеличим delay еще на ступень stepDelay)
        if((heating[i].temperature - settings.heatGistTemperature) - heating[i].realTemperature > 30 ) heating[i].jobDelay += settings.heatStepDelay;
       }
      if(heating[i].jobDelay > settings.heatMaxDelay) heating[i].jobDelay = settings.heatMaxDelay; // не позволяем подавать питания на термопривод импульсами дольше чем maxDelay

      heating[i].freezeCounter = settings.heatJobInterval * 0x02; // термопривод долго реагирует, не торопимся
      }else{
        if(heating[i].realTemperature > (heating[i].temperature + settings.heatGistTemperature)){ // нужно слабее открывать клапан (увеличить delay)
          heating[i].direction -= 0x01;
          heating[i].growTimer = 0x00; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
          heating[i].hotAlarm = 0x00; // обнуляем счетчик аварийных охлаждений
          heating[i].growCounter = 0x00; // обнуляем счетчик шансов на прогрев (перегрелись, куда еще :)
          // минуточку, может быть температура уже падает? зачем еще сильнее прикрывать клапан?
          if(heating[i].oldTemperature[0x02] > heating[i].oldTemperature[0x01] && heating[i].oldTemperature[0x01] > heating[i].realTemperature){ // уже падает
            // если падает и мы недалеко от целевой температуры - лучше немного подождать, чем вызвать резкое неконтролируемое падение
            if((heating[i].realTemperature - (heating[i].temperature + settings.heatGistTemperature)) < 0x04 ){ heating[i].jobDelay = heating[i].jobDelay;} // просто ждём 
            // хотя температура и падает, но у нас сильный перегрев (на 10 градусов) - нужно прикрыть клапан
            if((heating[i].oldTemperature[0x02] - (heating[i].temperature + settings.heatGistTemperature)) > 100 &&
                (heating[i].oldTemperature[0x01] -(heating[i].temperature + settings.heatGistTemperature)) > 100 &&
                (heating[i].realTemperature - (heating[i].temperature + settings.heatGistTemperature)) > 100 ){
              // горячее на 10 градусов уже 4 минуты
              // нужно дважды уменьшить ширину импульса
              if(heating[i].jobDelay < settings.heatStepDelay * 0x03){ heating[i].jobDelay = settings.heatStepDelay;}else{ heating[i].jobDelay -= (0x02 * settings.heatStepDelay);}
            }
          }else{ // температура не падает, уменьшаем ширину импульса на термопривод
            if(heating[i].jobDelay < settings.heatStepDelay * 0x02){ heating[i].jobDelay = settings.heatStepDelay; }else{ heating[i].jobDelay -= settings.heatStepDelay; }
            heating[i].freezeCounter = settings.heatJobInterval;
          }
        }else{ // температура в пределах гистерезиса
          heating[i].direction = 0x00; // никуда не дергаемся, тепло, хорошо :)
          heating[i].growTimer = 0x00; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
          heating[i].hotAlarm = 0x00; // обнуляем счетчик аварийных охлаждений
          heating[i].growCounter = 0x00; // обнуляем счетчик шансов на прогрев (вышли на режим, всё норм)
          heating[i].coldAlarm = 0x00; // обнуляем счетчик попыток замедлить падение температуры
        }
      }
    }
    // 
    if(timerHeating > (settings.heatJobInterval - heating[i].jobDelay)){ heating[i].job = true;
      switch(i){
        case 0x00: if(settings.heatOnOff & 0x01) R1_OPEN;
      break;
        case 0x01: if(settings.heatOnOff & 0x02) R2_OPEN;
      break;
        case 0x02: if(settings.heatOnOff & 0x04) R3_OPEN;
      break;
        case 0x03: if(settings.heatOnOff & 0x08) R4_OPEN;
      break;
        case 0x04: if(settings.heatOnOff & 0x10) R5_OPEN;
      break;
      }
    }else{ heating[i].job = false;
      switch(i){
        case 0x00: R1_CLOSED;
      break;
        case 0x01: R2_CLOSED;
      break;
        case 0x02: R3_CLOSED;
      break;
        case 0x03: R4_CLOSED;
      break;
        case 0x04: R5_CLOSED;
      break;
      }
    }
  }
  if(0x00 == timerHeating) timerHeating = settings.heatJobInterval;
  timerHeating--;
  timerSensor--;
}

void HeatingInit(void){
  R1_CLOSED;
  R2_CLOSED;
  R3_CLOSED;
  R4_CLOSED;
  R5_CLOSED;
  GPIOE->CRL &= ~(GPIO_CRL_MODE1 |GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
  GPIOE->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5);
  GPIOE->CRL |=  (GPIO_CRL_MODE1 |GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
  GPIOE->CRL |=  (GPIO_CRL_CNF1_0 | GPIO_CRL_CNF2_0 | GPIO_CRL_CNF3_0 | GPIO_CRL_CNF4_0 | GPIO_CRL_CNF5_0);
  
  #if defined DEBUG_HEATING
    printf("< OK >    Initialization HEATING\r\n");
  #endif
}
