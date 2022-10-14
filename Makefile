all: first

first: first.c
	gcc -lm -ldl -Wall -Werror -fsanitize=address first.c -o first

clean: 
	rm -rf first
