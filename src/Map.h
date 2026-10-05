// Map.h
// base class for every room in the game, see maps/StaufferMap.cpp for an example
#pragma once

#include <set>
#include <string>
#include <vector>

struct Room {
    std::string id;
    std::string title;  // shown at the top of the screen
    std::string image;  // background picture inside assets/, can be missing
};

// a clickable rectangle in a room
struct Hotspot {
    std::string room;
    std::string id;
    std::string label;
    float x, y, w, h;
    bool hidingSpot;
};

class Map {
public:
    Map();
    virtual ~Map() {}

    // every map has to have these
    virtual std::string getName() = 0;
    virtual void setup() = 0;
    virtual void onClick(std::string id) = 0;

    // optional
    virtual void onUseItem(std::string item, std::string id);
    virtual void onKeypadSolved(std::string keypadId);
    virtual std::string getHint();

    // helpers for the maps to use
    void addRoom(std::string id, std::string title, std::string image);
    void addHotspot(std::string room, std::string id, std::string label, float x, float y, float w, float h);
    void setHidingSpot(std::string id);
    void goToRoom(std::string id);
    void say(std::string text);
    void giveItem(std::string item);
    void removeItem(std::string item);
    bool hasItem(std::string item);
    void setFlag(std::string flag);
    bool hasFlag(std::string flag);
    void openKeypad(std::string keypadId, std::string code);
    void setTimeLimit(float seconds);
    void addDeanVisit(float secondsLeft);
    void win();

    // used by Game
    void checkSetup();
    Room* findRoom(std::string id);
    Hotspot* findHotspot(std::string id);
    Hotspot* hidingSpotIn(std::string room);

    std::vector<Room> rooms;
    std::vector<Hotspot> hotspots;
    std::vector<std::string> inventory;
    std::vector<std::string> messages;
    std::set<std::string> flags;
    std::vector<float> deanVisits;
    std::string currentRoom;
    float timeLeft;
    bool won;

    // set by openKeypad(), Game opens the keypad
    bool keypadRequested;
    std::string keypadId;
    std::string keypadCode;
};

// in maps/MapList.cpp
Map* createMap(int index);
int getMapCount();
