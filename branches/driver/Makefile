SRCNAME=driver
MODEL=atmega8

all: clean compile buildhex 

compile:
	avr-gcc -Os -O1 -O2 -O3 -mmcu=$(MODEL) -o $(SRCNAME).o $(SRCNAME).c

buildhex:
	avr-objcopy -O ihex $(SRCNAME).o $(SRCNAME).hex
	du $(SRCNAME).hex -b

cp: all
	cp $(SRCNAME).hex /media/CEDC-A265/

clean:
	rm ./*.o -f
	rm ./*.hex -f

