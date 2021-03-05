/******* Generalize code for LCD16x2 *******/

#include "NP_LCD.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* Private user code ---------------------------------------------------------*/
#define RS_Port GPIOB
#define RS_Pin GPIO_PIN_11
#define RWB_Port GPIOB
#define RWB_Pin GPIO_PIN_10
#define EN_Port GPIOB
#define EN_Pin GPIO_PIN_2
#define DATA_Port GPIOE
#define DATA_Pin GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
#define DATA_Pin_0 GPIO_PIN_0



#define LCD_MS_DELAY(X)(HAL_Delay(X))

/* List of COMMANDS */
#define LCD_CLEARDISPLAY      0x01
#define LCD_SETDDRAMADDR      0x80
#define LCD_IncrementCursor   0x06
#define LCD_CursorOff	        0x0C
#define LCD_2line5x7matrix    0x38
#define LCD_dispONcursorBlink 0x0E
#define LCD_secondline        0xC0

/* LCD Library Variables */
static uint8_t DisplayControl = 0x0F;
uint16_t dispvaluei[6];
uint8_t int_point=0;
uint16_t dispvaluei1[4];
uint16_t dispvaluef[4];
double temp=0;
uint8_t ii;

void LCD_init(void)
{

EN_Port->BSRR=(uint16_t)EN_Pin << 16U;

  for(uint8_t ii=0;ii<=200;ii++)
  delay();

  
  command(LCD_CLEARDISPLAY);
  command(LCD_IncrementCursor);
  command(LCD_dispONcursorBlink);
  command(LCD_SETDDRAMADDR);
  command(LCD_2line5x7matrix);
}
  
void command(uint8_t temp)
{
	ready();
	DATA_Port->ODR&=~((uint32_t)DATA_Pin);
	DATA_Port->ODR|=temp<<(DATA_Pin_0-1);
	RS_Port->BSRR=(uint16_t)RS_Pin << 16U;
	delay();
	RWB_Port->BSRR=(uint16_t)RWB_Pin << 16U;
	delay();
	EN_Port->BSRR=(uint16_t)EN_Pin;
	delay();
	EN_Port->BSRR=(uint16_t)EN_Pin << 16U;
}

void display(uint8_t temp)
{
	ready();
	DATA_Port->ODR&=~((uint32_t)DATA_Pin);
	DATA_Port->ODR|=temp<<(DATA_Pin_0-1);
	RS_Port->BSRR=(uint16_t)RS_Pin;
	delay();
	RWB_Port->BSRR=(uint16_t)RWB_Pin << 16U;
	delay();
	EN_Port->BSRR=(uint16_t)EN_Pin;
	delay();
	EN_Port->BSRR=(uint16_t)EN_Pin << 16U;
}

void ready(void)
{
	uint8_t i;
	for(i=0;i<=50;i++)
	delay();
}

void delay(void)
{
	uint16_t x;
	for(x=0;x<=1000;x++);
}

void LCD_setCursor(uint8_t row, uint8_t col)
{
    uint8_t maskData;
    maskData = (col)&0x0F;
    if(row==0)
  {
    maskData |= (0x80);
    command(maskData);
  }
  else
  {
    maskData |= (0xc0);
    command(maskData);
  }
}
/**
 * @brief Move to beginning of 1st line
 */
void LCD_1stLine(void)
{
  LCD_setCursor(0,0);
}
/**
 * @brief Move to beginning of 2nd line
 */
void LCD_2ndLine(void)
{
  LCD_setCursor(1,0);
}


void LCD_clear(void)
{
  command(LCD_CLEARDISPLAY);
  LCD_MS_DELAY(3);
}


void LCD_display(bool state)
{
  if(state)
  {
    DisplayControl |= (0x04);
    command(DisplayControl);
  }
  else
  {
    DisplayControl &= ~(0x04);
    command(DisplayControl);
  }
}

void LCD_printf(char stringArray[])
{
  for(uint8_t i=0;  i<strlen(stringArray); i++)
  {
    display((uint8_t)stringArray[i]);
  }
}

void LCD_data(uint32_t num)
{
	uint8_t c=1,i=1;
	
	while(num!=0)
	{
			dispvaluei[c]=(uint8_t)(((uint32_t)(num))%10);
			num/=10;
			c++;
		}
			for(i=c-1; i>0; i--)
	  {
	    display(dispvaluei[i]+0x30);
    }  
}
void LCD_floatdata(double num, uint8_t d_p)
{
	uint8_t c1=1,i1=1;
	uint16_t int_part;
	float  fract_part;

	int_part =(uint32_t)num;
	fract_part=(float)(num-int_part);
	while(int_part!=0)
	{
			dispvaluei1[c1]=(uint8_t)(((uint32_t)(int_part))%10);
			int_part/=10;
			c1++;
	}
	for(i1=c1-1;i1>0;i1--)
	{
	 display(dispvaluei1[i1]+0x30);
	}
	display('.');
	c1=0;
	for(i1=1;i1<=d_p;i1++)
  {  
	  fract_part*=10;
		dispvaluei1[i1]=(uint8_t)(((uint32_t)(fract_part))%10);	
		c1++;
	}
	for(i1=1;i1<=c1;i1++)
	{
	 display(dispvaluei1[i1]+0x30);
	}
}