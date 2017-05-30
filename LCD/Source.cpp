#include<cstdio>
#include<windows.h>

#define FTD2XX_EXPORTS
#include "ftd2xx.h"

int main()
{

	FT_HANDLE lcdHandle;
	unsigned char info = 0x00;
	DWORD sizeSent = 0;
	bool found = false;

	for (int i = 0; (i < 10) && !found; i++)
	{
		if (FT_Open(i, &lcdHandle) == FT_OK)	//Examples in FTDI guid use 0. But practice shows 1 is usually the case.
		{
			found = true;
			UCHAR Mask = 0xFF;	//Selects all FTDI pins.
			UCHAR Mode = 1; 	// Set asynchronous bit-bang mode
			if (FT_SetBitMode(lcdHandle, Mask, Mode) == FT_OK)	// Sets LCD as asynch bit mode. Otherwise it doesn't work.
			{

				//Examplo to write 0xf0 to the display 

				//(E=0, RS=0, D4-D7=f)...
				info = 0xf0;

				//Finally executes the action "write to LCD"...
				if (FT_Write(lcdHandle, &info, 1, &sizeSent) == FT_OK)
				{
					//If success continue with the program (...)

				}
				else
					printf("Error writing to the LCD\n");
			}
			else
				printf("Couldn't configure LCD\n");

			FT_Close(lcdHandle);
		}
		else
			printf("Couldn't open USB %d\n", i);
	}
	return 0;
}
