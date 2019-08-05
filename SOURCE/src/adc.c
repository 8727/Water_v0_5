#include "adc.h"

struct AdcInitTypeDef adc;

void ADCAlarm(void){
  if(settings.sensorOnOff & 0x01){
    if(adc.sensor1 > settings.alarmSensor1){
      settings.onAlarm |= 0x01;
      BlockWater();
    }
  }
  if(settings.sensorOnOff & 0x02){
    if(adc.sensor2 > settings.alarmSensor2){
      settings.onAlarm |= 0x02;
      BlockWater();
    }
  }
  if(settings.sensorOnOff & 0x04){
    if(adc.sensor3 > settings.alarmSensor3){
      settings.onAlarm |= 0x03;
      BlockWater();
    }
  }
  if(settings.sensorOnOff & 0x08){
    if(adc.sensor4 > settings.alarmSensor4){
      settings.onAlarm |= 0x04;
      BlockWater();
    }
  }
}

void ADC1_2_IRQHandler(void){
  if(ADC1->SR & ADC_SR_JEOC){
    ADC1->SR &= ~(ADC_SR_JEOC | ADC_SR_EOC);
    adc.adc = ((ADC1->JDR1 + ADC1->JDR2 + ADC1->JDR3 + ADC1->JDR4) >> 0x02);
    ADC1->JSQR &= ~(ADC_JSQR_JSQ1 | ADC_JSQR_JSQ2 | ADC_JSQR_JSQ3 | ADC_JSQR_JSQ4); // reset ch
    switch(adc.ch){
      case 0x00:
        adc.sensor1 = (adc.adc >>0x04) + settings.calibSensor1;
        #if defined INFO
          printf("\tSensor1:        %0d\r\n", adc.sensor1);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_0 | ADC_JSQR_JSQ2_0 | ADC_JSQR_JSQ3_0 |ADC_JSQR_JSQ4_0; // A 1
      break;
      case 0x01:
        adc.sensor2 = (adc.adc >>0x04) + settings.calibSensor2;
        #if defined INFO
          printf("\tSensor2:        %0d\r\n", adc.sensor2);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_1 | ADC_JSQR_JSQ2_1 | ADC_JSQR_JSQ3_1 |ADC_JSQR_JSQ4_1; // A 2
      break;
      case 0x02:
        adc.sensor3 = (adc.adc >>0x04) + settings.calibSensor3;
        #if defined INFO
          printf("\tSensor3:        %0d\r\n", adc.sensor3);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_0 | ADC_JSQR_JSQ2_0 | ADC_JSQR_JSQ3_0 |ADC_JSQR_JSQ4_0;
        ADC1->JSQR |= ADC_JSQR_JSQ1_1 | ADC_JSQR_JSQ2_1 | ADC_JSQR_JSQ3_1 |ADC_JSQR_JSQ4_1; // A 3
      break;
      case 0x03:
        adc.sensor4 = (adc.adc >>0x04) + settings.calibSensor4;
        #if defined INFO
          printf("\tSensor4:        %0d\r\n", adc.sensor4);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_1 | ADC_JSQR_JSQ2_1 | ADC_JSQR_JSQ3_1 |ADC_JSQR_JSQ4_1;
        ADC1->JSQR |= ADC_JSQR_JSQ1_3 | ADC_JSQR_JSQ2_3 | ADC_JSQR_JSQ3_3 |ADC_JSQR_JSQ4_3; // C 0
      break;
      case 0x04:
        adc.pressure1 = adc.adc + settings.calibPressure1;
        #if defined INFO
          printf("\tPressure1:      %0d\r\n", adc.pressure1);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_0 | ADC_JSQR_JSQ2_0 | ADC_JSQR_JSQ3_0 |ADC_JSQR_JSQ4_0;
        ADC1->JSQR |= ADC_JSQR_JSQ1_1 | ADC_JSQR_JSQ2_1 | ADC_JSQR_JSQ3_1 |ADC_JSQR_JSQ4_1;
        ADC1->JSQR |= ADC_JSQR_JSQ1_3 | ADC_JSQR_JSQ2_3 | ADC_JSQR_JSQ3_3 |ADC_JSQR_JSQ4_3; // C 1
      break;
      case 0x05:
        adc.pressure2 = adc.adc + settings.calibPressure2;
        #if defined INFO
          printf("\tPressure2:      %0d\r\n", adc.pressure2);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_2 | ADC_JSQR_JSQ2_2 | ADC_JSQR_JSQ3_2 |ADC_JSQR_JSQ4_2;
        ADC1->JSQR |= ADC_JSQR_JSQ1_3 | ADC_JSQR_JSQ2_3 | ADC_JSQR_JSQ3_3 |ADC_JSQR_JSQ4_3; // C 2
      break;
      case 0x06:
        adc.pressure3 = adc.adc + settings.calibPressure3;
        #if defined INFO
          printf("\tPressure3:      %0d\r\n", adc.pressure3);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_0 | ADC_JSQR_JSQ2_0 | ADC_JSQR_JSQ3_0 |ADC_JSQR_JSQ4_0;
        ADC1->JSQR |= ADC_JSQR_JSQ1_2 | ADC_JSQR_JSQ2_2 | ADC_JSQR_JSQ3_2 |ADC_JSQR_JSQ4_2;
        ADC1->JSQR |= ADC_JSQR_JSQ1_3 | ADC_JSQR_JSQ2_3 | ADC_JSQR_JSQ3_3 |ADC_JSQR_JSQ4_3; // C 3
      break;
      case 0x07:
        adc.pressure4 = adc.adc + settings.calibPressure4;
        #if defined INFO
          printf("\tPressure4:      %0d\r\n", adc.pressure4);
        #endif
        ADC1->JSQR |= ADC_JSQR_JSQ1_1 | ADC_JSQR_JSQ2_1 | ADC_JSQR_JSQ3_1 |ADC_JSQR_JSQ4_1;
        ADC1->JSQR |= ADC_JSQR_JSQ1_2 | ADC_JSQR_JSQ2_2 | ADC_JSQR_JSQ3_2 |ADC_JSQR_JSQ4_2;
        ADC1->JSQR |= ADC_JSQR_JSQ1_3 | ADC_JSQR_JSQ2_3 | ADC_JSQR_JSQ3_3 |ADC_JSQR_JSQ4_3; // C 4
      break;
      case 0x08:
        adc.powerV = adc.adc + settings.calibPowerV;
        #if defined INFO
          printf("\tPower 12:       %0d\r\n", adc.powerV);
        #endif
        ADC1->JSQR &= ~(ADC_JSQR_JSQ1 | ADC_JSQR_JSQ2 | ADC_JSQR_JSQ3 | ADC_JSQR_JSQ4);     // A 0
      break;
      default:
        //eroor
      break;
    }
    adc.ch++;
    if(0x09 < adc.ch) adc.ch = 0x00;
//    ADC1->CR2 |= ADC_CR2_JSWSTART;
  }
}

