// Localify breadboard player: change these to match your parts and wiring.
// The full wiring guide is in docs/BREADBOARD.md.
#pragma once

// ---------- Screen ----------
// Pick one:
//   DISPLAY_ST7789   colour SPI screen, 240x240 or 240x320 (1.3", 1.54", 2.0")
//   DISPLAY_ST7735   colour SPI screen, 128x160 (1.8")
//   DISPLAY_LCD_I2C  16x2 or 20x4 character LCD with an I2C backpack (4 pins)
#define DISPLAY_ST7789  1
#define DISPLAY_ST7735  2
#define DISPLAY_LCD_I2C 3
#define DISPLAY_TYPE DISPLAY_ST7789

// Colour screens (ST7789 / ST7735)
#define TFT_W        240
#define TFT_H        240          // 320 for a 240x320 screen
#define TFT_ROTATION 0            // 0-3; change if the picture is sideways or upside down
#define TFT_SPI_MODE SPI_MODE0    // 240x240 screens with no CS pin: SPI_MODE3, and set PIN_TFT_CS to -1
#define ST7735_TAB   INITR_BLACKTAB  // try INITR_GREENTAB if colours or edges look wrong
#define PIN_TFT_SCK  14
#define PIN_TFT_MOSI 13           // marked SDA on most screens
#define PIN_TFT_CS   15
#define PIN_TFT_DC   27
#define PIN_TFT_RST  -1           // -1 = wire the screen's RST to 3.3V
#define PIN_TFT_BL   -1           // -1 = wire the screen's BLK/BL to 3.3V

// Character LCD (I2C backpack)
#define LCD_ADDR 0x27             // some backpacks use 0x3F
#define LCD_COLS 16
#define LCD_ROWS 2
#define PIN_LCD_SDA 21
#define PIN_LCD_SCL 22

// ---------- microSD card (SPI) ----------
#define PIN_SD_SCK  18
#define PIN_SD_MISO 19
#define PIN_SD_MOSI 23
#define PIN_SD_CS   5
#define SD_SPI_HZ   20000000      // lower to 4000000 if the card isn't found

// ---------- I2S amplifier (MAX98357A) ----------
#define PIN_I2S_BCLK 26
#define PIN_I2S_LRC  25           // also called LRCLK or WS
#define PIN_I2S_DIN  32

// ---------- Buttons (each one between the pin and GND) ----------
#define PIN_BTN_UP   4
#define PIN_BTN_DOWN 16
#define PIN_BTN_OK   17
#define PIN_BTN_BACK 33

// ---------- Sound ----------
#define MAX_GAIN     1.0f         // 0.1-4.0. Raise if it's too quiet at full volume, lower if it crackles.
#define START_VOLUME 12           // 0-21, used until you change it
