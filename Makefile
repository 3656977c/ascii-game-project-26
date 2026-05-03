CXX := g++
CXXFLAGS := -g

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LDLIBS := -lncurses
else
LDLIBS := -lncurses
endif

main.o: main.cpp declar.h upgrades.h director.h enemies.h menu.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

menu.o: menu.cpp menu.h declar.h
	$(CXX) $(CXXFLAGS) -c menu.cpp -o menu.o

upgrades.o: upgrades.cpp upgrades.h declar.h
	$(CXX) $(CXXFLAGS) -c upgrades.cpp -o upgrades.o

director.o: director.cpp director.h declar.h
	$(CXX) $(CXXFLAGS) -c director.cpp -o director.o

enemies.o: enemies.cpp enemies.h declar.h
	$(CXX) $(CXXFLAGS) -c enemies.cpp -o enemies.o

main: main.o menu.o upgrades.o director.o enemies.o
	$(CXX) $(CXXFLAGS) main.o menu.o upgrades.o director.o enemies.o -o main $(LDLIBS)

clean:
	rm -f main main.exe main.o menu.o upgrades.o director.o enemies.o

.PHONY: clean main