void ADCInit(void){
  // Pressure
  GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3);
  GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3);  
  // Water in12v
  GPIOC->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4);
  GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4);
  
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    //подаем тактирование АЦП
  RCC->CFGR    &= ~RCC_CFGR_ADCPRE;      //входной делитель
  ADC1->CR1 = 0x00;                      //предочистка регистра
  ADC1->CR2 |= ADC_CR2_CAL;              //запуск калибровки 
  while (!(ADC1->CR2 & ADC_CR2_CAL)){};  //ждем окончания калибровки
  ADC1->CR2 = ADC_CR2_JEXTSEL;           //выбрать источником запуска разряд  JSWSTART
  ADC1->CR2 |= ADC_CR2_JEXTTRIG;         //разр. внешний запуск инжектированной группы
  ADC1->CR1 |= ADC_CR1_SCAN;             //режим сканирования (т.е. несколько каналов)
  ADC1->JSQR = ADC_JSQR_JL;              //задаем количество каналов в инжектированной группе
  ADC1->JSQR &= ~(ADC_JSQR_JSQ1 | ADC_JSQR_JSQ2 | ADC_JSQR_JSQ3 | ADC_JSQR_JSQ4); // A 0
  ADC1->CR2 |= ADC_CR2_ADON;             //включить АЦП
  adc.ch = 0x00;
  ADC1->CR1 |= ADC_CR1_JEOCIE;
  
  NVIC_SetPriority(ADC1_2_IRQn, PRIORITY_ADC);
  NVIC_EnableIRQ(ADC1_2_IRQn);
  
  ADC1->CR2 |= ADC_CR2_JSWSTART;        //запустить процес преобразования
  
  #if defined DEBUG_ADC
    printf("< OK >    Initialization SENSORS\r\n");
  #endif
}
