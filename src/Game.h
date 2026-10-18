// Game.h
// main loop, home page, and drawing everything for the current map
#pragma once

#include "Keypad.h"
#include "Map.h"
#include "Profile.h"

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>

enum GameState { HOME, HOW_TO_PLAY, PROFILE, PLAYING, WON, LOST };

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void startMap(int index);
    void goHome();
    void saveAndQuit();

    void onMouseClick(float x, float y);
    void onKey(sf::Keyboard::Key key);
    void onText(char c);
    void clickHome(float x, float y);
    void clickProfile(float x, float y);
    void clickRoom(float x, float y);
    void clickInventory(float x, float y);
    Hotspot* hotspotAt(float x, float y);

    void update(float dt);
    void updateDean(float dt);

    void draw();
    void drawButton(float x, float y, float w, float h, std::string text, unsigned size = 24);
    void drawHome();
    void drawHowToPlay();
    void drawProfile();
    void drawRoom();
    void drawTopBar();
    void drawInventory();
    void drawMessage();
    void drawDean();
    void drawDebug();
    void drawEndScreen();

    sf::RenderWindow window;
    GameState state;
    Map* map;
    int mapIndex;
    std::vector<std::string> mapNames;
    Keypad keypad;
    Profile profile;
    std::string typedName;  // username being typed on the profile screen
    int selectedItem;       // index into the inventory, -1 if nothing is selected
    bool showDebug;
    float mouseX, mouseY;
    float clock;        // seconds since the game started, used for blinking effects
    float timePlayed;   // how long the current map took
    int starsEarned;

    // the cursor turns into a hand over anything clickable
    std::optional<sf::Cursor> handCursor;
    std::optional<sf::Cursor> arrowCursor;
    bool overClickable;
    bool cursorIsHand;

    // the Dean
    bool deanComing;
    float hideTimer;
    int nextVisit;
};
