#include "main.h"
#include "lvgl.h"

#define LCD_WIDTH   320
#define LCD_HEIGHT  240

#define LCD_CS_LOW()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_HIGH()  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_DC_LOW()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_HIGH()  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)
#define LCD_RST_HIGH() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_RST_LOW()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)

void LCD_Init(void);
void LCD_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Flush_DMA(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_Clear(uint16_t color);
void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color);
void LCD_FillTest(uint16_t color);
void LCD_SimpleInit(void);

void SPI_SendByte(uint8_t data);