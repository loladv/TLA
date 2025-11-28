CC = gcc
CFLAGS = -Wall -g
SOURCES = main.c
INCLUDES = 
LIBS = 
OUTPUT = test_custom_phase_conditional

.PHONY: pre_build build post_build run

pre_build:
	@mkdir build

build: pre_build
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUT) $(SOURCES) $(LIBS)

post_build: build
	@cp main.c output.c

run: post_build
	@./$(OUTPUT)

.PHONY: if_fail_setup
if_fail_setup:
	@if [ $$? -ne 0 ]; then \
		@rm -rf build; \
		@rm -rf logs; \
	fi

.PHONY: if_success_cleanup
if_success_cleanup:
	@if [ $$? -eq 0 ]; then \
		@cp main.c backup.c; \
	fi

.PHONY: if_success_post_build
if_success_post_build:
	@if [ $$? -eq 0 ]; then \
		@mv output.c final.c; \
	fi

.PHONY: setup
setup:
	@mkdir builds
	@mkdir logs

.PHONY: cleanup
cleanup:
	@rm -rf temp/
	@rm -f *.o

