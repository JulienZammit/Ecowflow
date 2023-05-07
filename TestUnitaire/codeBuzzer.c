#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>
 
#define BuzzPin 0
 
#define CM2 294
#define CM3 330
#define CM5 393
#define CM6 441
 
int son[] = {CM3,CM5,CM6,CM3,CM2,CM3};
 
int beat[] = {1,1,3,1,1,3};
 
int main(void)
{ 
    if(wiringPiSetup() == -1)
    { 
        printf("WiringPi initialization failed !");
        return 1;
    }
    
    if(softToneCreate(BuzzPin) == -1)
    {
        printf("Soft Tone Failed !");
        return 1;
    }
    
    printf("Génération du son...\n");
    
    for(int i = 0; i < sizeof(son) / 4; i++)
    {
        softToneWrite(BuzzPin, son[i]);
        delay(beat[i] * 500);
    }
    
    return 0;
}
