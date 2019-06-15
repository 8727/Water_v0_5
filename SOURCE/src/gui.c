#include "gui.h"

struct TableInitTypeDef img[0x0100];
struct GuiInitTypeDef gui;
static uint16_t buffDMA = 0x0000;

void GuiSetWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
  if(settings.maxX < width) width = settings.maxX;
  if(settings.maxY < height) height = settings.maxY;
  if(settings.maxX < (x + width)) x = settings.maxX - width;
  if(settings.maxY <(y + height)) y = settings.maxY - height;
  
  LCD_REG = 0x2A;
  LCD_DATA = x >> 0x08;
  LCD_DATA = x;
  LCD_DATA = (x + width - 0x01) >> 0x08;
  LCD_DATA = x + width - 0x01;
  LCD_REG = 0x2B;
  LCD_DATA = y >> 0x08;
  LCD_DATA = y;
  LCD_DATA = (y + height - 0x01) >> 0x08;
  LCD_DATA = y + height - 0x01;
  LCD_REG = 0x2C;
}

void GuiCalibDraw(uint16_t x, uint16_t y){
  uint32_t c;
  GuiSetWindow(x - 0x0F, y - 0x0F, 0x20, 0x20);
  for(uint8_t i = 0x00; i < 0x20; i++){
//    c = calibDraw[i];
    for(uint8_t z = 0x00; z < 0x20; z++){
      if(c & 0x80000000){
        LCD_DATA = BLACK;
      }else{
        LCD_DATA = WHITE;
      }
      c <<= 0x01;
    }
  }
}

void GuiCalibErase(uint16_t x, uint16_t y){
  GuiSetWindow(x - 0x0F, y - 0x0F, 0x20, 0x20);
  uint16_t i = 0x0400;
  while(i--){
    LCD_DATA = WHITE;
  }
}

void GuiFullWindow(uint16_t color){
  uint32_t i = 0x025800;
  GuiSetWindow(0x00, 0x00, settings.maxX, settings.maxY);
  while(i--){
    LCD_DATA = color;
  }
}

void GuiEraseW25qxx(void){
  uint16_t i = (w25qxx.blocks + 0x08) * 0x28;
  GuiFullWindow(BLACK);
  GuiSetWindow(((settings.maxX - w25qxx.blocks ) / 0x02) - 0x04, (settings.maxY / 0x02) - 0x14, w25qxx.blocks + 0x08, 0x28);
    while(i--){
    LCD_DATA = WHITE;
  }
}

void GuiEraseBlocks(uint8_t block){
  uint8_t i = 0x32;
  GuiSetWindow(((settings.maxX - w25qxx.blocks) / 0x02) + block, (settings.maxY / 0x02) - 0x10, 0x01, 0x20);
  while(i--){
    LCD_DATA = RED;
  }
}

void GuiBr(void){
//  uint8_t c;
//  uint8_t br = 249;
//  for(uint8_t x = 0x00; x < 0x32; x++){
//    GuiSetWindow(0x0F + x * 0x09, 0x04, 0x08, 0x10);
//    for(uint8_t i = 0x00; i < 0x10; i++){
//      if((x * 5.1) < br){
//        c = brOn[i];
//      }else{
//        c = brOff[i];
//      }
//      for(uint8_t z = 0x00; z < 0x08; z++){
//        if(c & 0x80){
//          LCD_DATA = BLACK;
//        }else{
//          LCD_DATA = WHITE;
//        }
//        c <<= 0x01;
//      }
//    }
//  }
}









void GuiInit(void){
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  
  DMA1_Channel2->CCR = 0x00;
  DMA1_Channel2->CPAR = (uint32_t)&SPI1->DR;
  DMA1_Channel2->CMAR = 0x60020000;
  DMA1_Channel2->CNDTR = 0x00;
  DMA1_Channel2->CCR = DMA_CCR2_PL | DMA_CCR2_MSIZE_0 | DMA_CCR2_PSIZE_0 | DMA_CCR2_TCIE;
  
  DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
  DMA1_Channel3->CMAR = (uint32_t)&buffDMA;
  DMA1_Channel3->CNDTR = 0x00;
  DMA1_Channel3->CCR |= DMA_CCR3_PL | DMA_CCR3_MSIZE_0 | DMA_CCR3_PSIZE_0 | DMA_CCR3_DIR;
  
  NVIC_SetPriority(DMA1_Channel2_IRQn, PRIORITY_GUI_UPDATE);
  NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  
  GuiLoadImg(0x00, 0x00, 0x00);
}

void DMA1_Channel2_IRQHandler(void){
  DMA1->IFCR |= DMA_IFCR_CGIF2;
  DMA1_Channel2->CCR &= ~DMA_CCR2_EN;
  DMA1_Channel3->CCR &= ~DMA_CCR3_EN;
  if(0x00 < gui.step){
    gui.step--;
    DMA1_Channel2->CNDTR = 0xFFFF;
    DMA1_Channel3->CNDTR = 0xFFFF;
    DMA1_Channel2->CCR |= DMA_CCR2_EN;
    DMA1_Channel3->CCR |= DMA_CCR3_EN;
  }else{
    SPI1->CR1 &= ~SPI_CR1_DFF;
    SPI1->CR2 &= ~(SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN);
    W25Qxx_CS_HIGHT;
  }
}

void GuiLoadImg(uint16_t x, uint16_t y, uint8_t numb){
  uint32_t pixel = img[numb].width * img[numb].height;
  GuiSetWindow(x, y, img[numb].width, img[numb].height);
  if(0x025800 < pixel) pixel =  0x025800;
  if(0xFFFF < pixel){
    gui.step = pixel / 0xFFFF;
    DMA1_Channel2->CNDTR = pixel - gui.step * 0xFFFF;
    DMA1_Channel3->CNDTR = pixel - gui.step * 0xFFFF;
  }else{
    DMA1_Channel2->CNDTR = pixel;
    DMA1_Channel3->CNDTR = pixel;
  }
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_FAST_READ);
  W25QxxWriteRead(img[numb].address >> 0x08);
  W25QxxWriteRead(img[numb].address & 0x00FF);
  W25QxxWriteRead(0x00);
  W25QxxWriteRead(0x00);
  SPI1->CR1 |= SPI_CR1_DFF;
  SPI1->CR2 |= SPI_CR2_RXDMAEN | SPI_CR2_TXDMAEN;
  DMA1_Channel2->CCR |= DMA_CCR2_EN;
  DMA1_Channel3->CCR |= DMA_CCR3_EN;
}
