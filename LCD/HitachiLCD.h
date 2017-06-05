#ifndef HITACHILCD_H
#define HITACHILCD_H

#include <ftd2xx.h>
#include "basicLCD.h"

#define PORT_P0	0
#define PORT_P1	1
#define PORT_P2	2
#define PORT_P3 3
#define PORT_P4 4
#define PORT_P5 5
#define PORT_P6 6
#define PORT_P7 7

#define LCD_CLEAR		0x01
#define FUNCTION_SET_8	0x38
#define FUNCTION_SET_4	0x28
#define ENTRY_MODE_SET	0x06
#define RETURN_HOME		0x02

#define DISPLAY_CONTROL	0x0C

#define LCD_E	(1 << PORT_P0)
#define LCD_RS	(1 << PORT_P1)

#define LCD_E_ON	LCD_E
#define LCD_E_OFF	~LCD_E

#define LCD_RS_D	LCD_RS
#define LCD_RS_I	(LCD_RS ^ LCD_RS)


#define MAX_ROWS	2
#define	MAX_COLS	16

#define FIRST_LINE	0x00
#define SECOND_LINE	0x40

class HitachiLCD :
	public basicLCD
{
public:
	HitachiLCD();
	~HitachiLCD();

	FT_HANDLE * lcdInit(int iDevice);
	FT_STATUS lcdGetError();
	bool lcdInitOk();
	void lcdWriteIR(UCHAR valor);
	void lcdWriteDR(UCHAR valor);
	bool lcdSetCursorPosition(const cursorPosition pos);
	bool lcdClear();
	bool lcdClearToEOL();
	bool lcdMoveCursorUp();
	bool lcdMoveCursorDown();
	bool lcdMoveCursorRight();
	bool lcdMoveCursorLeft();
	cursorPosition lcdGetCursorPosition();
	basicLCD& operator<<(const unsigned char c);
	basicLCD& operator<<(const char *c);
	// Sobrecarga de operador >>
	// Scrollea el texto ingresado hacia la izquierda
	basicLCD& operator>>(const char *c);
	void lcdPrintFront(const char *c, int line);
	void lcdScrollText(const char *c, int line, unsigned int speed);
protected:
	FT_STATUS lcdStatus;
	FT_HANDLE deviceHandler;
	int deviceNumber;

	void lcdUpdateCursor();
	bool lcdWriteNibble(UCHAR value, UCHAR RS);
	bool lcdWriteByte(UCHAR value, UCHAR RS);

};


#endif // !HITACHILCD_H