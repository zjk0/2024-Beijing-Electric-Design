/*
 * LCD driver for the ILI9488 TFT LCD chips.
 *
 * Feel free to use, change, distribute this code as desired. Use under
 * GPLv3 open-source license.
 *
 * File:   ILI9488.c
 * Author: tommy
 *
 * Created on 29th March 2021
 */


#include "ILI9488.h"
#include "font.h"
#include "main.h"
#include "delay.h"

/*
 * A little bit of video RAM to speed things up. This lets the HAL SPI library
 * transfer data in bulk rather than separate transactions.
 * The minimum value is 3 (1 byte each for R, G, and B).
 * The theoretical maximum is 0xFFFF - 1 but that doesn't seem to work. Pick a
 * size that suits your RAM budget and works with your controller.
 */
#define V_BUFFER_SIZE 0x3FFF
;
uint8_t v_buffer[V_BUFFER_SIZE];

uint16_t buffer_counter = 0;

extern uint8_t BasicalWaveFrequency[5] = {0, 1, 2, 3, 28};
extern uint8_t CarrierWaveFrequency[5] = {4, 1, 2, 3, 28};
extern uint8_t DirectSignalAttenuation[7] = {5, 6, 9, 10, 13, 14, 28};
extern uint8_t DirectSignalDelay[7] = {5, 6, 9, 10, 11, 12, 28};
extern uint8_t MultipathSignalAttenuation[7] = {7, 8, 9, 10, 13, 14, 28};
extern uint8_t MultipathSignalDelay[7] = {7, 8, 9, 10, 11, 12, 28};
extern uint8_t AM_ModulationDegree[6] = {23, 24, 20, 21, 22, 28};
extern uint8_t InitialPhaseDifference[6] = {15, 16, 17, 18, 19, 28};
extern uint8_t AdjustValue[4] = {20, 25, 26, 28};
extern uint8_t FrequencyData[2] = {2, 3};
extern uint8_t Amplitude[2] = {27, 22};
extern uint8_t Delay[2] = {11, 12};
extern uint8_t ModulationDegree[3] = {20, 21, 22};
extern uint8_t AmplitudeValidVaule[6] = {27, 22, 30, 31, 32, 28};

/*
 * Writes a byte to SPI without changing chip select (CS) state.
 * Called by the write_command() and write_data() functions which
 * control these pins as required.
 */
void spi_write(unsigned char data) {
	HAL_SPI_Transmit(&hspi2, &data, 1, 10);
	while(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);
}

/*
 * Writes the V-RAM buffer to the display.
 * TODO: currently only uses SPI.
 */
void write_buffer() {
	HAL_SPI_Transmit(&hspi2, v_buffer, buffer_counter, 10);
	buffer_counter = 0;
}

/**
 * Writes data to an 8-bit parallel bus. This uses a (slow),
 * individual pin write mode just because the HAL libraries
 * don't offer a full port write.
 * If it suits your project you can use
 * GPIOx->ODR = data;
 * instead.
 */
void parallel_write(unsigned char data) {
	//In this particular example I'm using PA5:PA12
    HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_RESET);
	GPIOA->ODR = (data << 5);
	delay_ms(1);
    HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET);
}

/*
 * Writes a data byte to the display. Pulls CS low as required.
 */
void lcd_write_data(unsigned char data) {
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    if(SPI_MODE)
    	spi_write(data);
	else
		parallel_write(data);
    //HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

/*
 * Writes a command byte to the display
 */
void lcd_write_command(unsigned char data) {
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    if(SPI_MODE)
    	spi_write(data);
	else
		parallel_write(data);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void lcd_write_reg(unsigned int data) {
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    if(SPI_MODE)
    	spi_write(data);
	else
		parallel_write(data);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

/**
 * Read a number of bytes from teh display.
 */
void lcd_read_bytes(int byteCount) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Set the GPIO mode to INPUT
	GPIO_InitStruct.Pin = LCD0_Pin|LCD1_Pin|LCD2_Pin|LCD3_Pin
						  |LCD4_Pin|LCD5_Pin|LCD6_Pin|LCD7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Read each byte
	while(byteCount--) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_RESET);
		delay_ms(10);
		unsigned int data = GPIOA->IDR;
		data = (data >> 5) & 0xFF;
		HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_SET);
	}

    //Reset the GPIO mode to OUTPUT
    GPIO_InitStruct.Pin = LCD0_Pin|LCD1_Pin|LCD2_Pin|LCD3_Pin
                            |LCD4_Pin|LCD5_Pin|LCD6_Pin|LCD7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
 * Swaps two 16-bit integers
 */
