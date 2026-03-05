main.o: main.cpp declar.h
	g++ -c main.cpp
menu.o: menu.cpp declar.h
	g++ -c menu.cpp
main: main.o menu.o
	g++ main.o menu.o -o main

clean:
	rm -f main.exe
.PHONY: clean