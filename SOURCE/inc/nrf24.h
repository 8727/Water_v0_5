#ifndef _NRF24_H
#define _NRF24_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/
#define NRF24_CE_LOW                  GPIOB->BSRR = GPIO_BSRR_BR11
#define NRF24_CE_HIGHT                GPIOB->BSRR = GPIO_BSRR_BS11

#define NRF24_CS_LOW                  GPIOB->BSRR = GPIO_BSRR_BR12
#define NRF24_CS_HIGHT                GPIOB->BSRR = GPIO_BSRR_BS12

/* Команды */
#define NRF24_R_REGISTER              0x00 // + n Прочитать регистр n
#define NRF24_W_REGISTER              0x20 // + n Записать регистр n
#define NRF24_R_RX_PAYLOAD            0x61 // Принять данные данные из верхнего слота очереди приёмника. 
#define NRF24_W_TX_PAYLOAD            0xA0 // Записать в очередь передатчика данные для отправки
#define NRF24_FLUSH_TX                0xE1 // Сбросить очередь передатчика
#define NRF24_FLUSH_RX                0xE2 // Сбросить очередь приёмника
#define NRF24_REUSE_TX_PL             0xE3 // Использовать повторно последний переданный пакет
#define NRF24_R_RX_PL_WID             0x60 // Прочитать размер данных принятого пакета в начале очереди приёмника. 
#define NRF24_W_ACK_PAYLOAD           0xA8 // + p Записать данные для отправки с пакетом подтверждения по каналу p. 
#define NRF24_W_TX_PAYLOAD_NOACK      0xB0 // Записать в очередь передатчика данные, для отправки без подтверждения
#define NRF24_NOP                     0xFF // Нет операции. Может быть использовано для чтения регистра статуса

/* Регистры */
#define NRF24_CONFIG                  0x00 // Регистр настроек
#define NRF24_EN_AA                   0x01 // Выбор автоподтверждения
#define NRF24_EN_RXADDR               0x02 // Выбор каналов приёмника
#define NRF24_SETUP_AW                0x03 // Настройка размера адреса
#define NRF24_SETUP_RETR              0x04 // Настройка повторной отправки
#define NRF24_CH                      0x05 // Номер радиоканала, на котором осуществляется работа. От 0 до 125. 
#define NRF24_SETUP                   0x06 // Настройка радиоканала
#define NRF24_STATUS                  0x07 // Регистр статуса. 
#define NRF24_OBSERVE_TX              0x08 // Количество повторов передачи и потерянных пакетов
#define NRF24_RPD                     0x09 // Мощность принимаемого сигнала. Если младший бит = 1, то уровень более -64dBm 
#define NRF24_RX_ADDR_P0              0x0A // 3-5 байт (начиная с младшего байта). Адрес канала 0 приёмника. 
#define NRF24_RX_ADDR_P1              0x0B // 3-5 байт (начиная с младшего байта). Адрес канала 1 приёмника.
#define NRF24_RX_ADDR_P2              0x0C // Младший байт адреса канала 2 приёмника. Старшие байты из RX_ADDR_P1
#define NRF24_RX_ADDR_P3              0x0D // Младший байт адреса канала 3 приёмника. Старшие байты из RX_ADDR_P1
#define NRF24_RX_ADDR_P4              0x0E // Младший байт адреса канала 4 приёмника. Старшие байты из RX_ADDR_P1
#define NRF24_RX_ADDR_P5              0x0F // Младший байт адреса канала 5 приёмника. Старшие байты из RX_ADDR_P1
#define NRF24_TX_ADDR                 0x10 // 3-5 байт (начиная с младшего байта). Адрес удалённого устройства для передачи
#define NRF24_RX_PW_P0                0x11 // Размер данных при приёме по каналу 0: от 1 до 32. 0 - канал не используется.
#define NRF24_RX_PW_P1                0x12 // Размер данных при приёме по каналу 1: от 1 до 32. 0 - канал не используется.
#define NRF24_RX_PW_P2                0x13 // Размер данных при приёме по каналу 2: от 1 до 32. 0 - канал не используется.
#define NRF24_RX_PW_P3                0x14 // Размер данных при приёме по каналу 3: от 1 до 32. 0 - канал не используется.
#define NRF24_RX_PW_P4                0x15 // Размер данных при приёме по каналу 4: от 1 до 32. 0 - канал не используется.
#define NRF24_RX_PW_P5                0x16 // Размер данных при приёме по каналу 5: от 1 до 32. 0 - канал не используется.
#define NRF24_FIFO_STATUS             0x17 // Состояние очередей FIFO приёмника и передатчика
#define NRF24_DYNPD                   0x1C // Выбор каналов приёмника для которых используется произвольная длина пакетов.
#define NRF24_FEATURE                 0x1D // Регистр опций