void swap_int(unsigned int *num1, unsigned int *num2) {
    int temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

/*
 * Swaps two 8-bit integers
 */
void swap_char(char *num1, char *num2) {
    char temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

/*
 * Delay calcualted on 32MHz clock.
 * Does NOT adjust to clock setting
 */
void delay_ms(double millis) {
    int multiplier = 4;
    double counter = millis;
    while(multiplier--) {
        while(counter--);
        counter = millis;
    }
}

/*
 * A short microsecond delay routine
 * (not measured)
 */
void delay_us(long int cycles) {
    while(cycles--);
}

/*
 * Initialisation routine for the LCD
 * I got this from the one of the ebay sellers which make them.
 * From Open-Smart
 */
void lcd_init_parallel() {

    //SET control pins for the LCD HIGH (they are active LOW)
    HAL_GPIO_WritePin(RESX_PORT, RESX_PIN, GPIO_PIN_SET); //RESET pin HIGH (Active LOW)
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET); //Chip Select Active LOW
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET); //Data / Command select Active LOW
    HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_SET); //READ pin HIGH (active LOW)
    HAL_GPIO_WritePin(WR_PORT, WR_PIN, GPIO_PIN_SET); //WRITE pin HIGH (active LOW)
    //Cycle reset pin
    delay_ms(100);
    HAL_GPIO_WritePin(RESX_PORT, RESX_PIN, GPIO_PIN_RESET);
    // HAL_Delay(100);
    delay_ms(100);
    HAL_GPIO_WritePin(RESX_PORT, RESX_PIN, GPIO_PIN_SET);
    delay_ms_PE(100);

    lcd_init_command_list();

}
/*
 * Same as above, but initialises with an SPI port instead.
 */
void lcd_init_spi() {
    //SET control pins for the LCD HIGH (they are active LOW)
    HAL_GPIO_WritePin(RESX_PORT, RESX_PIN, GPIO_PIN_SET); //RESET pin HIGH (Active LOW)
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET); //Chip Select Active LOW
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET); //Data / Command select Active LOW
    //Cycle reset pin
    delay_ms_PE(100);
    HAL_GPIO_WritePin(RESX_PORT, RESX_PIN, GPIO_PIN_RESET);
    delay_ms_PE(500);
    HAL_GPIO_WritePin(RESX_PORT, RESX_PIN, GPIO_PIN_SET);
    delay_ms_PE(500);

    lcd_init_command_list();

}

/**
 * This is the magic initialisation routine.
 */
