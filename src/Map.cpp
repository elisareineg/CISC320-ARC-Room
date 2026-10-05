// Map.cpp
#include "Map.h"

#include <iostream>

Map::Map() {
    currentRoom = "";
    timeLeft = 600;
    won = false;
    keypadRequested = false;
}

void Map::onUseItem(std::string item, std::string) {
    say("Using the " + item + " there does nothing.");
}

void Map::onKeypadSolved(std::string) {
    say("Something clicks open.");
}

std::string Map::getHint() {
    return "Try clicking on everything in the room, then try using your items on things.";
}

void Map::addRoom(std::string id, std::string title, std::string image) {
    Room room;
    room.id = id;
    room.title = title;
    room.image = image;
    rooms.push_back(room);
    // the first room added is where the player starts
    if (currentRoom == "") currentRoom = id;
}

void Map::addHotspot(std::string room, std::string id, std::string label, float x, float y, float w, float h) {
    Hotspot spot;
    spot.room = room;
    spot.id = id;
    spot.label = label;
    spot.x = x;
    spot.y = y;
    spot.w = w;
    spot.h = h;
    spot.hidingSpot = false;
    hotspots.push_back(spot);
}

void Map::setHidingSpot(std::string id) {
    Hotspot* spot = findHotspot(id);
    if (spot != nullptr) spot->hidingSpot = true;
    else say("Setup problem: setHidingSpot(\"" + id + "\") but there is no hotspot with that id.");
}

void Map::goToRoom(std::string id) {
    if (findRoom(id) == nullptr) {
        say("Setup problem: goToRoom(\"" + id + "\") but there is no room with that id.");
        return;
    }
    currentRoom = id;
}

void Map::say(std::string text) {
    messages.push_back(text);
}

void Map::giveItem(std::string item) {
    if (!hasItem(item)) inventory.push_back(item);
}

void Map::removeItem(std::string item) {
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i] == item) {
            inventory.erase(inventory.begin() + i);
            return;
        }
    }
}

bool Map::hasItem(std::string item) {
    for (int i = 0; i < (int)inventory.size(); i++) {
        if (inventory[i] == item) return true;
    }
    return false;
}

void Map::setFlag(std::string flag) { flags.insert(flag); }

bool Map::hasFlag(std::string flag) { return flags.count(flag) > 0; }

void Map::openKeypad(std::string id, std::string code) {
    keypadRequested = true;
    keypadId = id;
    keypadCode = code;
}

void Map::setTimeLimit(float seconds) { timeLeft = seconds; }

void Map::addDeanVisit(float secondsLeft) { deanVisits.push_back(secondsLeft); }

void Map::win() { won = true; }

// checks for typos in setup() and shows them in the game
void Map::checkSetup() {
    std::vector<std::string> problems;
    if (rooms.size() == 0) problems.push_back("the map has no rooms. Call addRoom() in setup().");

    for (int i = 0; i < (int)hotspots.size(); i++) {
        if (findRoom(hotspots[i].room) == nullptr)
            problems.push_back("hotspot \"" + hotspots[i].id + "\" is in room \"" + hotspots[i].room +
                               "\", which doesn't exist.");
        for (int j = 0; j < i; j++) {
            if (hotspots[j].id == hotspots[i].id)
                problems.push_back("two hotspots are called \"" + hotspots[i].id + "\".");
        }
    }

    if (deanVisits.size() > 0) {
        for (int i = 0; i < (int)rooms.size(); i++) {
            if (hidingSpotIn(rooms[i].id) == nullptr)
                problems.push_back("room \"" + rooms[i].id + "\" has no hiding spot, but the Dean visits.");
        }
    }

    for (int i = 0; i < (int)problems.size(); i++) {
        std::cout << getName() << " setup problem: " << problems[i] << "\n";
        say("Setup problem: " + problems[i]);
    }
}

Room* Map::findRoom(std::string id) {
    for (int i = 0; i < (int)rooms.size(); i++) {
        if (rooms[i].id == id) return &rooms[i];
    }
    return nullptr;
}

Hotspot* Map::findHotspot(std::string id) {
    for (int i = 0; i < (int)hotspots.size(); i++) {
        if (hotspots[i].id == id) return &hotspots[i];
    }
    return nullptr;
}

Hotspot* Map::hidingSpotIn(std::string room) {
    for (int i = 0; i < (int)hotspots.size(); i++) {
        if (hotspots[i].room == room && hotspots[i].hidingSpot) return &hotspots[i];
    }
    return nullptr;
}
