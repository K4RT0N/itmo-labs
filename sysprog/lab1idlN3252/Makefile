PROGRAM_NAME=lab1idlN3252
TEST_DIR=./test

.PHONY: test

all:
	gcc -Wall -Wextra -O3 -fanalyzer -o $(PROGRAM_NAME) main.c args.c messages.c target.c file_walk.c debug.c
clean:
	rm $(PROGRAM_NAME)
test:
	./$(PROGRAM_NAME) $(TEST_DIR) 0xc0ffee
	./$(PROGRAM_NAME) $(TEST_DIR) 0x123456
	./$(PROGRAM_NAME) $(TEST_DIR) 0x00