void lcd_init_command_list(void)
{

	//********Start Initial Sequence*******//
	lcd_write_command(0xE0); //P-Gamma
	lcd_write_data(0x00);
	lcd_write_data(0x13);
	lcd_write_data(0x18);
	lcd_write_data(0x04);
	lcd_write_data(0x0F);
	lcd_write_data(0x06);
	lcd_write_data(0x3A);
	lcd_write_data(0x56);
	lcd_write_data(0x4D);
	lcd_write_data(0x03);
	lcd_write_data(0x0A);
	lcd_write_data(0x06);
	lcd_write_data(0x30);
	lcd_write_data(0x3E);
	lcd_write_data(0x0F);
	lcd_write_command(0XE1); //N-Gamma
	lcd_write_data(0x00);
	lcd_write_data(0x13);
	lcd_write_data(0x18);
	lcd_write_data(0x01);
	lcd_write_data(0x11);
	lcd_write_data(0x06);
	lcd_write_data(0x38);
	lcd_write_data(0x34);
	lcd_write_data(0x4D);
	lcd_write_data(0x06);
	lcd_write_data(0x0D);
	lcd_write_data(0x0B);
	lcd_write_data(0x31);
	lcd_write_data(0x37);
	lcd_write_data(0x0F);
	lcd_write_command(0xC0);
	lcd_write_data(0x18);
	lcd_write_data(0x16);
	lcd_write_command(0xC1);
	lcd_write_data(0x45);
	lcd_write_command(0xC5); //VCOM
	lcd_write_data(0x00);
	lcd_write_data(0x63);
	lcd_write_data(0x01);

	lcd_write_command(0x36); //RAM address mode
	//0xF8 and 0x3C are landscape mode. 0x5C and 0x9C for portrait mode.
	if(LANDSCAPE)
		lcd_write_data(0xF8);
	else
		lcd_write_data(0x5C);

	lcd_write_command(0x3A); //Interface Mode Control
	lcd_write_data(0x66); //16-bit serial mode
	lcd_write_command(0xB0); //Interface Mode Control
	lcd_write_data(0x80); //SDO not in use
	lcd_write_command(0xB1); //Frame rate 70HZ
	lcd_write_data(0x00); //
	lcd_write_data(0x10);
	lcd_write_command(0xB4);
	lcd_write_data(0x02);

	lcd_write_command(0xB6); //RGB/MCU Interface Control
	lcd_write_data(0x02);
	//lcd_write_data(0x22);

	lcd_write_command(0xE9);
	lcd_write_data(0x00);
	lcd_write_command(0xF7);
	lcd_write_data(0xA9);
	lcd_write_data(0x51);
	lcd_write_data(0x2C);
	lcd_write_data(0x82);
	lcd_write_command(0x11);
	delay_ms_PE(120);
	lcd_write_command(0x21);


	delay_ms_PE(120);
	lcd_write_command(0x29);
}

/*
 * Sets the X,Y position for following commands on the display.
 * Should only be called within a function that draws something
 * to the display.
 */
void set_draw_window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {

    //Check that the values are in order
    if(x2 < x1)
        swap_int(&x2, &x1);
    if(y2 < y1)
        swap_int(&y2, &y1);

    lcd_write_command(ILI9488_CASET);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xFF);

    lcd_write_data(x2 >> 8);
    lcd_write_data(x2 & 0xFF);

    lcd_write_command(ILI9488_PASET);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);

    lcd_write_data(y2 >> 8);
    lcd_write_data(y2 & 0xFF);

    lcd_write_command(ILI9488_RAMWR);
}

/*
 * Draws a single pixel to the LCD at position X, Y, with
 * Colour.
 *
 * 28 bytes per pixel. Use it wisely.
 */
void draw_pixel(unsigned int x, unsigned int y, unsigned int colour) {

    //All my colours are in 16-bit RGB 5-6-5 so they have to be converted to 18-bit RGB
    unsigned char r = (colour >> 8) & 0xF8;
    unsigned char g = (colour >> 3) & 0xFC;
    unsigned char b = (colour << 3);

    //Set the x, y position that we want to write to
    set_draw_window(x, y, x+1, y+1);
    lcd_write_data(r);
    lcd_write_data(g);
    lcd_write_data(b);
}

/*
 * Fills a rectangle with a given colour
 */
void fill_rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour) {
    //All my colours are in 16-bit RGB 5-6-5 so they have to be converted to 18-bit RGB
    unsigned char r = (colour >> 8) & 0xF8;
    unsigned char g = (colour >> 3) & 0xFC;
    unsigned char b = (colour << 3);


    //Set the drawing region
    set_draw_window(x1, y1, x2, y2);

    //We will do the SPI write manually here for speed
    //( the data sheet says it doesn't matter if CS changes between
    // data sections but I don't trust it.)
    //CS low to begin data
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);


    //Write colour to each pixel
    for(int y = 0; y < y2-y1+1 ; y++) {
        for(int x = 0; x < x2-x1+1; x++) {
		    v_buffer[buffer_counter] = r;
		    buffer_counter++;
		    v_buffer[buffer_counter] = g;
		    buffer_counter++;
		    v_buffer[buffer_counter] = b;
		    buffer_counter++;

			//If the buffer is full then send it to the dispaly
			if(buffer_counter > V_BUFFER_SIZE - 3) {
				write_buffer();
			}
        }
    }
    //Send the remaining bytes
	write_buffer();
    //Return CS to high
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

