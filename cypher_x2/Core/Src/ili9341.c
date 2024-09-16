/*
 * ili9341.c
 *
 *  Created on: Sep 15, 2024
 *      Author: Dhiru
 */



#include "ili9341.h"

static void DAL_SPI_ILI9341_Select() {
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET);
}

void  DAL_SPI_ILI9341_Unselect() {
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);
}

static void DAL_SPI_ILI9341_Reset() {
    HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port, ILI9341_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(ILI9341_RES_GPIO_Port, ILI9341_RES_Pin, GPIO_PIN_SET);
}

static void DAL_SPI_ILI9341_WriteCommand(SPI_HandleTypeDef *hspi1, uint8_t cmd) {
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi1, &cmd, sizeof(cmd), HAL_MAX_DELAY);

}

static void DAL_SPI_ILI9341_WriteData(SPI_HandleTypeDef *hspi1, uint8_t* buff, size_t buff_size) {
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);

    // split data in small chunks because HAL can't send more then 64K at once
    while(buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        HAL_SPI_Transmit(hspi1, buff, chunk_size, HAL_MAX_DELAY);

        buff += chunk_size;
        buff_size -= chunk_size;
    }
}


static void DAL_SPI_ILI9341_SetAddressWindow(SPI_HandleTypeDef *hspi1, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // column address set
	DAL_SPI_ILI9341_WriteCommand(hspi1, 0x2A); // CASET

	uint8_t data1[] = { (x0 >> 8) & 0xFF, x0 & 0xFF, (x1 >> 8) & 0xFF, x1 & 0xFF };
	DAL_SPI_ILI9341_WriteData(hspi1, data1, sizeof(data1));


    // row address set
    DAL_SPI_ILI9341_WriteCommand(hspi1, 0x2B); // RASET

	uint8_t data2[] = { (y0 >> 8) & 0xFF, y0 & 0xFF, (y1 >> 8) & 0xFF, y1 & 0xFF };
	DAL_SPI_ILI9341_WriteData(hspi1, data2, sizeof(data2));


    // write to RAM
    DAL_SPI_ILI9341_WriteCommand(hspi1, 0x2C); // RAMWR
}


static void DAL_SPI_ILI9341_WriteChar(SPI_HandleTypeDef *hspi1, uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    DAL_SPI_ILI9341_SetAddressWindow(hspi1, x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                DAL_SPI_ILI9341_WriteData(hspi1, data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                DAL_SPI_ILI9341_WriteData(hspi1, data, sizeof(data));
            }
        }
    }
}

