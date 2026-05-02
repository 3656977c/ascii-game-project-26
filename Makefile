CXX := g++
CXXFLAGS := -g

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDLIBS := -lncurses
else
LDLIBS := -lncursesw
endif

main.o: main.cpp declar.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

menu.o: menu.cpp declar.h
	$(CXX) $(CXXFLAGS) -c menu.cpp -o menu.o

main.exe: main.o menu.o
	$(CXX) $(CXXFLAGS) main.o menu.o -o main.exe $(LDLIBS)

ifeq ($(UNAME_S),Darwin)
main: main.o menu.o
	$(CXX) $(CXXFLAGS) main.o menu.o -o main $(LDLIBS)
else
main: main.exe
endif

clean:
ifeq ($(UNAME_S),Darwin)
	rm -f main main.exe main.o menu.o
else
	-del /Q main.exe main.o menu.o 2>nul
endif

.PHONY: clean main
