#include "declar.h"
#include "menu.h"
#include <fstream>

//menu is initially responsible for the start menu settings, which allows the player to start the game, change difficulties, look at the bestiary, and edit settings
//menu also works when "state" is listed as pause or is anything else but "game"

void drawMenuTitle() {
    mvprintw(1, 4, "ASCII KNIGHT");
    mvprintw(2, 4, "----------");
}

string difficultyName(int diff) {
    if (diff == 0) return "Easy";
    if (diff == 1) return "Normal";
    if (diff == 2) return "Hard";

    return "Unknown";
}

void changeDifficulty(gamestate &settings) {
    settings.diff++;

    if (settings.diff > 2) {
        settings.diff = 0;
    }
}

string showMainMenu(gamestate &settings) {
    timeout(-1);

    while (true) {
        erase();
        drawMenuTitle();
        mvprintw(5, 4, "1. Start game");
        mvprintw(6, 4, "2. Bestiary");
        mvprintw(7, 4, "3. Difficulty: %s", difficultyName(settings.diff).c_str());
        mvprintw(8, 4, "4. Quit");
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

        if (c == '3') {
            changeDifficulty(settings);
        }

        if (c == '4' || c == 27) {
            return "quit";
        }
    }
}

void showBestiary() {
    timeout(-1);

    ifstream bestiaryFile("bestiary.txt");
    vector<string> lines;
    string line;

    if (bestiaryFile.is_open()) {
        while (getline(bestiaryFile, line)) {
            lines.push_back(line);
        }
    } else {
        lines.push_back("Could not open bestiary.txt");
    }

    int topLine = 0;
    int visibleLines = 12;

    while (true) {
        erase();
        drawMenuTitle();
        mvprintw(3, 4, "Bestiary");

        int lastShown = topLine + visibleLines;
        if (lastShown > (int)lines.size()) {
            lastShown = lines.size();
        }

        if ((int)lines.size() > visibleLines) {
            mvprintw(3, 24, "Lines %d-%d/%d", topLine + 1, lastShown, (int)lines.size());
        }

        for (int i = 0; i < visibleLines; i++) {
            int lineIndex = topLine + i;

            if (lineIndex >= (int)lines.size()) {
                break;
            }

            mvprintw(5 + i, 4, "%s", lines[lineIndex].c_str());
        }

        mvprintw(18, 4, "W/S scroll  A/D page  ESC back");
        refresh();

        int c = tolower(getch());

        if (c == 27) {
            return;
        }

        if (c == 'w' && topLine > 0) {
            topLine--;
        }

        if (c == 's' && topLine + visibleLines < (int)lines.size()) {
            topLine++;
        }
    }
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
