# Makefile for CS 453
# type "make" to compile the following files

SRC = badPixels.cpp

IMGLIB = /home/schar/cs453/imageLib
ARCH := $(shell arch)

CC = g++
CPPFLAGS = -O2 -W -Wall -I$(IMGLIB)
LDLIBS = -L$(IMGLIB) -lImg.$(ARCH) -lpng -lz

BIN = $(SRC:.cpp=)

all: $(BIN)

clean: 
	rm -f $(BIN)
