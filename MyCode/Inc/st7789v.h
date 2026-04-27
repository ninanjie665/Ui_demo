#include "main.h"
#include "lvgl.h"

#define LCD_WIDTH   320
#define LCD_HEIGHT  240

void LCD_Init(void);
void LCD_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Flush_DMA(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Clear(uint16_t color);
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color);