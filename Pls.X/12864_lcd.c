// LIBRAY FROM: https://www.codeproject.com/Articles/5360360/Experimenting-with-ST7920-128-64-Graphical-LCD-on


#include "delay.h"			
#include "12864_lcd.h"
#include "font.h"
#include "string.h"
#include <xc.h>

void LCD_Init(void) 
{ 
    TRISB = 0;
	LCD_REST=1; 
	LCD_REST=0; 
	delay_ms(5);
	LCD_REST=1;  

	delay_ms(50);

	LCD_WriteCommand(0b00100000); // function set, 4 bit mode
	delay_ms(5);

	LCD_WriteCommand(0b00100000); // 4 bit mode again
	delay_ms(5);
	
	LCD_WriteCommand(0b00001100); // display on, cursor off, blink off
	delay_ms(5);

	LCD_WriteCommand(0x01); 	  // display clear
	delay_ms(5);

	LCD_WriteCommand(0x06); 	  // Cursor moves right, no display shift.  
	delay_ms(5);

	LCD_WriteCommand(0b00000010); // Returns to home. Cursor moves to starting point.
	delay_ms(5);
}

void LCD_TextDisplayString(unsigned char line, char* string)
{  
	unsigned char addr,i; 
	if(line==1) 
		addr=0x80; //The first line address
	else if(line==2) 
		addr=0x90; //The second line address
	else if(line==3) 
		addr=0x88;   //The third line address
	else if(line==4) 
		addr=0x98;    //The fourth line address 

	LCD_WriteCommand(addr); 

	for(i=0;i<16;i++) 
		LCD_WriteData(*string++); 
} 

void LCD_EnableGraphics(void)
{
	LCD_WriteCommand(0x20);		
	delay_ms(1);
	LCD_WriteCommand(0x24);			// Switch to extended instruction mode.	
	delay_ms(1);
	LCD_WriteCommand(0x26);			// Enable graphics mode.
	delay_ms(1);
}

void LCD_DisableGraphics(void)
{
	LCD_WriteCommand(0x20);			// Graphics and extended instruction mode turned off.
	delay_ms(1);
}

void LCD_FillScreenGraphics(const unsigned char* graphic)
{
	unsigned char x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)							// Draws top half of the screen.
			{												// In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				LCD_WriteCommand(0x80 | y);					// Vertical coordinate of the screen is specified first. (0-31)
    			LCD_WriteCommand(0x80 | x);					// Then horizontal coordinate of the screen is specified. (0-8)
				LCD_WriteData(graphic[2*x + 16*y]);			// Data to the upper byte is sent to the coordinate.
				LCD_WriteData(graphic[2*x+1 + 16*y]);		// Data to the lower byte is sent to the coordinate.
			}
		}
		else
		{
			for(x = 0; x < 8; x++)							// Draws bottom half of the screen.
			{												// Actions performed as same as the upper half screen.
				LCD_WriteCommand(0x80 | (y-32));			// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
    			LCD_WriteCommand(0x88 | x);
				LCD_WriteData(graphic[2*x + 16*y]);
				LCD_WriteData(graphic[2*x+1 + 16*y]);
			}
		}
		
	}
}