/*
 * Draws a single char to the screen.
 * Called by the various string writing functions like print().
 *
 * NOTE:
 * This sends approx. 800 bytes per char to the LCD, but it does preserver
 * the background image. Use the draw_fast_char() function where possible.
 */
void draw_char(unsigned int x, unsigned int y, char c, unsigned int colour, char size) {
    int i, j;
    char line;
    unsigned int font_index = (c - 32);

    //Get the line of pixels from the font file
    for(i=0; i<13; i++ ) {

        line = FontLarge[font_index][12 - i];

        //Draw the pixels to screen
        for(j=0; j<8; j++) {
            if(line & (0x01 << j)) {
                if(size == 1) {
                    //If we are just doing the smallest size font then do a single pixel each
                    draw_pixel(x+(8-j), y+i, colour);
                }
                else {
                    // do a small box to represent each pixel
                    fill_rectangle(x+((8-j)*size), y+((i)*size), x+((8-j)*size)+size, y+((i)*size)+size, colour);
                }
            }
        }
    }
}

/*
 * Draws a char to the screen using a constant stream of pixel data whic his faster
 * than drawing individual pixels.
 * This will draw over any background image though.
 *
 * NOTE: This sends 130 bytes for a regular sized char
 */
void draw_fast_char(unsigned int x, unsigned int y, char c, unsigned int colour, unsigned int bg_colour) {
    char line;
    char width = 8;
    char height = 13;
    unsigned int font_index = (c - 32);
    unsigned int this_px = bg_colour;
    //If the buffer is too small to fit a full character then we have to write each pixel
    int smallBuffer = 0;
    if(V_BUFFER_SIZE < height * width) {
    	smallBuffer++;
    }

    //Set the drawing region
    set_draw_window(x, y, x + width - 1, y + height);

    //We will do the SPI write manually here for speed
    //CS low to begin data
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

    //Get the line of pixels from the font file
    for(int i=0; i < height; i++ ) {
        line = FontLarge[font_index][12 - i];

        //Draw the pixels to screen
        for(int j = width-1; j >= 0; j--) {
            //Default pixel colour is the background colour, unless changed below
            this_px = bg_colour;
			if((line >> (j)) & 0x01)
				this_px = colour;

            //DCreate the bitmap in the frame buffer
		    v_buffer[buffer_counter++] = (this_px >> 8) & 0xF8;
		    v_buffer[buffer_counter++] = (this_px >> 3) & 0xFC;
		    v_buffer[buffer_counter++] = (this_px << 3);

		    //If the buffer was too small for a full font then write each pixel
		    if(smallBuffer)
		    	write_buffer();
        }
    }

    //Write the vram buffer to the display
    if(!smallBuffer)
    	write_buffer();

    //Return CS to high
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}


/*
 * Writes a string to the display as an array of chars at position x, y with
 * a given colour and size.
 */
void draw_string(unsigned int x, unsigned int y, unsigned int colour, char size, char *str) {

    //Work out the size of each character
    int char_width = size * 9;
    //Iterate through each character in the string
    int counter = 0;
    while(str[counter] != '\0') {
        //Calculate character position
        int char_pos = x + (counter * char_width);
        //Write char to the display
        draw_char(char_pos, y, str[counter], colour, size);
        //Next character
        counter++;
    }
}

/*
 * Draws a string using the draw_fast_char() function.
 * This will not preserve any background image and so a custom background
 * colour should be provided.
 * NOTE: Can only be the regular sized font. No scaling.
 */
void draw_fast_string(unsigned int x, unsigned int y, unsigned int colour, unsigned int bg_colour, char *str) {
    //Iterate through each character in the string
    int counter = 0;
    while(str[counter] != '\0') {
        //Write char to the display
        draw_fast_char(x + (counter * 9), y, str[counter], colour, bg_colour);
        //Next character
        counter++;
    }
}

/*
 * Draws a bitmap by directly writing the byte stream to the LCD.
 *
 * So the scaling is done strangely here because writing individual pixels
 * has an overhead of 26 bytes each.
 */
