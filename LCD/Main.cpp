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
	std::string input;

	while (true)
	{	
		system("cls");
		myLCD.lcdClear();
		myLCD << "LCD Scrolling:";
		std::getline(std::cin, input);
		myLCD >> input.c_str();
	}
	return 0;
	
}
