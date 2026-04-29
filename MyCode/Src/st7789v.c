#include "st7789v.h"
#include "spi.h"

#define     RED          0XF800	  //��ɫ
#define     GREEN        0X07E0	  //��ɫ
#define     BLUE         0X001F	  //��ɫ
#define     WHITE        0XFFFF	  //��ɫ

// ========== 硬件引脚宏（请根据你的实际引脚修改）==========


static uint8_t dma_busy = 0;
static lv_disp_drv_t *disp_drv_ref = NULL;

// ---------- 基础 SPI 发送 ----------
void SPI_SendByte(uint8_t data) {
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
    LCD_Delay_ms(50);
    LCD_RST_LOW();
    LCD_Delay_ms(100);
    LCD_RST_HIGH();
    LCD_Delay_ms(120);

    // 修改 1: 添加软件复位命令，确保状态稳定
    sendCmd(0x01);   // Software Reset
    LCD_Delay_ms(150);

    sendCmd(0x11);   // 退出睡眠模式
    LCD_Delay_ms(120);

    // 修改 2: 修改 0x36 寄存器参数，设置为横屏模式 (320x240)
    sendCmd(0x36);
    sendData(0xA0);   // 修改: 0x60 -> 0xA0

    sendCmd(0x3A);
    sendData(0x55);   // 16位色 (RGB565)


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

    sendCmd(0x20);   // 反转显示
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

