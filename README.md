# EscapeRoom_Group13
CISC 320 Group 13 team project: Escape room game developed in C++

## Gael Jail

Point and click escape rooms set around campus. Pick a room on the home page and get out before
the timer runs out. When the screen turns red the Dean is coming, so click the glowing hiding spot.
Finish faster for more stars, and log in from Profile to keep your best times.
Right now the Library (Stauffer) is done. See `TEAM_GUIDE.md` to add a room.

Uses SFML 3.

## Setting it up in Visual Studio

1. Set up SFML 3 like in the SFML setup guide (x64, include and lib folders, `sfml-graphics`,
   `sfml-window`, `sfml-system`, and the DLLs next to the exe).
2. Add all the files in `src/` and `src/maps/` to the project.
3. Project properties > C/C++ > Language > C++ Language Standard: ISO C++17. SFML 3 needs it.
4. Leave the working directory as `$(ProjectDir)` and keep `assets/` in the project folder or
   above it.

## On a Mac

Install SFML 3 with Homebrew, then build from the project folder:

```
brew install sfml
g++ -std=c++17 src/*.cpp src/maps/*.cpp -I$(brew --prefix)/include -L$(brew --prefix)/lib -lsfml-graphics -lsfml-window -lsfml-system -o gaeljail
./gaeljail
```

## Controls

- Click things to look at them or pick them up
- Click an item at the bottom, then click something in the room to use it
- H for a hint, D to see the hotspot boxes and mouse position
- Home (top right) or Esc to go back to the home page
- Keypads work with the mouse or the number keys and Enter

The answers for the Library are in `WALKTHROUGH.md`.

## Files

```
src/main.cpp           starts the game
src/Game.cpp           main loop, home page, drawing, timer, the Dean
src/Profile.cpp        login, stars and best times, saved to <name>.gaeljail
src/Map.cpp            Map class that every room inherits from
src/Keypad.cpp         keypad popup
src/Draw.cpp           drawing helpers
src/maps/MapList.cpp   list of rooms in the menu
src/maps/StaufferMap   the Library
assets/                pictures (credits in assets/CREDITS.txt)
```
