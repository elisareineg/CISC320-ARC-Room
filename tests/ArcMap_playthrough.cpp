// ArcMap_playthrough.cpp
// plays the whole ARC map without a window and prints what the player would see.
// Every code is read from the messages, the way a player would, so this also checks the clues match the
// answers. The codes are random, so they change every run. Build and run from the project folder:
//   clang++ -std=c++17 tests/ArcMap_playthrough.cpp src/Map.cpp src/maps/ArcMap.cpp src/maps/*Puzzle.cpp -o /tmp/arc_playthrough && /tmp/arc_playthrough
#include <cassert>
#include <iostream>
#include "../src/maps/ArcMap.h"

// prints the messages, then clears them like the player clicking through them
std::string show(ArcMap& map) {
    std::string all;
    for (std::string& message : map.messages) {
        std::cout << "    " << message << "\n";
        all += message + "\n";
    }
    map.messages.clear();
    return all;
}

std::string click(ArcMap& map, std::string id) {
    Hotspot* spot = map.findHotspot(id);
    assert(spot != nullptr && spot->room == map.currentRoom);  // you can only click what's on screen
    std::cout << "\n> Click " << spot->label << "\n";
    map.onClick(id);
    return show(map);
}

// the keypad is part of Game, so this does what it does: compare to the code, tell the map if it's right
std::string type(ArcMap& map, std::string typed) {
    assert(map.keypadRequested);
    std::cout << "\n> Type " << typed << " on the keypad\n";
    if (typed != map.keypadCode) {
        std::cout << "    (the keypad shakes: wrong code)\n";
        return "";
    }
    map.keypadRequested = false;
    map.onKeypadSolved(map.keypadId);
    return show(map);
}

void hint(ArcMap& map) {
    std::cout << "\n> Press H\n    Hint: " << map.getHint() << "\n";
}

// the digits after `marker` on the same line: ("code: 8 - 4 - 2 - 3", "code:") -> "8423"
std::string digitsAfter(std::string text, std::string marker) {
    std::string digits;
    for (char c : text.substr(text.find(marker) + marker.size())) {
        if (c == '\n') break;
        if (c >= '0' && c <= '9') digits += c;
    }
    return digits;
}

int main() {
    ArcMap map;
    map.setup();
    map.checkSetup();  // says "Setup problem" if a hotspot is in a room that doesn't exist, ids repeat, etc.
    for (std::string& message : map.messages) assert(message.find("Setup problem") == std::string::npos);

    std::cout << "=== " << map.getName() << " ===\n";
    show(map);

    std::cout << "\n--- Lobby (puzzle 1: key card) ---\n";
    click(map, "gate");
    click(map, "reception");  // nothing yet, the map hasn't been read
    assert(!map.hasItem("Key card"));
    hint(map);
    click(map, "floormap");
    click(map, "floorplan");
    click(map, "coatlockers");  // the square isn't on the torn map
    click(map, "reception");
    click(map, "merch");
    click(map, "reception");
    hint(map);
    click(map, "bleachers");
    assert(map.hasItem("Key card"));
    click(map, "gate");
    assert(map.currentRoom == "gym");

    std::cout << "\n--- Fitness floor (puzzle 2: locker) ---\n";
    click(map, "hallway");
    assert(map.currentRoom == "gym");
    click(map, "locker7");
    assert(!map.keypadRequested);
    std::string settings = digitsAfter(click(map, "tvs"), "numbers:");
    hint(map);
    click(map, "treadmill");
    assert(!map.keypadRequested);
    click(map, "rower");
    type(map, "999");
    std::string lockerCode = digitsAfter(type(map, settings), "code:");
    click(map, "locker3");
    click(map, "locker7");
    std::string note = type(map, lockerCode);
    assert(map.hasItem("Basketball"));
    click(map, "hallway");
    assert(map.currentRoom == "court");

    std::cout << "\n--- Court (puzzle 3: exit key) ---\n";
    click(map, "standsL");
    assert(map.currentRoom == "court");
    click(map, "exit");
    click(map, "hoop");
    assert(map.currentRoom == "overhead");
    hint(map);
    std::string number = digitsAfter(note, "seat ");
    bool left = note.find("\"Left") != std::string::npos;
    click(map, (left ? "right" : "left") + number);  // right number, wrong side
    assert(!map.hasItem("Exit key"));
    click(map, (left ? "left" : "right") + number);
    assert(map.hasItem("Exit key"));
    click(map, "backdown");
    click(map, "exit");
    assert(map.won);

    std::cout << "\nEscaped! The whole ARC map works from start to end.\n";
}
