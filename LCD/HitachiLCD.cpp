#include "HitachiLCD.h"
#include <chrono>
#include <thread>
#include <string>

#define FTD2XX_EXPORTS

HitachiLCD::HitachiLCD(){}

HitachiLCD::~HitachiLCD()
{
	FT_Close(&deviceHandler);

	//TODO: liberar la memoria que sea necesaria
}

FT_HANDLE * HitachiLCD::lcdInit(int iDevice)
{
	bool status = true;
	lcdStatus = FT_Open(iDevice, &deviceHandler);	//Intenta abrir el LCD. Si abre devuelve FT_OK
	if (lcdStatus == FT_OK)
	{
		UCHAR Mask = 0xFF;	//Selects all FTDI pins.
		UCHAR Mode = 1;
		lcdStatus = FT_SetBitMode(deviceHandler, Mask, Mode);	//Setea el FTDI en "asynchronmous bit-bang mode"

		status &= lcdWriteNibble((FUNCTION_SET_8 & 0xF0) >> 8, LCD_RS_I);
		std::this_thread::sleep_for(std::chrono::milliseconds(4));
		status &= lcdWriteNibble((FUNCTION_SET_8 & 0xF0) >> 8, LCD_RS_I);
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		status &= lcdWriteNibble((FUNCTION_SET_8 & 0xF0) >> 8, LCD_RS_I);
		status &= lcdWriteNibble((FUNCTION_SET_4 & 0xF0) >> 8, LCD_RS_I);
		status &= lcdWriteByte(FUNCTION_SET_4, LCD_RS_I);
		status &= lcdWriteByte(DISPLAY_CONTROL, LCD_RS_I);
		status &= lcdWriteByte(LCD_CLEAR, LCD_RS_I);
		status &= lcdWriteByte(ENTRY_MODE_SET, LCD_RS_I);
		if (!status)
		{
			lcdStatus = FT_OTHER_ERROR; //Si no puede setear en modo 4 bits, setea lcdStatus en Error.
		}
		return &deviceHandler;
	}
	return nullptr;	//Si no puede abrir el dispositivo devuelve nullptr.
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

	if (FT_Write(deviceHandler, buffer, 1, &bytesSent) == FT_OK)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		buffer[0] |= LCD_E_ON;

		//Seteo:
		//E = 1
		//RS = 1
		//nibble = value

		if (FT_Write(deviceHandler, buffer, 1, &bytesSent) == FT_OK)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(500));

			buffer[0] &= LCD_E_OFF;
			if (FT_Write(deviceHandler, buffer, 1, &bytesSent) == FT_OK)
			{
				ret = true;
			}
		}
	}
	return ret;
}

bool HitachiLCD::lcdWriteByte(UCHAR value, UCHAR RS)
{
	bool ret = true;
	ret &= lcdWriteNibble((value & 0xF0) >> 4, RS);
	ret &= lcdWriteNibble(value & 0x0F, RS);

	return ret;
}

void HitachiLCD::lcdWriteDR(UCHAR valor)
{
	if (lcdWriteByte(valor, LCD_RS_D) && (cadd - 1) < MAX_COLS*MAX_ROWS)
	{
		cadd++;
		lcdUpdateCursor();
	}
}

void HitachiLCD::lcdWriteIR(UCHAR valor)
{
	lcdWriteByte(valor, LCD_RS_I);
}

