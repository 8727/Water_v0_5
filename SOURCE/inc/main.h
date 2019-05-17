#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"


/* Define --------------------------------------------------------------------*/
#define BUILD_YEAR (__DATE__[7] == '?' ? 1900 : (((__DATE__[7] - '0') * 1000 ) + (__DATE__[8] - '0') * 100 + (__DATE__[9] - '0') * 10 + __DATE__[10] - '0'))
#define BUILD_MONTH (__DATE__ [2] == '?' ? 1 : __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) : __DATE__ [2] == 'b' ? 2 : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
        : __DATE__ [2] == 'y' ? 5 : __DATE__ [2] == 'l' ? 7 : __DATE__ [2] == 'g' ? 8 : __DATE__ [2] == 'p' ? 9 : __DATE__ [2] == 't' ? 10 : __DATE__ [2] == 'v' ? 11 : 12)
#define BUILD_DAY (__DATE__[4] == '?' ? 1 : ((__DATE__[4] == ' ' ? 0 : ((__DATE__[4] - '0') * 10)) + __DATE__[5] - '0'))
#define BUILD_TIME_H (__TIME__[0] == '?' ? 1 : ((__TIME__[0] == ' ' ? 0 : ((__TIME__[0] - '0') * 10)) + __TIME__[1] - '0'))
#define BUILD_TIME_M (__TIME__[3] == '?' ? 1 : ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0'))
#define BUILD_TIME_S (__TIME__[6] == '?' ? 1 : ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0'))

#define BLACK         0x0000      /*   0,   0,   0 */
#define NAVY          0x000F      /*   0,   0, 128 */
#define DARKGREEN     0x03E0      /*   0, 128,   0 */
#define DARKCYAN      0x03EF      /*   0, 128, 128 */
#define MAROON        0x7800      /* 128,   0,   0 */
#define PURPLE        0x780F      /* 128,   0, 128 */
#define OLIVE         0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY     0xC618      /* 192, 192, 192 */
#define DARKGREY      0x7BEF      /* 128, 128, 128 */
#define BLUE          0x001F      /*   0,   0, 255 */
#define GREEN         0x07E0      /*   0, 255,   0 */
#define CYAN          0x07FF      /*   0, 255, 255 */
#define RED           0xF800      /* 255,   0,   0 */
#define MAGENTA       0xF81F      /* 255,   0, 255 */
#define YELLOW        0xFFE0      /* 255, 255,   0 */
#define WHITE         0xFFFF      /* 255, 255, 255 */
#define ORANGE        0xFD20      /* 255, 165,   0 */
#define GREENYELLOW   0xAFE5      /* 173, 255,  47 */




#endif /* __MAIN_H */
