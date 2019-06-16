#include "heating.h"

struct heatingInitTypeDef heating;
struct heatingValveInitTypeDef heatingValve[HEATING_MAX_DEVICES];

void HeatingPWM(void){ // 10Hz
  uint8_t i;
  for(i = 0x00; i < HEATING_MAX_DEVICES; i++){
    if(heating.counter > heating.interval){
      heating.counter = 0x00;
      heatingValve[i].oldTemperature[0x02] = heatingValve[i].oldTemperature[0x01];
      heatingValve[i].oldTemperature[0x01] = heatingValve[i].oldTemperature[0x00];
      heatingValve[i].oldTemperature[0x00] = heatingValve[i].realTemperature;
    }
    // управляем воздействием в зависимости от текущей температуры
    if(heatingValve[i].freezeCounter > 0x00){ // между точками воздействия реагируем на аварийные ситуации 
     heatingValve[i].freezeCounter--;
     // вдруг мы пытаемся нагревать, а температура превысила уставку?
     // нужно срочно остудить на одну ступень, а не ждать кучу времени ($freezeCounter) пока перегреется
     // но только один раз
      if(heatingValve[i].realTemperature > heatingValve[i].temperature + heating.gistTemperature &&
          heatingValve[i].direction == 0x01 && heatingValve[i].hotAlarm == 0x00){
        heatingValve[i].direction = -0x01;
        heatingValve[i].growCounter = 0x00; // обнуляем счетчик шансов на прогрев (перегрелись, куда еще :)
        heatingValve[i].hotAlarm++;
        heatingValve[i].delay -= heating.stepDelay;  
      }
      // резкое падение нужно один раз замедлить
      if(heatingValve[i].coldAlarm < 0x01 && heatingValve[i].oldTemperature[0x02] > heatingValve[i].oldTemperature[0x01] &&
          heatingValve[i].oldTemperature[0x01] > heatingValve[i].realTemperature &&
          heatingValve[i].oldTemperature[0x02] - heatingValve[i].realTemperature > 0x0F){ // 1.5C
        heatingValve[i].delay += heating.stepDelay;  
        heatingValve[i].coldAlarm++;
      } 
    }else{
      // противоаварийный сброс настроек, если температура выше критической 4 минуты подряд
      if(heatingValve[i].realTemperature > heating.topTemperature && 
          heatingValve[i].oldTemperature[0x01] > heating.topTemperature && 
          heatingValve[i].oldTemperature[0x02] > heating.topTemperature){ heatingValve[i].delay = heating.valveDefDelay;}
          
      // если температура выше критической и delay на максимуме - сбрасываем настройки, что-то резко изменилось
      if(heatingValve[i].realTemperature > heating.topTemperature &&
          heatingValve[i].delay >= (heating.maxDelay - heating.stepDelay)){ heatingValve[i].delay = heating.valveDefDelay;}
          
      // а если температура = установке $temperature, тогда вообще ничего делать не нужно
      // тогда остановим $freezeCounter, пока температура не выйдет за пределы
      if(heatingValve[i].realTemperature < (heatingValve[i].temperature - heating.gistTemperature)){ // если температура ниже заданной
        heatingValve[i].growTimer++;
        
        // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
        // если (таймер > 10 x interval && delay < valveDefDelay) тогда delay = valveDefDelay  
        // для случаев когда после сильного падения delay (например, до нуля) замучаешься ждать роста
        // ведь с шагом 0.25 необходимо 12 / 0.25 * 2мин = 96 минут
        if(heatingValve[i].growTimer > 10 * heating.interval && heatingValve[i].delay < heating.valveDefDelay){
          heatingValve[i].delay = heating.valveDefDelay;
        }
        // нужно сильнее открывать клапан (увеличить delay)
        heatingValve[i].coldAlarm = 0x00; // обнуляем счетчик попыток замедлить падение температуры
        heatingValve[i].direction = 0x01; 
        // минуточку, может быть температура уже растёт? зачем еще сильнее открывать клапан?
        if(heatingValve[i].oldTemperature[0x02] < heatingValve[i].oldTemperature[0x01] && heatingValve[i].oldTemperature[0x01] < heatingValve[i].realTemperature){
        if(heatingValve[i].growCounter < 0x04){
          heatingValve[i].growCounter++; // температура и так растет. даём шанс
        }else{
          if(heatingValve[i].realTemperature - heatingValve[i].oldTemperature[0x02] >= 0x0F){ //  шансы на рост уже исчерпаны. но вдруг температура уже растёт хорошими темпами?
          // растёт хорошо! 1.5 градуса за 4 минуты, будем ждать сколько потребуется!
          }else{ heatingValve[i].delay += heating.stepDelay;}// рост слишком медленный, увеличиваем ширину импульса на термопривод
        }
       }else{
        heatingValve[i].delay += heating.stepDelay;
        // если до уставки осталось более 3 градусов - ускоряем разогрев (увеличим delay еще на ступень stepDelay)
        if((heatingValve[i].temperature - heating.gistTemperature) - heatingValve[i].realTemperature > 30 ) heatingValve[i].delay += heating.stepDelay;
       }
      if(heatingValve[i].delay > heating.maxDelay) heatingValve[i].delay = heating.maxDelay; // не позволяем подавать питания на термопривод импульсами дольше чем maxDelay

      heatingValve[i].freezeCounter = heating.interval * 0x02; // термопривод долго реагирует, не торопимся
      }else{
        if(heatingValve[i].realTemperature > (heatingValve[i].temperature + heating.gistTemperature)){ // нужно слабее открывать клапан (увеличить delay)
          heatingValve[i].direction -= 0x01;
          heatingValve[i].growTimer = 0x00; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
          heatingValve[i].hotAlarm = 0x00; // обнуляем счетчик аварийных охлаждений
          heatingValve[i].growCounter = 0x00; // обнуляем счетчик шансов на прогрев (перегрелись, куда еще :)
          // минуточку, может быть температура уже падает? зачем еще сильнее прикрывать клапан?
          if(heatingValve[i].oldTemperature[0x02] > heatingValve[i].oldTemperature[0x01] && heatingValve[i].oldTemperature[0x01] > heatingValve[i].realTemperature){ // уже падает
            // если падает и мы недалеко от целевой температуры - лучше немного подождать, чем вызвать резкое неконтролируемое падение
            if((heatingValve[i].realTemperature - (heatingValve[i].temperature + heating.gistTemperature)) < 0x04 ){ heatingValve[i].delay = heatingValve[i].delay;} // просто ждём 
            // хотя температура и падает, но у нас сильный перегрев (на 10 градусов) - нужно прикрыть клапан
            if((heatingValve[i].oldTemperature[0x02] - (heatingValve[i].temperature + heating.gistTemperature)) > 100 &&
                (heatingValve[i].oldTemperature[0x01] -(heatingValve[i].temperature + heating.gistTemperature)) > 100 &&
                (heatingValve[i].realTemperature - (heatingValve[i].temperature + heating.gistTemperature)) > 100 ){
              // горячее на 10 градусов уже 4 минуты
              // нужно дважды уменьшить ширину импульса
              if(heatingValve[i].delay < heating.stepDelay * 0x03){ heatingValve[i].delay = heating.stepDelay;}else{ heatingValve[i].delay -= (0x02 * heating.stepDelay);}
            }
          }else{ // температура не падает, уменьшаем ширину импульса на термопривод
            if(heatingValve[i].delay < heating.stepDelay * 0x02){ heatingValve[i].delay = heating.stepDelay; }else{ heatingValve[i].delay -= heating.stepDelay; }
            heatingValve[i].freezeCounter = heating.interval;
          }
        }else{ // температура в пределах гистерезиса
          heatingValve[i].direction = 0x00; // никуда не дергаемся, тепло, хорошо :)
          heatingValve[i].growTimer = 0x00; // таймер "сколько секунд подряд нужен рост". запускаем как только температура ниже уставки. обнуляем как дойдет до уставки или выше.
          heatingValve[i].hotAlarm = 0x00; // обнуляем счетчик аварийных охлаждений
          heatingValve[i].growCounter = 0x00; // обнуляем счетчик шансов на прогрев (вышли на режим, всё норм)
          heatingValve[i].coldAlarm = 0x00; // обнуляем счетчик попыток замедлить падение температуры
        }
      }
    }
    // 
    if(heating.counter > heatingValve[i].delay){
      heatingValve[i].status = 0x00;
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
    }else{
      heatingValve[i].status = 0x01;
      switch(i){
      case 0x00: R1_OPEN;
      break;
      case 0x01: R2_OPEN;
      break;
      case 0x02: R3_OPEN;
      break;
      case 0x03: R4_OPEN;
      break;
      case 0x04: R5_OPEN;
      break;
      }
    }
  }
  heating.valveCounter++;
  if(heating.valveCounter > heating.valveInterval) heating.valveCounter = 0x00;
  heating.counter++;
}

void TIM6_IRQHandler(void){
  TIM6->SR &= ~TIM_SR_UIF;
  HeatingPWM();
}

void HeatingInit(void){
  GPIOE->CRL &= ~(GPIO_CRL_MODE1 |GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
  GPIOE->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5);
  GPIOE->CRL |=  (GPIO_CRL_MODE1 |GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
  GPIOE->CRL |=  (GPIO_CRL_CNF1_0 | GPIO_CRL_CNF2_0 | GPIO_CRL_CNF3_0 | GPIO_CRL_CNF4_0 | GPIO_CRL_CNF5_0);
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  TIM6->PSC = 0x1F3F; // 7999 80000000:8000=10000Hz
  TIM6->ARR = 0x03E7; // 10Hz
  TIM6->SR = 0x00;
  TIM6->DIER |= TIM_DIER_UIE;
  TIM6->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;
  
  NVIC_SetPriority(TIM6_IRQn, PRIORITY_HEATING);
  NVIC_EnableIRQ(TIM6_IRQn);
}
