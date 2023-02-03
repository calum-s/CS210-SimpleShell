
sources:= src/token.c src/command.c

all:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -fsanitize=address,undefined src/shell.c $(sources) -o build/shell

clean:
	rm -rf build

test:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -fsanitize=address,undefined src/test.c $(sources) -o build/test
	./build/test

run:
	./build/shell

unsafe:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion ./src/shell.c -o build/shell
