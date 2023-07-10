build:
	g++ main.c -o main
run: build
	./main
debug:
	g++ main.c -o main -g
	gdb main
