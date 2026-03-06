CXX := g++
CXXFLAGS := -g
LDLIBS := -lncursesw

main.o: main.cpp declar.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

menu.o: menu.cpp declar.h
	$(CXX) $(CXXFLAGS) -c menu.cpp -o menu.o

main.exe: main.o menu.o
	$(CXX) $(CXXFLAGS) main.o menu.o -o main.exe $(LDLIBS)

main: main.exe

clean:
	-del /Q main.exe main.o menu.o 2>nul

.PHONY: clean main