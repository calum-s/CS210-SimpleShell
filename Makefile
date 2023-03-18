
sources:= src/token.c src/command.c src/builtin.c src/file.c src/alias.c

all: format
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -Wpedantic -DCONFORMANT -fsanitize=address,undefined src/shell.c $(sources) -o build/shell

clean: format
	rm -rf build

test:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -Wpedantic -fsanitize=address,undefined src/test.c $(sources) -o build/test
	./build/test

run:
	./build/shell

unsafe: format
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -Wpedantic ./src/shell.c $(sources) -o build/shell

demo: format
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -Wpedantic -fsanitize=address,undefined src/shell.c $(sources) -o build/shell

format:
	clang-format -i src/*.c src/*.h

ci:
	mkdir -p build
	clang -g -Wall -Wextra -Wconversion -Wpedantic -DCONFORMANT -fsanitize=address,undefined src/shell.c $(sources) -o build/shell
