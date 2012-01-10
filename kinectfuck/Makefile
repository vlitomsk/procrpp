all: clean compile run

clean:
	rm -f ./main

compile:
	g++ ./main.cpp -o main -I/usr/include/opencv -I/usr/local/include/libfreenect -lfreenect -lfreenect_sync -lhighgui -lcv -lcxcore

run:
	./main
