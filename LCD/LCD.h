#ifndef LCD_H
#define LCD_H

#define PORT_P0	0
#define PORT_P1	1
#define PORT_P2	2
#define PORT_P3 3
#define PORT_P4 4
#define PORT_P5 5
#define PORT_P6 6
#define PORT_P7 7

#define LCD_CLEAR	0x01

#define LCD_E	(1 << PORT_P0)
#define LCD_RS	(1 << PORT_P1)

#define LCD_E_ON	LCD_E
#define LCD_E_OFF	~LCD_E

#define LCD_RS_D	LCD_RS
#define LCD_RS_I	(LCD_RS ^ LCD_RS)


define

#endif // !LCD_H
