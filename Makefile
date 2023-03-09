
sources:= src/token.c src/command.c src/builtin.c src/file.c

all:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -DCONFORMANT -fsanitize=address,undefined src/shell.c $(sources) -o build/shell

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
	clang -g -Wall -Wextra -Wconversion ./src/shell.c $(sources) -o build/shell

demo:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -fsanitize=address,undefined src/shell.c $(sources) -o build/shell
