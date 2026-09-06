IUP_INCLUDE = ./iup/include
IUP_LIB = ./iup/lib/MacOS265

CC = gcc

ARCH = $(shell uname -m)
ifeq ($(ARCH),arm64)
    ARCH_FLAGS = -arch arm64
else
    ARCH_FLAGS = -arch x86_64
endif


GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0 2>/dev/null)

CFLAGS = -Wall -g $(ARCH_FLAGS) -I$(IUP_INCLUDE) $(GTK_CFLAGS)
SRC = main.c gui.c image.c process.c
OBJ = $(SRC:.c=.o)
TARGET = Image_Editor

$(TARGET): $(OBJ)
	$(CC) $(ARCH_FLAGS) $(OBJ) -o $(TARGET) $(IUP_LIB)/libiup.a $(GTK_LIBS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)