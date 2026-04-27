#include "st7789v.h"
#include "spi.h"

// ========== 硬件引脚宏（请根据你的实际引脚修改）==========
#define LCD_CS_LOW()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_HIGH()  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_DC_LOW()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_HIGH()  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)
#define LCD_RST_HIGH() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)
#define LCD_RST_LOW()  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET)

static uint8_t dma_busy = 0;
static lv_disp_drv_t *disp_drv_ref = NULL;

// ---------- 基础 SPI 发送 ----------
static void SPI_SendByte(uint8_t data) {
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
}

static void sendCmd(uint8_t cmd) {
    LCD_DC_LOW();
    LCD_CS_LOW();
    SPI_SendByte(cmd);
    LCD_CS_HIGH();
}

static void sendData(uint8_t data) {
    LCD_DC_HIGH();
    LCD_CS_LOW();
    SPI_SendByte(data);
    LCD_CS_HIGH();
}

// ---------- 延时 ----------
void LCD_Delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}

// ---------- 设置窗口（修正版）----------
void LCD_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    sendCmd(0x2A);
    LCD_DC_HIGH(); LCD_CS_LOW();
    SPI_SendByte(x1 >> 8); SPI_SendByte(x1 & 0xFF);
    SPI_SendByte(x2 >> 8); SPI_SendByte(x2 & 0xFF);
    LCD_CS_HIGH();

    sendCmd(0x2B);
    LCD_DC_HIGH(); LCD_CS_LOW();
    SPI_SendByte(y1 >> 8); SPI_SendByte(y1 & 0xFF);
    SPI_SendByte(y2 >> 8); SPI_SendByte(y2 & 0xFF);
    LCD_CS_HIGH();

    sendCmd(0x2C);   // 写显存命令
}

// ---------- 屏幕初始化（横屏 320x240）----------
void LCD_Init(void) {
    LCD_RST_HIGH();
    LCD_Delay_ms(10);
    LCD_RST_LOW();
    LCD_Delay_ms(100);
    LCD_RST_HIGH();
    LCD_Delay_ms(120);

    sendCmd(0x11);   // 退出睡眠
    LCD_Delay_ms(120);

    sendCmd(0x36);
    sendData(0x60);   // 横屏模式（320x240）

    sendCmd(0x3A);
    sendData(0x55);   // 16位色

    // 帧率控制
    sendCmd(0xB2);
    sendData(0x0C); sendData(0x0C); sendData(0x00); sendData(0x33); sendData(0x33);

    sendCmd(0xB7);
    sendData(0x35);
    sendCmd(0xBB);
    sendData(0x19);
    sendCmd(0xC0);
    sendData(0x2C);
    sendCmd(0xC2);
    sendData(0x01);
    sendCmd(0xC3);
    sendData(0x12);
    sendCmd(0xC4);
    sendData(0x20);
    sendCmd(0xC6);
    sendData(0x0F);

    // 正伽马
    sendCmd(0xE0);
    sendData(0xD0); sendData(0x04); sendData(0x0D); sendData(0x11);
    sendData(0x13); sendData(0x2B); sendData(0x3F); sendData(0x54);
    sendData(0x4C); sendData(0x18); sendData(0x0D); sendData(0x0B);
    sendData(0x1F); sendData(0x23);

    // 负伽马
    sendCmd(0xE1);
    sendData(0xD0); sendData(0x04); sendData(0x0C); sendData(0x11);
    sendData(0x13); sendData(0x2C); sendData(0x3F); sendData(0x44);
    sendData(0x51); sendData(0x2F); sendData(0x1F); sendData(0x1F);
    sendData(0x20); sendData(0x23);

    sendCmd(0x21);   // 反转显示
    sendCmd(0x29);   // 开启显示
}

// ---------- 阻塞式填充（备用）----------
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    uint32_t total = (x2 - x1 + 1) * (y2 - y1 + 1);
    LCD_Set_Window(x1, y1, x2, y2);
    LCD_DC_HIGH();
    LCD_CS_LOW();
    for (uint32_t i = 0; i < total; i++) {
        SPI_SendByte(color >> 8);
        SPI_SendByte(color & 0xFF);
    }
    LCD_CS_HIGH();
}

void LCD_Clear(uint16_t color) {
    LCD_Fill(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, color);
}

void LCD_Draw_Point(uint16_t x, uint16_t y, uint16_t color) {
    LCD_Fill(x, y, x, y, color);
}

// ---------- LVGL 专用 DMA 刷新函数 ----------
void LCD_Flush_DMA(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    if (dma_busy) return;

    uint16_t x1 = area->x1;
    uint16_t y1 = area->y1;
    uint16_t x2 = area->x2;
    uint16_t y2 = area->y2;
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);
    uint32_t data_size = pixel_cnt * sizeof(lv_color_t);   // 16位色 = pixel_cnt*2

    LCD_Set_Window(x1, y1, x2, y2);
    LCD_DC_HIGH();
    LCD_CS_LOW();

    dma_busy = 1;
    disp_drv_ref = disp_drv;

    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)color_p, data_size);
}

// ---------- DMA 传输完成回调 ----------
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        LCD_CS_HIGH();
        dma_busy = 0;
        if (disp_drv_ref) {
            lv_disp_flush_ready(disp_drv_ref);
        }
    }
}