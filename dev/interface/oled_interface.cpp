//
// Created by Quoke on 3/24/2022.
//

#include "oled_interface.h"

uint8_t oledIF::OLED_GRAM[130][8];

SPIConfig oledIF::SPI1_cfg{
        FALSE,
        nullptr,
        GPIOB,
        GPIOB_OLED_RST,
        0x00000028U,
        0
};

void oledIF::init() {
    spiStart(&OLED_SPI_DRIVER, &oledIF::SPI1_cfg);

    oled_cmd_set();

    oled_reset_clear();
    chThdSleepMilliseconds(500);
    oled_reset_set();

    spiAcquireBus(&OLED_SPI_DRIVER);
    spiSelect(&OLED_SPI_DRIVER);

    oled_write_byte(0xae, OLED_CMD);    //turn off oled panel
    oled_write_byte(0x00, OLED_CMD);    //set low column address
    oled_write_byte(0x10, OLED_CMD);    //set high column address
    oled_write_byte(0x40, OLED_CMD);    //set start line address
    oled_write_byte(0x81, OLED_CMD);    //set contrast control resigter
    oled_write_byte(0xcf, OLED_CMD);    //set SEG output current brightness
    oled_write_byte(0xa1, OLED_CMD);    //set SEG/column mapping
    oled_write_byte(0xc8, OLED_CMD);    //set COM/row scan direction
    oled_write_byte(0xa6, OLED_CMD);    //set nomarl display
    oled_write_byte(0xa8, OLED_CMD);    //set multiplex display
    oled_write_byte(0x3f, OLED_CMD);    //1/64 duty
    oled_write_byte(0xd3, OLED_CMD);    //set display offset
    oled_write_byte(0x00, OLED_CMD);    //not offest
    oled_write_byte(0xd5, OLED_CMD);    //set display clock divide ratio/oscillator frequency
    oled_write_byte(0x80, OLED_CMD);    //set divide ratio
    oled_write_byte(0xd9, OLED_CMD);    //set pre-charge period
    oled_write_byte(0xf1, OLED_CMD);    //pre-charge: 15 clocks, discharge: 1 clock
    oled_write_byte(0xda, OLED_CMD);    //set com pins hardware configuration
    oled_write_byte(0x12, OLED_CMD);    //
    oled_write_byte(0xdb, OLED_CMD);    //set vcomh
    oled_write_byte(0x40, OLED_CMD);    //set vcom deselect level
    oled_write_byte(0x20, OLED_CMD);    //set page addressing mode
    oled_write_byte(0x02, OLED_CMD);    //
    oled_write_byte(0x8d, OLED_CMD);    //set charge pump enable/disable
    oled_write_byte(0x14, OLED_CMD);    //charge pump disable
    oled_write_byte(0xa4, OLED_CMD);    //disable entire dispaly on
    oled_write_byte(0xa7, OLED_CMD);    //disable inverse display on
    oled_write_byte(0xaf, OLED_CMD);    //turn on oled panel

    oled_write_byte(0xaf, OLED_CMD);    //display on

    oled_clear(Pen_Clear);
}

void oledIF::oled_reset_clear() {
    palWritePad(GPIOB, GPIOB_OLED_RST, GPIO_PIN_RESET);
}

void oledIF::oled_reset_set() {
    palWritePad(GPIOB, GPIOB_OLED_RST, GPIO_PIN_SET);
}

void oledIF::oled_cmd_clear() {
    palWritePad(GPIOB, GPIOB_OLED_DC, GPIO_PIN_RESET);
}

void oledIF::oled_cmd_set() {
    palWritePad(GPIOB, GPIOB_OLED_DC, GPIO_PIN_SET);
}

void oledIF::oled_write_byte(uint8_t data, uint8_t cmd) {
    if (cmd != 0) {
        oled_cmd_set();
    }
    else {
        oled_cmd_clear();
    }
    spiSend(&OLED_SPI_DRIVER, 1, &data);
}

void oledIF::oled_clear(oledIF::Pen_Typedef pen)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 130; n++)
        {
            if (pen == Pen_Write)
                OLED_GRAM[n][i] = 0xff;
            else if (pen == Pen_Clear)
                OLED_GRAM[n][i] = 0x00;
            else
                OLED_GRAM[n][i] = 0xff - OLED_GRAM[n][i];
        }
    }
}

