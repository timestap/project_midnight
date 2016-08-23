mainmake: main.c parser.c
	cc -std=c99 -g -O0 -Wall main.c mpc.c parser.c gc.c error_handler.c vars.c func.c -ledit -lm -o main