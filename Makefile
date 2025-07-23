OBJS = src/main.c
OUT = build/mb.exe
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
COMPILER_FLAGS = -w
CC = g++

CLEAR_SCREEN = clear

all : COMPILE CLEAR
	./$(OUT)

CLEAR :
	$(CLEAR_SCREEN)

COMPILE : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUT)
