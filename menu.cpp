#include "declar.h"
#include "menu.h"
#include <fstream>

//menu is initially responsible for the start menu settings, which allows the player to start the game, change difficulties, look at the bestiary, and edit settings
//menu also works when "state" is listed as pause or is anything else but "game"

void drawMenuTitle() {
    mvprintw(1, 4, "ASCII GAME");
    mvprintw(2, 4, "----------");
}

string showMainMenu() {
    timeout(-1);

    while (true) {
        erase();
        drawMenuTitle();
        mvprintw(5, 4, "1. Start game");
        mvprintw(6, 4, "2. Bestiary");
        mvprintw(7, 4, "3. Quit");
        mvprintw(10, 4, "Choose an option.");
        refresh();

        int c = getch();

        if (c == '1' || c == '\n') {
            timeout(200);
            return "start";
        }

        if (c == '2') {
            showBestiary();
        }

        if (c == '3' || c == 27) {
            return "quit";
        }
    }
}

void showBestiary() {
    erase();
    drawMenuTitle();

    ifstream bestiaryFile("bestiary.txt");
    string line;
    int row = 5;

    if (bestiaryFile.is_open()) {
        while (getline(bestiaryFile, line) && row < 16) {
            mvprintw(row, 4, "%s", line.c_str());
            row++;
        }
    } else {
        mvprintw(5, 4, "Could not open bestiary.txt");
    }

    mvprintw(16, 4, "Press any key to go back.");
    refresh();
    getch();
}

string showEndMenu(string result) {
    timeout(-1);

    while (true) {
        erase();
        drawMenuTitle();

        if (result == "win") {
            mvprintw(5, 4, "You beat all 6 levels!");
        } else {
            mvprintw(5, 4, "Run ended.");
        }

        mvprintw(7, 4, "1. Play again");
        mvprintw(8, 4, "2. Main menu");
        mvprintw(9, 4, "3. Quit");
        refresh();

        int c = getch();

        if (c == '1' || c == '\n') {
            timeout(200);
            return "restart";
        }

        if (c == '2') {
            return "menu";
        }

        if (c == '3' || c == 27) {
            return "quit";
        }
    }
}
