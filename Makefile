CXX := g++
CXXFLAGS := -g

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDLIBS := -lncurses
else
LDLIBS := -lncursesw
endif

main.o: main.cpp declar.h upgrades.h director.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

menu.o: menu.cpp declar.h
	$(CXX) $(CXXFLAGS) -c menu.cpp -o menu.o

upgrades.o: upgrades.cpp upgrades.h declar.h
	$(CXX) $(CXXFLAGS) -c upgrades.cpp -o upgrades.o

director.o: director.cpp director.h declar.h
	$(CXX) $(CXXFLAGS) -c director.cpp -o director.o

enemies.o: enemies.cpp enemies.h declar.h
	$(CXX) $(CXXFLAGS) -c enemies.cpp -o enemies.o

main.exe: main.o menu.o upgrades.o director.o
	$(CXX) $(CXXFLAGS) main.o menu.o upgrades.o director.o -o main.exe $(LDLIBS)

ifeq ($(UNAME_S),Darwin)
main: main.o menu.o upgrades.o director.o
	$(CXX) $(CXXFLAGS) main.o menu.o upgrades.o director.o -o main $(LDLIBS)
else
main: main.exe
endif

clean:
ifeq ($(UNAME_S),Darwin)
	rm -f main main.exe main.o menu.o upgrades.o director.o
else
	-del /Q main.exe main.o menu.o upgrades.o director.o 2>nul
endif

.PHONY: clean main