void oledIF::oled_refresh_gram() {
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        oled_set_pos(0, i);

        for (n = 0; n < 130; n++)
        {
            oled_write_byte(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}

void oledIF::oled_set_pos(uint8_t x, uint8_t y)
{
    x += 2;
    oled_write_byte((0xb0 + y), OLED_CMD);              //set page address y
    oled_write_byte(((x&0xf0)>>4)|0x10, OLED_CMD);      //set column high address
    oled_write_byte((x&0xf0), OLED_CMD);                //set column low address
}

void oledIF::oled_LOGO()
{
    oledIF::oled_clear(oledIF::Pen_Clear);
    uint8_t temp_char = 0;
    uint8_t x = 0, y = 0;
    uint8_t i = 0;
    for(; y < 64; y += 8)
    {
        for(x = 0; x < 128; x++)
        {
            temp_char = oledIF::LOGO_BMP[x][y/8];
            for(i = 0; i < 8; i++)
            {
                if(temp_char & 0x80) oledIF::oled_drawpoint(x, y + i,oledIF::Pen_Write);
                else oledIF::oled_drawpoint(x,y + i,oledIF::Pen_Clear);
                temp_char <<= 1;
            }
        }
    }
    oledIF::oled_refresh_gram();
}

void oledIF::oled_drawpoint(int8_t x, int8_t y, oledIF::Pen_Typedef pen)
{
    uint8_t page = 0, row = 0;

    /* check the corrdinate */
    if ((x < 0) || (x > (X_WIDTH - 1)) || (y < 0) || (y > (Y_WIDTH - 1)))
        return;

    page = y / 8;
    row = y % 8;

    if (pen == Pen_Write)
        OLED_GRAM[x][page] |= 1 << row;
    else if (pen == Pen_Inversion)
        OLED_GRAM[x][page] ^= 1 << row;
    else
        OLED_GRAM[x][page] &= ~(1 << row);
}

const unsigned char  oledIF::LOGO_BMP[128][8] = {
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x40,0x00,0x00,0x00,0x00,0x00,0x00,0xFE},
{0x60,0x00,0x00,0x00,0x00,0x10,0x00,0x6C},
{0x70,0x00,0x00,0x00,0x00,0x30,0x00,0x00},
{0x78,0x00,0x00,0x00,0x00,0xF0,0x00,0x7C},
{0x7C,0x00,0x00,0x00,0x07,0xF0,0x00,0xFE},
{0x7E,0x00,0x00,0x00,0x3F,0xF0,0x00,0xC6},
{0x7F,0x00,0x00,0x01,0xFF,0xF0,0x00,0xC6},
{0x7F,0x80,0x00,0x0F,0xFF,0xF0,0x00,0xC6},
{0x7F,0xC0,0x00,0x7F,0xFF,0xF0,0x00,0xFE},
{0x7F,0xE0,0x03,0xFF,0xFF,0xF0,0x00,0x7C},
{0x7F,0xF0,0x3F,0xFF,0xFF,0xF0,0x00,0x02},
{0x7F,0xF8,0x3F,0xFF,0xFF,0xF0,0x00,0x06},
{0x7F,0xFC,0x3F,0xFF,0xFF,0xF0,0x00,0x1E},
{0x7F,0xFE,0x3F,0xFF,0xFF,0xF0,0x00,0xBC},
{0x7F,0xFF,0x3F,0xFF,0xFF,0xF0,0x00,0xE0},
{0x7F,0xFF,0xBF,0xFF,0xFF,0x80,0x00,0xF8},
{0x7F,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x3E},
{0x7F,0xFF,0xFF,0xFF,0xE0,0x00,0x00,0x0E},
{0x7F,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xB8},
{0x7F,0xFF,0xFF,0xF8,0x00,0x00,0x00,0xE0},
{0x7F,0xFF,0xFF,0xF0,0x00,0x00,0x00,0xFE},
{0x7F,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x1E},
{0x7F,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x02},
{0x7F,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00},
{0x7F,0xEF,0xFF,0xF0,0x02,0x00,0x00,0x06},
{0x7F,0xE7,0xFF,0xF0,0x02,0x00,0x00,0x0E},
{0x7F,0xE3,0xFF,0xF0,0x02,0x00,0x00,0x1C},
{0x7F,0xE1,0xFF,0xF8,0x03,0x00,0x00,0xBA},
{0x7F,0xE0,0xFF,0xFC,0x03,0x00,0x00,0xF6},
{0x7F,0xE0,0x7F,0xFE,0x03,0x80,0x00,0xE6},
{0x7F,0xE0,0x3F,0xFF,0x03,0x80,0x00,0xF6},
{0x7F,0xE0,0x3F,0xFF,0x83,0xC0,0x00,0x3E},
{0x7F,0xE0,0x3F,0xFF,0xC3,0xC0,0x00,0x0E},
{0x7F,0xE0,0x3F,0xFF,0xE3,0xE0,0x00,0x02},
{0x7F,0xE0,0x3F,0xFF,0xF3,0xE0,0x00,0x00},
{0x7F,0xE0,0x3F,0xFF,0xFB,0xF0,0x00,0x02},
{0x7F,0xE0,0x3F,0xFF,0xFF,0xF0,0x00,0x66},
{0x7F,0xE0,0x3F,0xFF,0xFF,0xF8,0x00,0xF6},
{0x7F,0xE0,0x3F,0xFF,0xFF,0xF8,0x00,0xD6},
{0x7F,0xE0,0x3F,0xFF,0xFF,0xFC,0x00,0xD6},
{0x7F,0xF0,0x7F,0xFF,0xFF,0xFC,0x00,0xD6},
{0x7F,0xF8,0xFF,0xF7,0xFF,0xFE,0x00,0xD6},
{0x7F,0xFF,0xFF,0xF3,0xFF,0xFE,0x00,0xDE},
{0x3F,0xFF,0xFF,0xE1,0xFF,0xFF,0x00,0x8C},
{0x3F,0xFF,0xFF,0xE0,0xFF,0xCF,0x00,0x40},
{0x1F,0xFF,0xFF,0xC0,0x7F,0xC7,0x80,0xC0},
{0x1F,0xFF,0xFF,0xC0,0x3F,0xC3,0x80,0xC0},
{0x0F,0xFF,0xFF,0x80,0x1F,0xC1,0xC0,0xFE},
{0x07,0xFF,0xFF,0x00,0x0F,0xC0,0xC0,0xFE},
{0x03,0xFF,0xFE,0x00,0x07,0xC0,0x60,0xC0},
{0x01,0xFF,0xFC,0x00,0x03,0xC0,0x20,0xC0},
{0x00,0x7F,0xF0,0x00,0x01,0xC0,0x00,0x86},
{0x00,0x0F,0x80,0x00,0x00,0xC0,0x00,0x16},
{0x00,0x00,0x00,0x00,0x00,0x40,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD0},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x62},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD6},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDE},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
};