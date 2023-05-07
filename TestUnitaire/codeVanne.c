#include <stdio.h>
#include <wiringPi.h>

int main(void)
{
	//Initialise la bibliothèque WiringPi
	if(wiringPiSetup() == -1)
	{
		return 1;
	}
	
	//configure la broche 1 en sortie
	pinMode(1, OUTPUT);
	
	while(1)
	{
		//Active l'électrovanne (GPIO à niveau haut)
		digitalWrite(1, HIGH);
		delay(1000);
		//Désactive l'électrovanne (GPIO à niveau bas)
		digitalWrite(1, LOW);
		delay(1000);
	}
	
	return 0;
}