void draw_bitmap(unsigned int x1, unsigned int y1, int scale, const unsigned int *bmp) {
	int width = bmp[0];
	int height = bmp[1];
	unsigned int this_byte;
	int x2 = x1 + (width * scale);
	int y2 = y1 + (height * scale);

	//Set the drawing region
	set_draw_window(x1, y1, x2 + scale - 1, y2);

	//We will do the SPI write manually here for speed
    HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
	//CS low to begin data
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

	//Write colour to each pixel
	for (int i = 0; i < height; i++) {
		//this loop does the vertical axis scaling (two of each line))
		for (int sv = 0; sv < scale; sv++) {
			for (int j = 0; j <= width; j++) {
				//Choose which byte to display depending on the screen orientation
				//NOTE: We add 2 bytes because of the first two bytes being dimension data in the array
				this_byte = bmp[(width * (i)) + j + 2];

				//And this loop does the horizontal axis scale (three bytes per pixel))
				for (int sh = 0; sh < scale; sh++) {

				    unsigned char r = (this_byte >> 8) & 0xF8;
				    unsigned char g = (this_byte >> 3) & 0xFC;
				    unsigned char b = (this_byte << 3);
				    v_buffer[buffer_counter] = r;
				    buffer_counter++;
				    v_buffer[buffer_counter] = g;
				    buffer_counter++;
				    v_buffer[buffer_counter] = b;
				    buffer_counter++;

					//If the buffer is full then send it to the dispaly
					if(buffer_counter > V_BUFFER_SIZE - 3) {
						write_buffer();
					}
				}
			}
		}
	}
	//Send the remaining bytes
	write_buffer();

	//Return CS to high
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

}

uint32_t pow (uint32_t num, uint32_t count) {
    uint32_t result = 1;

    while (count != 0) {
        result *= num;
        count--;
    }

    return result;
}

void LCD_DrawChar (uint16_t x, uint16_t y, char ch, uint16_t char_width, uint16_t char_height, unsigned int CharColor, unsigned int BackColor) {
    if (x > WIDTH || y > HEIGHT) {
        return;
    }

    uint16_t char_position, char_offset;

    char_position = ch - ' ';
    char_offset = char_position * 32;
    uint8_t* ch_ptr = &Font16_Table[char_offset];

    for (int i = 0; i < char_height; i++) {
        for (int j = 0; j < char_width; j++) {
            if (*ch_ptr & (0x80 >> (j % 8))) {
                draw_pixel(x + j, y + i, CharColor);
            }
            else {
                draw_pixel(x + j, y + i, BackColor);
            }

            if (j % 8 == 7) {
                ch_ptr++;
            }
        }

        if (char_width % 8 != 0) {
            ch_ptr++;
        }
    }
}

void LCD_DrawString (uint16_t x, uint16_t y, char* string, uint16_t strLength, uint16_t char_width, uint16_t char_height, unsigned int CharColor, unsigned int BackColor) {
    char *ch = string;
    uint16_t pos_x = x, pos_y = y;

    for (uint16_t i = 0; i < strLength; i++) {
        LCD_DrawChar(pos_x, pos_y, *ch, char_width, char_height, CharColor, BackColor);
        ch++;
        pos_x += (char_width + 1);
    }
}

void LCD_DrawSingleChinese (uint16_t x, uint16_t y, uint8_t chinese_position, uint16_t ChineseWidth, uint16_t ChineseHeight, unsigned int ChineseColor, unsigned int BackColor) {
    if (x > WIDTH || y > HEIGHT) {
        return;
    }

    uint16_t chinese_offset;

    chinese_offset = chinese_position * 60;
    uint8_t* ch_ptr = &ChineseTable[chinese_offset];

    for (int i = 0; i < ChineseHeight; i++) {
        for (int j = 0; j < ChineseWidth; j++) {
            if (*ch_ptr & (0x80 >> (j % 8))) {
                draw_pixel(x + j, y + i, ChineseColor);
            }
            else {
                draw_pixel(x + j, y + i, BackColor);
            }

            if (j % 8 == 7) {
                ch_ptr++;
            }
        }

        if (ChineseWidth % 8 != 0) {
            ch_ptr++;
        }
    }
}