void LCD_GraphicsDisplayString(unsigned char line, char* str)
{
	// Output text to the LCD in graphics mode using a 5x7 font
	// Each char occupies 6 cols, 8 lines, including of space to next char
	unsigned char row;
	unsigned char colInd;
	unsigned char shouldContinue = 1;
	unsigned char count = 0;

	while(*str && shouldContinue)
	{
		unsigned char letterA = *str++;
		if (letterA == 0)
		{
			letterA = 32;
			shouldContinue = 0;
		}

		// if string length is odd, last letter does not come in pair, append space			
		unsigned char letterB = *str++;
		if (letterB == 0) 
		{
			letterB = 32; // odd number of characters in a string, replace NULL with space		
			shouldContinue = 0;
		}

		unsigned char indA = letterA < 0x52 ? letterA - 0x20 : letterA - 0x52;
		unsigned char indB = letterB < 0x52 ? letterB - 0x20 : letterB - 0x52;
		unsigned char colListA[5];
		unsigned char colListB[5];
	
	    if(letterA < 0x52){
			colListA[4] = Alpha1[(indA*5)];
			colListA[3] = Alpha1[(indA*5)+1];
			colListA[2] = Alpha1[(indA*5)+2];
			colListA[1] = Alpha1[(indA*5)+3];
			colListA[0] = Alpha1[(indA*5)+4];
		}
		else
		{
			colListA[4] = Alpha2[(indA*5)];
			colListA[3] = Alpha2[(indA*5)+1];
			colListA[2] = Alpha2[(indA*5)+2];
			colListA[1] = Alpha2[(indA*5)+3];
			colListA[0] = Alpha2[(indA*5)+4];
		}
	
	    if(letterB < 0x52){
			colListB[4] = Alpha1[(indB*5)];
			colListB[3] = Alpha1[(indB*5)+1];
			colListB[2] = Alpha1[(indB*5)+2];
			colListB[1] = Alpha1[(indB*5)+3];
			colListB[0] = Alpha1[(indB*5)+4];
		}
		else
		{
			colListB[4] = Alpha2[(indB*5)];
			colListB[3] = Alpha2[(indB*5)+1];
			colListB[2] = Alpha2[(indB*5)+2];
			colListB[1] = Alpha2[(indB*5)+3];
			colListB[0] = Alpha2[(indB*5)+4];
		}

		for (row=0;row<8;row++)
		{
			if (line < 4)	// first half
			{
				LCD_WriteCommand(0x80 | (line * 8 + row));
				LCD_WriteCommand(0x80 | count);
			}
			else
			{
				LCD_WriteCommand(0x80 | ( (line-4) * 8 + row));
				LCD_WriteCommand(0x88 | count);
			}
					
			unsigned char dataA = 0x00;
			for (colInd=0;colInd<5;colInd++)
			{
				if (colListA[colInd] & (1 << row))
				{
					dataA = dataA | (1 << (colInd+3));
				}
			}

			unsigned char dataB = 0x00;
			for (colInd=0;colInd<5;colInd++)
			{
				if (colListB[colInd] & (1 << row))
				{
					dataB = dataB | (1 << (colInd+3));
				}
			}
	
			LCD_WriteData(dataA);
			LCD_WriteData(dataB);
		}

		count++;
	}
}

void LCD_ClearGraphics(void)
{
	// This function performs similar to the LCD_FillScreenGraphics but
	// only zeros are sent into the screen instead of data from an array.
	unsigned char x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			LCD_WriteCommand(0x80 | y);
    		LCD_WriteCommand(0x80);
		}
		else
		{
			LCD_WriteCommand(0x80 | (y-32));
    		LCD_WriteCommand(0x88);
		}
		for(x = 0; x < 8; x++)
		{
			LCD_WriteData(0x00);
			LCD_WriteData(0x00);
		}
	}
}


void LCD_WriteByte(unsigned char command)
{
    LCD_EN = 1;
	delay_us(20);
	// Move higher nibble first
	LCD_DB4 = (command & (1 << 4)) >> 4;
    LCD_DB5 = (command & (1 << 5)) >> 5;
    LCD_DB6 = (command & (1 << 6)) >> 6;
    LCD_DB7 = (command & (1 << 7)) >> 7;
	delay_us(20);
	LCD_EN = 0;

	LCD_EN = 1;
	delay_us(20); 
	// Then move the lower nibble
    LCD_DB4 = (command & (1 << 0)) >> 0;
    LCD_DB5 = (command & (1 << 1)) >> 1;
    LCD_DB6 = (command & (1 << 2)) >> 2;
    LCD_DB7 = (command & (1 << 3)) >> 3;
	delay_us(20);
    LCD_EN = 0;

	delay_us(20);					// Please increase this delay if the screen is not showing things correctly.
}

void LCD_WriteData(unsigned char data) //write data
{  
	LCD_RS=1; 
	LCD_WriteByte(data);
} 

void LCD_WriteCommand(unsigned char command) //write command
{ 
	LCD_RS=0; 
	LCD_WriteByte(command);
}
