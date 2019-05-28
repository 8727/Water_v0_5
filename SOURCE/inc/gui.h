#ifndef _GUI_H
#define _GUI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"
//#include "calib.h"

/* Define --------------------------------------------------------------------*/

static const uint8_t brOn [] = {0xC7, 0xBB, 0x6D, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45, 0x6D, 0xBB, 0xC7};
static const uint8_t brOff [] = {0xC7, 0xBB, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0xBB, 0xC7};

struct TableInitTypeDef{
  uint16_t address;
  uint16_t width;
  uint16_t height;
  uint8_t  type;
  uint8_t  pictures;
  uint8_t  fps;
  uint8_t  a;
  uint8_t  b;
  uint8_t  c;
};

struct GuiInitTypeDef{
  uint8_t  step;
};

extern struct TableInitTypeDef img[0x0100];
extern struct GuiInitTypeDef gui;

void GuiCalibDraw(uint16_t x, uint16_t y);
void GuiCalibErase(uint16_t x, uint16_t y);
void GuiSetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void GuiFullWindow(uint16_t color);
void GuiEraseW25qxx(void);
void GuiEraseBlocks(uint8_t block);
void GuiBr(void);

void GuiInit(void);

#endif /* _GUI_H */