void LCD_DrawChinese (uint16_t x, uint16_t y, uint8_t* Chinese, uint16_t ChineseLength, uint16_t ChineseWidth, uint16_t ChineseHeight, unsigned int ChineseColor, unsigned int BackColor) {
    uint8_t* SingleChinese = Chinese;
    uint16_t pos_x = x, pos_y = y;

    for (uint16_t i = 0; i < ChineseLength; i++) {
        LCD_DrawSingleChinese(pos_x, pos_y, *SingleChinese, ChineseWidth, ChineseHeight, ChineseColor, BackColor);
        SingleChinese++;
        pos_x += (ChineseWidth + 1);
    }
}

void LCD_ShowNumber (uint16_t x, uint16_t y, uint32_t num, uint16_t NumWidth, uint16_t NumHeight, unsigned int NumColor, unsigned int BackColor) {
    uint32_t TempNum = num;
    uint32_t ShowNum;
    uint16_t NumCount = 0;
    uint16_t pos_x = x, pos_y = y;

    if (num == 0) {
        NumCount++;
    }

    while (TempNum != 0) {
        TempNum = TempNum / 10;
        NumCount++; 
    }

    for (uint16_t i = 0; i < NumCount; i++) {
        ShowNum = num / pow(10, NumCount - i - 1);
        LCD_DrawChar(pos_x, pos_y, '0' + ShowNum, NumWidth, NumHeight, NumColor, BackColor);
        num = num % pow(10, NumCount - i - 1);
        pos_x += NumWidth;
    }
}

void ShowInformation (uint16_t ChineseWidth, uint16_t ChineseHeight, unsigned int ChineseColor, unsigned int BackColor) {
    LCD_DrawChinese(2, 2, BasicalWaveFrequency, 5, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 1 + 2, CarrierWaveFrequency, 5, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    // LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 2 + 2, DirectSignalAttenuation, 7, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    // LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 3 + 2, DirectSignalDelay, 7, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    // LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 2 + 2, MultipathSignalAttenuation, 7, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 2 + 2, MultipathSignalDelay, 7, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 3 + 2, AM_ModulationDegree, 6, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 4 + 2, InitialPhaseDifference, 6, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 5 + 2, AmplitudeValidVaule, 6, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChinese(2, 2 + CHINESE_HEIGHT * 6 + 2, AdjustValue, 4, CHINESE_WIDTH, CHINESE_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    // LCD_DrawString(2 + CHINESE_WIDTH * 4 + 2, 2 + CHINESE_HEIGHT * 6 + 6, "CW or AM", 8, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    // LCD_DrawString(2, HEIGHT - CHAR_HEIGHT - 2, "CW", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

    LCD_ShowNumber(2 + CHINESE_WIDTH * 5 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2, 
                   2000000, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawString(2 + CHINESE_WIDTH * 5 + 2 + CHAR_WIDTH * 7 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2, 
                   "Hz", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

    LCD_ShowNumber(2 + CHINESE_WIDTH * 5 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT + 6, 
                   Frequency[Frequency_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawString(2 + CHINESE_WIDTH * 5 + 2 + CHAR_WIDTH * 8 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT + 6, 
                   "Hz", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

    LCD_ShowNumber(2 + CHINESE_WIDTH * 7 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8, 
                   DelayTimeData[DelayTime_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawString(2 + CHINESE_WIDTH * 7 + 2 + CHAR_WIDTH * 3 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 2 + 8, 
                   "ns", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

    LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 3 + 16, 
                   ModulationDegreeData[ModulationDegree_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawChar(2 + CHINESE_WIDTH * 6 + 2 + CHAR_WIDTH * 2 + 2, 
                 2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 3 + 16, 
                 '%', CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);

    LCD_ShowNumber(2 + CHINESE_WIDTH * 6 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20, 
                   AmplitudeValidValueData[Amplitude_index], CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
    LCD_DrawString(2 + CHINESE_WIDTH * 6 + 2 + CHAR_WIDTH * 4 + 2, 
                   2 + CHINESE_HEIGHT / 2 - CHAR_HEIGHT / 2 + 2 + CHAR_HEIGHT * 5 + 20, 
                   "mV", 2, CHAR_WIDTH, CHAR_HEIGHT, COLOR_WHITE, COLOR_DARKBLUE);
}