bool HitachiLCD::lcdClear()
{
	bool ret = false;
	if (lcdWriteByte(LCD_CLEAR, LCD_RS_I))
	{
		if (lcdWriteByte(RETURN_HOME, LCD_RS_I))
		{
			cadd = 1;
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
	lcdUpdateCursor();
	return ret;
}

void HitachiLCD::lcdUpdateCursor()
{
	if (cadd - 1 < MAX_COLS)
	{
		lcdWriteIR((FIRST_LINE + cadd - 1) | 0x80);
	}
	else if (cadd - 1 < MAX_ROWS * MAX_COLS)
	{
		lcdWriteIR((SECOND_LINE + cadd - 1 - MAX_COLS) | 0x80);
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

bool HitachiLCD::lcdMoveCursorUp()
{
	bool ret = false;
	if (cadd - 1 > MAX_COLS)
	{
		cadd -= 16;
		lcdUpdateCursor();
		ret = true;
	}
	return ret;
}

bool HitachiLCD::lcdMoveCursorDown()
{
	bool ret = false;
	if (cadd - 1 < MAX_COLS)
	{
		cadd += 16;
		lcdUpdateCursor();
		ret = true;
	}
	return ret;
}

bool HitachiLCD::lcdMoveCursorRight()
{
	bool ret = false;
	if (cadd < MAX_COLS * MAX_ROWS)
	{
		cadd++;
		lcdUpdateCursor();
		ret = true;
	}
	return ret;
}

bool HitachiLCD::lcdMoveCursorLeft()
{
	bool ret = false;
	if (cadd - 1 > 0)
	{
		cadd--;
		lcdUpdateCursor();
		ret = true;
	}
	return ret;
}

cursorPosition HitachiLCD::lcdGetCursorPosition()
{
	cursorPosition ret;
	ret.column = (cadd - 1) / 16;
	if (cadd - 1 < MAX_COLS)
	{
		ret.row = cadd - 1;
	}
	else
	{
		ret.row = cadd - 1 - MAX_COLS;
	}
	return ret;
}

basicLCD& HitachiLCD::operator<<(const unsigned char c)
{
	lcdWriteDR(c);
	return *this;
}

basicLCD& HitachiLCD::operator<<(const char *c)
{
	int i = 0;
	//Primero cuento los caracteres de la cadena
	while (c[i] != '\0')
	{
		i++;
	}
	//Si son mas caracteres que "espacios libres" imprimo solo los que entren en el display(los ultimos)
	if (i > 32 - cadd)
	{
		for (int j = i + 1 - 32 - cadd; j <= i + 1; j++)
		{
			lcdWriteDR(c[j]);
		}
	}
	//Si entran todos imprimo todos
	else
	{
		i = 0;
		while (c[i] != '\0')
		{
			lcdWriteDR(c[i]);
			i++;
		}
	}
	return *this;
}

//Imprime la cadena de caracteres desde la primer columna de la linea indicada 
//Si la extension en mayor a MAX_COLS imprime los primeros caracteres
void HitachiLCD::lcdPrintFront(const char *c, int line)
{
	if (line == 1)
	{
		cadd = 1;
	}
	else if (line == 2)
	{
		cadd = 17;
	}
	int auxCadd = cadd;
	lcdUpdateCursor();
	std::string msj(c);
	for (int i = 0; i < MAX_COLS && i < msj.length(); i++)
	{
		lcdWriteDR(msj[i]);
	}
	//if (msj.length() > MAX_COLS)
	//{
	//	msj = msj.substr(0, MAX_COLS);
	//}
	
	cadd = auxCadd;
	lcdUpdateCursor();
}

void HitachiLCD::lcdScrollText(const char *c, int line, unsigned int speed)
{
	if (speed > 10)
	{
		speed = 10;
	}
	lcdUpdateCursor();
	std::string msj(c);
	std::string spaces = "               ";
	std::string aux = spaces + msj + " ";
	for (int i = 0; i < aux.length(); i++)
	{
		std::string scroll = aux.substr(i);
		lcdPrintFront(scroll.c_str(), line);
		std::this_thread::sleep_for(std::chrono::milliseconds(10*speed));
	}
}

basicLCD& HitachiLCD::operator>>(const char *c)
{
	lcdUpdateCursor();
	std::string msj(c);
	std::string spaces = "               ";
	std::string aux = spaces + msj + " ";
	for (int i = 0; i < aux.length(); i++)
	{
		std::string scroll = aux.substr(i);
		lcdPrintFront(scroll.c_str(), 2);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return *this;
}
