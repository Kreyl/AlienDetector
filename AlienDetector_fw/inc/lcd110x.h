/*
 * File:   lcd110x.h
 * Author: Kreyl Laurelindo
 *
 * Created on 24 �������� 2010 �., 14:25
 */

#ifndef LCD110X_H
#define	LCD110X_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "delay_util.h"

#include <string.h>

// ================================ Defines ====================================
#define LCD_GPIO        GPIOB

#define LCD_SCLK        GPIO_Pin_12
#define LCD_SDA         GPIO_Pin_10
#define LCD_XRES        GPIO_Pin_11
#define LCD_XCS         GPIO_Pin_2

#define LCD_STR_HEIGHT  8
#define LCD_STR_WIDTH   16


// Uncomment this line, if image need to be turned upside down
//#define LCD_UPSIDEDOWN

// Data sizes
#define LCD_VIDEOBUF_SIZE       864     // = 96 * 9

enum Invert_t {NotInverted, Inverted};

class Lcd_t {
private:
    // Variables
    uint8_t IBuf[LCD_VIDEOBUF_SIZE];
    uint16_t CurrentPosition;   // x, y to place data to
    // Text
    uint32_t Timer;
    // Pin driving functions
    void XRES_Hi(void) { LCD_GPIO->BSRR = LCD_XRES; }
    void XRES_Lo(void) { LCD_GPIO->BRR  = LCD_XRES; }
    void XCS_Hi (void) { LCD_GPIO->BSRR = LCD_XCS;  }
    void XCS_Lo (void) { LCD_GPIO->BRR  = LCD_XCS;  }
    void SCLK_Hi(void) { LCD_GPIO->BSRR = LCD_SCLK; }
    void SCLK_Lo(void) { LCD_GPIO->BRR  = LCD_SCLK; }
    void SDA_Hi (void) { LCD_GPIO->BSRR = LCD_SDA;  }
    void SDA_Lo (void) { LCD_GPIO->BRR  = LCD_SDA;  }
    void WriteCmd(uint8_t ACmd);
    void WriteData(uint8_t AData);
    // High-level
    void GotoXY(uint8_t x, uint8_t y);
    void GotoCharXY(uint8_t x, uint8_t y);
    void DrawChar(uint8_t AChar, Invert_t AInvert);
    void PrintUint (uint32_t ANumber);
    void PrintInt (int32_t ANumber);
    void PrintString (const uint8_t x, const uint8_t y, const char *S, Invert_t AInvert);
    void PrintStringLen(const char *S, uint16_t ALen, Invert_t AInvert);
public:
    // General use
    void Init(void);
    void Task(void);
    void Shutdown(void);
    // High-level
    void Printf(const uint8_t x, const uint8_t y, const char *S, ...);
    void Cls(void);
    void DrawImage(const uint8_t x, const uint8_t y, const uint8_t *Img);
    void DrawSymbol(const uint8_t x, const uint8_t y, const uint8_t ACode);
    void DrawPeak(const uint8_t N, const uint8_t ALevel);
};

extern Lcd_t Lcd;

#endif	/* LCD110X_H */

