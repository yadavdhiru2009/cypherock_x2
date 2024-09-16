/*
 * fonts.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Dhiru
 */

#ifndef INC_FONTS_H_
#define INC_FONTS_H_


#include <stdint.h>

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;


extern FontDef Font_7x10;

#endif /* INC_FONTS_H_ */