void DAL_SPI_ILI9341_WriteString(SPI_HandleTypeDef *hspi1, uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
	DAL_SPI_ILI9341_Select();

    while(*str) {
        if(x + font.width >= ILI9341_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ILI9341_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        DAL_SPI_ILI9341_WriteChar(hspi1, x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    DAL_SPI_ILI9341_Unselect();
}

void DAL_SPI_ILI9341_FillRectangle(SPI_HandleTypeDef *hspi1, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >= ILI9341_WIDTH) w = ILI9341_WIDTH - x;
    if((y + h - 1) >= ILI9341_HEIGHT) h = ILI9341_HEIGHT - y;

    DAL_SPI_ILI9341_Select();
    DAL_SPI_ILI9341_SetAddressWindow(hspi1, x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            HAL_SPI_Transmit(hspi1, data, sizeof(data), HAL_MAX_DELAY);

        }
    }

    DAL_SPI_ILI9341_Unselect();
}

void DAL_SPI_ILI9341_FillScreen(SPI_HandleTypeDef *hspi1, uint16_t color) {
	DAL_SPI_ILI9341_FillRectangle(hspi1, 0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}
//
//void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
//    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
//    if((x + w - 1) >= ILI9341_WIDTH) return;
//    if((y + h - 1) >= ILI9341_HEIGHT) return;
//
//    ILI9341_Select();
//    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
//    ILI9341_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
//    ILI9341_Unselect();
//}
//
//void ILI9341_InvertColors(bool invert) {
//    ILI9341_Select();
//    ILI9341_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
//    ILI9341_Unselect();
//}
//
//
//void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
//    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT))
//        return;
//
//    ILI9341_Select();
//
//    ILI9341_SetAddressWindow(x, y, x+1, y+1);
//    uint8_t data[] = { color >> 8, color & 0xFF };
//    ILI9341_WriteData(data, sizeof(data));
//
//    ILI9341_Unselect();
//}
//


HAL_StatusTypeDef DAL_SPI_ILI9341_init(SPI_HandleTypeDef *hspi1)
{
    DAL_SPI_ILI9341_Select();
    DAL_SPI_ILI9341_Reset();

    // SOFTWARE RESET
    DAL_SPI_ILI9341_WriteCommand(hspi1, 0x01);
    HAL_Delay(1000);

     // POWER CONTROL A
    DAL_SPI_ILI9341_WriteCommand(hspi1, 0xCB);

    uint8_t data1[] = { 0x39, 0x2C, 0x00, 0x34, 0x02 };
    DAL_SPI_ILI9341_WriteData(hspi1, data1, sizeof(data1));

     // POWER CONTROL B
    DAL_SPI_ILI9341_WriteCommand(hspi1, 0xCF);

    uint8_t data2[] = { 0x00, 0xC1, 0x30 };
    DAL_SPI_ILI9341_WriteData(hspi1, data2, sizeof(data2));

     // DRIVER TIMING CONTROL A
    DAL_SPI_ILI9341_WriteCommand(hspi1,0xE8);
    uint8_t data3[] = { 0x85, 0x00, 0x78 };
    DAL_SPI_ILI9341_WriteData(hspi1,data3, sizeof(data3));

     // DRIVER TIMING CONTROL B
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xEA);

	 uint8_t data4[] = { 0x00, 0x00 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data4, sizeof(data4));


     // POWER ON SEQUENCE CONTROL
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xED);

	 uint8_t data5[] = { 0x64, 0x03, 0x12, 0x81 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data5, sizeof(data5));


     // PUMP RATIO CONTROL
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xF7);

	 uint8_t data6[] = { 0x20 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data6, sizeof(data6));


     // POWER CONTROL,VRH[5:0]
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xC0);

	 uint8_t data7[] = { 0x23 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data7, sizeof(data7));


     // POWER CONTROL,SAP[2:0];BT[3:0]
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xC1);

	 uint8_t data8[] = { 0x10 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data8, sizeof(data8));


     // VCM CONTROL
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xC5);

	 uint8_t data9[] = { 0x3E, 0x28 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data9, sizeof(data9));


     // VCM CONTROL 2
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xC7);

	 uint8_t data10[] = { 0x86 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data10, sizeof(data10));


     // MEMORY ACCESS CONTROL
     DAL_SPI_ILI9341_WriteCommand(hspi1,0x36);

	 uint8_t data11[] = { 0x48 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data11, sizeof(data11));


     // PIXEL FORMAT
     DAL_SPI_ILI9341_WriteCommand(hspi1,0x3A);

	 uint8_t data12[] = { 0x55 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data12, sizeof(data12));


     // FRAME RATIO CONTROL, STANDARD RGB COLOR
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xB1);

	 uint8_t data13[] = { 0x00, 0x18 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data13, sizeof(data13));


     // DISPLAY FUNCTION CONTROL
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xB6);

	 uint8_t data14[] = { 0x08, 0x82, 0x27 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data14, sizeof(data14));


     // 3GAMMA FUNCTION DISABLE
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xF2);

	 uint8_t data15[] = { 0x00 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data15, sizeof(data15));


     // GAMMA CURVE SELECTED
     DAL_SPI_ILI9341_WriteCommand(hspi1,0x26);

	 uint8_t data16[] = { 0x01 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data16, sizeof(data16));


     // POSITIVE GAMMA CORRECTION
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xE0);

	 uint8_t data17[] = { 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 };
	 DAL_SPI_ILI9341_WriteData(hspi1,data17, sizeof(data17));


     // NEGATIVE GAMMA CORRECTION
     DAL_SPI_ILI9341_WriteCommand(hspi1,0xE1);

	 uint8_t data18[] = { 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F };
	 DAL_SPI_ILI9341_WriteData(hspi1,data18, sizeof(data18));


     // EXIT SLEEP
     DAL_SPI_ILI9341_WriteCommand(hspi1,0x11);
     HAL_Delay(120);

     // TURN ON DISPLAY
     DAL_SPI_ILI9341_WriteCommand(hspi1,0x29);

     // MADCTL
     DAL_SPI_ILI9341_WriteCommand(hspi1,0x36);
	 uint8_t data19[] = { ILI9341_ROTATION };
	 DAL_SPI_ILI9341_WriteData(hspi1,data19, sizeof(data19));


     DAL_SPI_ILI9341_Unselect();

     return HAL_OK;
}

