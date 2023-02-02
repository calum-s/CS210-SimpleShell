all:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -fsanitize=address,undefined ./src/shell.c ./src/token.c -o build/shell

clean:
	rm -rf build

test:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -fsanitize=address,undefined ./src/test.c ./src/token.c -o build/test
	./build/test

run:
	./build/shell

unsafe:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion ./src/shell.c -o build/shell