/* Биты регистров */
// CONFIG
#define NRF24_MASK_RX_DR              0x40 // Запрещает прерывание по RX_DR (получение пакета)
#define NRF24_MASK_TX_DS              0x20 // Запрещает прерывание по TX_DS (завершение отправки пакета) 
#define NRF24_MASK_MAX_RT             0x10 // Запрещает прерывание по MAX_RT (превышение числа повторных попыток отправки) 
#define NRF24_EN_CRC                  0x08 // Включает CRC
#define NRF24_CRCO                    0x04 // Размер поля CRC: 0 - 1 байт; 1 - 2 байта
#define NRF24_PWR_UP                  0x02 // Включение питания
#define NRF24_PRIM_RX                 0x01 // Выбор режима: 0 - PTX (передатчик) 1 - PRX (приёмник)

// EN_AA
#define NRF24_ENAA_P5                 0x20 // Включает автоподтверждение данных, полученных по каналу 5
#define NRF24_ENAA_P4                 0x10 // Включает автоподтверждение данных, полученных по каналу 4
#define NRF24_ENAA_P3                 0x08 // Включает автоподтверждение данных, полученных по каналу 3
#define NRF24_ENAA_P2                 0x04 // Включает автоподтверждение данных, полученных по каналу 2
#define NRF24_ENAA_P1                 0x02 // Включает автоподтверждение данных, полученных по каналу 1
#define NRF24_ENAA_P0                 0x01 // Включает автоподтверждение данных, полученных по каналу 0

// EN_RXADDR
#define NRF24_ERX_P5                  0x20 // Включает канал 5 приёмника
#define NRF24_ERX_P4                  0x10 // Включает канал 4 приёмника 
#define NRF24_ERX_P3                  0x08 // Включает канал 3 приёмника 
#define NRF24_ERX_P2                  0x04 // Включает канал 2 приёмника 
#define NRF24_ERX_P1                  0x02 // Включает канал 1 приёмника 
#define NRF24_ERX_P0                  0x01 // Включает канал 0 приёмника 

// SETUP_AW // Два бита, Выбирает ширину поля адреса: 1 - 3 байта; 2 - 4 байта; 3 - 5 байт.
#define NRF24_SETUP_AW_3BYTES_ADDRESS 0x01
#define NRF24_SETUP_AW_4BYTES_ADDRESS 0x02
#define NRF24_SETUP_AW_5BYTES_ADDRESS 0x03

// SETUP_RETR 
#define NRF24_SETUP_NO_RETRANSMIT     0x00
#define NRF24_SETUP_1_RETRANSMIT      0x01
#define NRF24_SETUP_2_RETRANSMIT      0x02
#define NRF24_SETUP_3_RETRANSMIT      0x03
#define NRF24_SETUP_4_RETRANSMIT      0x04
#define NRF24_SETUP_5_RETRANSMIT      0x05
#define NRF24_SETUP_6_RETRANSMIT      0x06
#define NRF24_SETUP_7_RETRANSMIT      0x07
#define NRF24_SETUP_8_RETRANSMIT      0x08
#define NRF24_SETUP_9_RETRANSMIT      0x09
#define NRF24_SETUP_10_RETRANSMIT     0x0A
#define NRF24_SETUP_11_RETRANSMIT     0x0B
#define NRF24_SETUP_12_RETRANSMIT     0x0C
#define NRF24_SETUP_13_RETRANSMIT     0x0D
#define NRF24_SETUP_14_RETRANSMIT     0x0E
#define NRF24_SETUP_15_RETRANSMIT     0x0F

