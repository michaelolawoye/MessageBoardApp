CC = g++
SOURCE_FILE = src/main.cpp
COMPILER_FLAGS = 
LINKER_FLAGS = -lSDL3 -lSDL3_image -lSDL3_ttf
OUTPUT = build/main_out.exe

all : CREATE_OUT
	./${OUTPUT}

CLEAR :
	TERM=xterm clear

CREATE_OUT: ${SOURCE_FILE}
	${CC} ${SOURCE_FILE} ${COMPILER_FLAGS} ${LINKER_FLAGS} -o ${OUTPUT}
