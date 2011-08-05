all: clean compile link 

COMPILER = g++
LINKER = g++

COMPILER_LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui
COMPILER_INCLUDES = -I/usr/local/include/opencv/ -I./include
COMPILER_OPTIONS = $(COMPILER_INCLUDES) $(COMPILER_LIBS) -O3 -c 
LINKER_OPTIONS = $(COMPILER_LIBS)

BINARY_NAME = ./main

compile:
	$(COMPILER) ./src/main.cpp $(COMPILER_OPTIONS) -o ./lib/main.o
	$(COMPILER) ./src/ObjectsFinder.cpp $(COMPILER_OPTIONS) -o ./lib/ObjectsFinder.o 
	$(COMPILER) ./src/Errors.cpp $(COMPILER_OPTIONS) -o ./lib/Errors.o 

link:
	$(LINKER) ./lib/*.o $(LINKER_OPTIONS) -o $(BINARY_NAME)

clean:
	rm ./lib/*.o -f
	rm $(BINARY_NAME) -f