// STATUS
#define NRF24_RX_DR                   0x40 // Флаг получения новых данных в FIFO приёмника. Для сброса флага нужно записать 1
#define NRF24_TX_DS                   0x20 // Флаг завершения передачи. Для сброса флага нужно записать 1
#define NRF24_MAX_RT                  0x10 // Флаг превышения установленного числа повторов. Без сброса (записать 1) обмен невозможен
#define NRF24_RX_P_NO                 0x0E // FIFO пусто.
#define NRF24_RX_P5                   0x0C // 5 канал, данные для которого доступны в FIFO приёмника.
#define NRF24_RX_P4                   0x0A // 4 канал, данные для которого доступны в FIFO приёмника.
#define NRF24_RX_P3                   0x08 // 3 канал, данные для которого доступны в FIFO приёмника.
#define NRF24_RX_P2                   0x06 // 2 канал, данные для которого доступны в FIFO приёмника.
#define NRF24_RX_P1                   0x04 // 1 канал, данные для которого доступны в FIFO приёмника.
#define NRF24_RX_P0                   0x02 // 0 канал, данные для которого доступны в FIFO приёмника.
#define NRF24_TX_FULL_STATUS          0x01 // Признак заполнения FIFO передатчика: 1 - заполнено; 0 - есть доступные слоты 

// OBSERVE_TX
#define NRF24_PLOS_CNT  4 // 4 бита. Общее количество пакетов без подтверждения. Сбрасывается записью RF_CH
#define NRF24_ARC_CNT   0 // 4 бита. Количество предпринятых повторов при последней отправке. 

// FIFO_STATUS
#define NRF24_TX_REUSE                0x40 // Признак готовности последнего пакета для повторной отправки. 
#define NRF24_TX_FULL_FIFO            0x20 // Флаг переполнения FIFO очереди передатчика.
                                           // (переименовано из TX_FULL во избежание путаницы с одноимённым битом из регистра STATUS)
#define NRF24_TX_EMPTY                0x10 // Флаг освобождения FIFO очереди передатчика.
#define NRF24_RX_FULL                 0x02 // Флаг переполнения FIFO очереди приёмника.
#define NRF24_RX_EMPTY                0x01 // Флаг освобождения FIFO очереди приёмника.

// DYNDP
#define NRF24_DPL_P5                  0x20 // Включает приём пакетов произвольной длины по каналу 5
#define NRF24_DPL_P4                  0x10 // Включает приём пакетов произвольной длины по каналу 4
#define NRF24_DPL_P3                  0x08 // Включает приём пакетов произвольной длины по каналу 3
#define NRF24_DPL_P2                  0x04 // Включает приём пакетов произвольной длины по каналу 2
#define NRF24_DPL_P1                  0x02 // Включает приём пакетов произвольной длины по каналу 1
#define NRF24_DPL_P0                  0x01 // Включает приём пакетов произвольной длины по каналу 0

// FEATURE  
#define NRF24_EN_DPL                  0x04 // Включает поддержку приёма и передачи пакетов произвольной длины
#define NRF24_EN_ACK_PAY              0x02 // Разрешает передачу данных с пакетами подтверждения приёма
#define NRF24_EN_DYN_ACK              0x01 // Разрешает использование W_TX_PAYLOAD_NOACK











uint8_t Nrf24SendData(uint8_t *buff, uint8_t size);
void Nrf24CheckRadio(void);

void Nrf24Init(void);

#endif /* _NRF24_H */
