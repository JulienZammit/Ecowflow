all : exeTemperature exeDebit exeBuzzer exeLCD exeVanne

exeTemperature : codeTemperature.c
	gcc -o exeTemperature codeTemperature.c -lwiringPi -lwiringPiDev
	
exeDebit : codeDebit.c
	gcc -o exeDebit codeDebit.c -lwiringPi -lwiringPiDev
	
exeBuzzer : codeBuzzer.c
	gcc -o exeBuzzer codeBuzzer.c -lwiringPi -lwiringPiDev
	
exeLCD : codeLCD.c
	gcc -o exeLCD codeLCD.c -lwiringPi -lwiringPiDev
	
exeVanne : codeVanne.c
	gcc -o exeVanne codeVanne.c -lwiringPi -lwiringPiDev
	
clean : 
	rm exeTemperature
	rm exeDebit
	rm exeBuzzer
	rm exeLCD
	rm exeVanne
