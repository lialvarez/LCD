#include "HitachiLCD.h"
#include <chrono>
#include <thread>

HitachiLCD::HitachiLCD()
{
	bool found = false;
	for (int i = 0; (i < 10) && !found; i++)
	{
		lcdStatus = FT_Open(i, &lcdHandler);	//Intenta abrir el LCD. Si abre devuelve FT_OK
		if (lcdStatus == FT_OK)
		{
			found = true;	//Setea el flag en true. Dispositivo hallado
			UCHAR Mask = 0xFF;	//Selects all FTDI pins.
			UCHAR Mode = 1;
			lcdStatus = FT_SetBitMode(lcdHandler, Mask, Mode);	//Setea el FTDI en "asynchronmous bit-bang mode"
		}
		deviceNumber = i;	//Numero que identifica al dispositivo FTDI
	}
}


HitachiLCD::~HitachiLCD()
{
	FT_Close(&lcdHandler);

	//TODO: liberar la memoria que sea necesaria
}

bool HitachiLCD::lcdInitOk()
{
	if (lcdStatus == FT_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

FT_STATUS HitachiLCD::lcdGetError()
{
	return lcdStatus;
}

bool HitachiLCD::lcdWriteNibble(UCHAR value, UCHAR RS)
{
	bool ret = false;
	char buffer[1];
	DWORD bytesSent = 0;
	buffer[0] = (value << 4) & 0xF0;
	buffer[0] |= RS;

	//Seteo:
	//E = 0
	//RS = 1
	//nibble = value

	if (FT_Write(&lcdHandler, buffer, 1, &bytesSent) == FT_OK)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		buffer[0] |= LCD_E_ON;

		//Seteo:
		//E = 1
		//RS = 1
		//nibble = value

		if (FT_Write(&lcdHandler, buffer, 1, &bytesSent) == FT_OK)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			buffer[0] &= LCD_E_OFF;
			if (FT_Write(&lcdHandler, buffer, &bytesSent) == FT_OK)
			{
				ret = true;
			}
		}
	}
	return ret;
}

bool HitachiLCD::lcdWriteByte(UCHAR value, UCHAR RS)
{
	bool ret = false;
	if (lcdWriteNibble(value << 4, RS))
	{
		if (lcdWriteNibble(value & 0x0F, RS))
		{
			ret = true;
		}
	}
	return ret;
}

void HitachiLCD::lcdUpdateCursor()
{
	if (cadd - 1 < MAX_COLS)
	{
		lcdWriteByte(FIRST_LINE + cadd - 1, LCD_RS_I);
	}
	else if (cadd - 1 < MAX_ROWS * MAX_COLS)
	{
		lcdWriteByte(SECOND_LINE + cadd - 1, LCD_RS_I);
	}
}

bool HitachiLCD::lcdSetCursorPosition(const cursorPosition pos)
{
	bool ret = false;
	if ((pos.column >= 0 && pos.column < MAX_COLS) && (pos.row >= 0 && pos.row < MAX_ROWS))
	{
		cadd = pos.row * MAX_ROWS + pos.column + 1;
		lcdUpdateCursor();
		ret = true;
	}
	return ret;
}

bool HitachiLCD::lcdClear()
{
	bool ret = false;
	cursorPosition newPos;
	newPos.column = 0;
	newPos.row = 0;
	if (lcdWriteByte(LCD_CLEAR, LCD_RS_I))
	{
		if (lcdSetCursorPosition(newPos))
		{
			ret = true;
		}
	}
	return ret;
}

bool HitachiLCD::lcdClearToEOL()
{
	bool ret = true;
	if (cadd - 1 < MAX_COLS)
	{
		for (int i = cadd - 1; i < MAX_COLS; i++)
		{
			if (!lcdWriteByte(' ', LCD_RS_D))
			{
				ret = false;
			}
		}
	}
	else
	{
		for (int i = cadd - 16 - 1; i < MAX_COLS; i++)
		{
			if (!lcdWriteByte(' ', LCD_RS_D))
			{
				ret = false;
			}
		}
	}
	return ret;
}


