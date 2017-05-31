#include <cstdio>
#include <iostream>
#include <string>
#include "HitachiLCD.h"

int main()
{
	HitachiLCD myLCD;
	myLCD.lcdInit(1);
	if (!myLCD.lcdInitOk())
	{
		std::cout << "No se pudo inicializar LCD" << std::endl << "Preione enter para salir..." << std::endl;
		getchar();
		return 0;
	}
	std::string input1, input2;
	cursorPosition pos;
	pos.column = 0;
	pos.row = 0;
	while (true)
	{
		std::getline(std::cin, input1);
		std::getline(std::cin, input2);
		myLCD.lcdClear();
		myLCD.lcdPrintFront("EDA - GRUPO 3", 1);
		myLCD >> input1.c_str();
		system("cls");
	}
	
}
