CC = gcc
CFLAGS = 
SOURCES = a b c
INCLUDES = 
LIBS = 
OUTPUT = testInterpolation

.PHONY: pre_build build post_build run

pre_build:
	@echo "Pre-build phase"

build: pre_build
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) $(SOURCES) $(LIBS)

post_build: build
	@echo "Post-build phase"

run: post_build
	@./$(OUTPUT)

