// ArcMap.cpp
// the ARC (gym), one puzzle per room, solved in order:
// Lobby: torn map on the floor -> same symbols on the floor plan -> 3 key card pieces -> scan it at the gate
// Gym:   numbers on the TVs -> type them into the machine that was left on -> it shows the locker code
//        -> locker 7 opens -> basketball with a note that names a seat
// Court: basketball in the hoop -> bleachers from above -> seat from the note -> key -> exit door
//
// The rules are in KeycardPuzzle, LockerPuzzle and CourtPuzzle (tested in tests/). This file only turns
// clicks into calls on them and says what happened. tests/ArcMap_playthrough.cpp plays it start to end.
//
// PLACEHOLDER marks what the ARC design wants that the shared Game can't show yet:
//   (photo)  no picture for the room yet, so Game draws every hotspot as a grey labelled box
//   (popup)  the design zooms in on something, for now it's a say() message or the keypad
//   (sound)  the design plays a sound, Game has no audio yet
//   (visual) the design changes how something looks once it's used, Game has one picture per room
// Search for PLACEHOLDER to find them all.
#include "ArcMap.h"

namespace {
// the fixed answers. The locker code and the seat are random every game (see the constructor)
const Equipment CORRECT_EQUIPMENT = Equipment::Rower;
const std::vector<int> MACHINE_SETTINGS = {3, 1, 4};
const int CORRECT_LOCKER = 7;
const int LOCKER_COUNT = 8;
const int SEATS_PER_SIDE = 10;

// {3, 1, 4} -> "314", what the keypad expects
std::string toCode(const std::vector<int>& digits) {
    std::string code;
    for (int d : digits) code += std::to_string(d);
    return code;
}

// {3, 1, 4} -> "3 - 1 - 4", for messages
std::string spaced(const std::vector<int>& digits) {
    std::string text;
    for (int i = 0; i < (int)digits.size(); i++) {
        if (i > 0) text += " - ";
        text += std::to_string(digits[i]);
    }
    return text;
}

std::string seatName(Seat seat) {
    return std::string(seat.side == Side::left ? "Left" : "Right") + " bleachers, seat " + std::to_string(seat.number);
}
}

// Game makes a new ArcMap every time the player starts it, so the codes change each game
ArcMap::ArcMap()
    : rng(std::random_device{}()),
      keycard({Location::Reception, Location::MerchStore, Location::Bleachers}),
      locker(LockerPuzzle::withRandomCode(CORRECT_EQUIPMENT, MACHINE_SETTINGS, CORRECT_LOCKER, rng)),
      court(CourtPuzzle::withRandomSeat(rng, SEATS_PER_SIDE)) {}

std::string ArcMap::getName() { return "Gym (ARC)"; }

void ArcMap::setup() {
    setTimeLimit(900);
    // no Dean visits yet. If you add addDeanVisit(), every room needs a setHidingSpot()

    // reception desk in the middle, merch store on the right, gym entrance on the left, paper on the floor
    // PLACEHOLDER (photo): assets/arc/lobby.png doesn't exist yet
    addRoom("lobby", "ARC: Lobby", "arc/lobby.png");
    addHotspot("lobby", "gate", "Gym entrance gate", 40, 120, 250, 400);
    addHotspot("lobby", "floorplan", "Floor plan", 330, 80, 200, 140);
    addHotspot("lobby", "bleachers", "Bleachers entrance", 590, 90, 180, 200);
    addHotspot("lobby", "coatlockers", "Coat lockers", 800, 100, 130, 200);
    addHotspot("lobby", "merch", "Merch store", 960, 100, 280, 400);
    addHotspot("lobby", "reception", "Reception desk", 470, 330, 340, 170);
    addHotspot("lobby", "floormap", "Paper on the floor", 380, 540, 140, 70);

    // exercise equipment, TVs, and lockers on the side
    // PLACEHOLDER (photo): assets/arc/gym.png doesn't exist yet. The design only makes the right machine and
    // the right locker clickable. Until the picture exists the others are hotspots too, so they show up as
    // boxes; once gym.png is in, their addHotspot lines can go
    addRoom("gym", "ARC: Fitness Floor", "arc/gym.png");
    addHotspot("gym", "hallway", "Door to the courts", 60, 60, 160, 180);
    addHotspot("gym", "tvs", "TV screens", 380, 60, 520, 110);
    addHotspot("gym", "treadmill", "Treadmill", 60, 280, 200, 200);
    addHotspot("gym", "bike", "Exercise bike", 290, 300, 170, 180);
    addHotspot("gym", "rower", "Rowing machine", 490, 380, 300, 120);
    addHotspot("gym", "weights", "Weight rack", 810, 280, 150, 200);
    // lockers 1 to 8, two rows of 4 on the right wall
    for (int i = 0; i < LOCKER_COUNT; i++) {
        addHotspot("gym", "locker" + std::to_string(i + 1), "Locker " + std::to_string(i + 1),
                   1000 + (i % 4) * 65, 100 + (i / 4) * 230, 60, 210);
    }

    // the court with bleachers on both sides
    // PLACEHOLDER (photo): assets/arc/court.png doesn't exist yet
    addRoom("court", "ARC: Main Court", "arc/court.png");
    addHotspot("court", "hoop", "Basketball hoop", 540, 70, 200, 200);
    addHotspot("court", "standsL", "Left bleachers", 20, 100, 220, 450);
    addHotspot("court", "standsR", "Right bleachers", 1040, 100, 220, 450);
    addHotspot("court", "exit", "Exit door", 540, 400, 200, 220);

    // the bird's-eye view of both bleachers, after the ball goes in
    // PLACEHOLDER (photo): assets/arc/overhead.png doesn't exist yet
    addRoom("overhead", "ARC: Bleachers From Above", "arc/overhead.png");
    for (int n = 1; n <= SEATS_PER_SIDE; n++) {
        addHotspot("overhead", "left" + std::to_string(n), "Left seat " + std::to_string(n), 80, 40 + n * 55, 180, 48);
        addHotspot("overhead", "right" + std::to_string(n), "Right seat " + std::to_string(n), 1020, 40 + n * 55, 180, 48);
    }
    addHotspot("overhead", "backdown", "Back to the court", 440, 260, 400, 160);

    say("The ARC after closing. The lights hum, and the gate into the gym is locked.");
    say("There has to be a way through the gym and out the other side.");
}

void ArcMap::onClick(std::string id) {
    // lobby, puzzle 1
    if (id == "floormap") {
        keycard.viewMap();
        // PLACEHOLDER (popup): the design zooms in on the torn map and shows the symbols as pictures
        say("A page torn from a map. Three symbols are circled: a triangle, a star and a circle.");
    } else if (id == "floorplan") {
        // PLACEHOLDER (popup): the design zooms in on the floor plan, with a symbol drawn on each area
        say("The floor plan has a symbol on each area: Reception = triangle, Merch store = star, "
            "Bleachers = circle, Coat lockers = square.");
    } else if (id == "reception") {
        search(Location::Reception, "reception desk");
    } else if (id == "merch") {
        search(Location::MerchStore, "merch store");
    } else if (id == "bleachers") {
        search(Location::Bleachers, "bleachers entrance");
    } else if (id == "coatlockers") {
        search(Location::Locker, "coat lockers");
    } else if (id == "gate") {
        if (keycard.isSolved()) {
            goToRoom("gym");
        } else if (keycard.scanCard()) {
            removeItem("Key card");
            // PLACEHOLDER (sound): correct sound
            // PLACEHOLDER (visual): the gate slides open
            say("The reader beeps green and the gate slides open.");
            goToRoom("gym");
        } else {
            // PLACEHOLDER (sound): short error sound
            say("A glass gate with a card reader. The light blinks red: you need a key card.");
        }
    }

    // gym, puzzle 2
    else if (id == "tvs") {
        say("Every TV shows the same numbers: " + spaced(MACHINE_SETTINGS));
    } else if (id == "treadmill" || id == "bike" || id == "rower" || id == "weights") {
        useMachine(id);
    } else if (id.rfind("locker", 0) == 0) {  // locker1 to locker8
        openLocker(std::stoi(id.substr(6)));
    } else if (id == "hallway") {
        if (locker.isSolved()) goToRoom("court");
        else say("The door to the courts is locked.");
    }

    // court, puzzle 3
    else if (id == "hoop") {
        if (hasItem("Basketball")) {
            removeItem("Basketball");
            court.shootBall();
            // PLACEHOLDER (visual): the design pans the camera up to a bird's-eye view of the court. For now
            // it switches to the "overhead" room, which will be its own picture
            say("Swish! You look up, and from here you can see the whole court and both bleachers.");
            goToRoom("overhead");
        } else {
            say("A basketball hoop. The net is still swaying.");
        }
    } else if (id == "standsL" || id == "standsR") {
        if (court.bleachersOpen()) goToRoom("overhead");
        else say("Rows and rows of seats. From down here you can't read the numbers.");
    } else if (id == "exit") {
        if (court.useKey()) {
            // PLACEHOLDER (sound): the exit door's unlocking sound
            say("The key turns and the exit door unlocks with a heavy clunk. Cold air. You're out.");
            win();
        } else {
            // PLACEHOLDER (sound): short error sound
            say("The exit door. It needs a key.");
        }
    }

    // bleachers from above
    else if (id == "backdown") {
        goToRoom("court");
    } else if (id.rfind("left", 0) == 0) {  // left1 to left10
        checkSeat({Side::left, std::stoi(id.substr(4))});
    } else if (id.rfind("right", 0) == 0) {  // right1 to right10
        checkSeat({Side::right, std::stoi(id.substr(5))});
    }
}

// puzzle 1: KeyCardPuzzle ignores searches until the torn map has been read
void ArcMap::search(Location place, std::string name) {
    if (keycard.wasFound(place)) {
        say("You already searched the " + name + ".");
    } else if (!keycard.searchLocation(place)) {
        // PLACEHOLDER (sound): short error sound
        say("You look around the " + name + ", but nothing stands out.");
    } else if (keycard.hasCard()) {
        giveItem("Key card");
        // PLACEHOLDER (sound): correct sound
        // PLACEHOLDER (popup): the design shows the finished key card
        say("The last piece! The pieces snap together into a full key card.");
    } else {
        // PLACEHOLDER (sound): correct sound
        say("Part of a key card is hidden at the " + name + ". (" + std::to_string(keycard.fragmentsFound()) +
            " of " + std::to_string(keycard.fragmentsNeeded()) + " pieces)");
    }
}

// puzzle 2, step 1: only the machine someone left on takes the settings
void ArcMap::useMachine(std::string id) {
    Equipment equipment = Equipment::Treadmill;
    if (id == "bike") equipment = Equipment::Bike;
    else if (id == "rower") equipment = Equipment::Rower;
    else if (id == "weights") equipment = Equipment::Weights;

    if (!locker.isCorrectEquipment(equipment)) {
        say("Switched off and wiped down. Nobody has used it tonight.");
    } else if (locker.machineShowsCode()) {
        say("Its screen still shows: " + spaced(locker.displayedCode()));
    } else {
        // PLACEHOLDER (popup): the design zooms in on the machine so the settings can be entered. For now
        // this message describes it and the shared keypad popup takes the settings
        say("This one was left on, halfway through being set up. Its screen asks for " +
            std::to_string(MACHINE_SETTINGS.size()) + " settings.");
        openKeypad("machine", toCode(MACHINE_SETTINGS));
    }
}

// puzzle 2, step 2: the locker only takes a code once the machine has shown it
void ArcMap::openLocker(int number) {
    if (!locker.isCorrectLocker(number)) {
        say("Locker " + std::to_string(number) + " is empty.");
    } else if (locker.isSolved()) {
        say("Locker " + std::to_string(number) + " is empty now. The note said: " + seatName(court.correctSeat()));
    } else if (!locker.machineShowsCode()) {
        say("Locker " + std::to_string(number) + " has a keypad lock. You don't know the code yet.");
    } else {
        // PLACEHOLDER (popup): the design zooms in on the locker. The shared keypad popup is used for the code
        openKeypad("locker", toCode(locker.displayedCode()));
    }
}

// puzzle 3: only the seat from the locker note has the key
void ArcMap::checkSeat(Seat seat) {
    if (court.keyFound()) {
        say("You already found the key. Go back down to the exit door.");
    } else if (court.selectSeat(seat)) {
        giveItem("Exit key");
        // PLACEHOLDER (sound): correct sound
        // PLACEHOLDER (popup): the design shows the hidden key being revealed under the seat
        say("Something is taped under the seat: a key!");
    } else {
        // PLACEHOLDER (sound): short error sound
        say("Nothing under this seat.");
    }
}

// clicking the gate, hoop or exit already works when you have the item, this is for players who use it on them
void ArcMap::onUseItem(std::string item, std::string id) {
    if ((item == "Key card" && id == "gate") || (item == "Basketball" && id == "hoop") ||
        (item == "Exit key" && id == "exit")) {
        onClick(id);
    } else {
        // PLACEHOLDER (sound): short error sound
        say("That doesn't do anything.");
    }
}

// the keypad already checked the code, this tells the puzzle so its next step unlocks
void ArcMap::onKeypadSolved(std::string keypadId) {
    if (keypadId == "machine") {
        locker.enterMachineSettings(MACHINE_SETTINGS);
        // PLACEHOLDER (sound): correct sound
        say("The machine beeps and its screen changes to a new code: " + spaced(locker.displayedCode()));
    } else if (keypadId == "locker") {
        locker.enterLockerCode(locker.displayedCode());
        giveItem("Basketball");
        // PLACEHOLDER (sound): correct sound
        // PLACEHOLDER (visual): the locker swings open
        // PLACEHOLDER (popup): the design zooms in on the note attached to the basketball
        say("Locker " + std::to_string(CORRECT_LOCKER) + " swings open. Inside is a basketball with a note taped "
            "to it: \"" + seatName(court.correctSeat()) + "\"");
        // PLACEHOLDER (visual): the door opens to show the hallway to the court
        say("Across the gym, the door to the courts clicks open.");
    }
}

// PLACEHOLDER (popup): the design has a Hint button in the top right with a bubble that stays for 5 seconds.
// The shared Game shows the hint as a message when the player presses H
std::string ArcMap::getHint() {
    if (currentRoom == "lobby") {
        if (!keycard.hasViewedMap()) return "Something fell on the floor.";
        if (!keycard.hasCard()) return "Look closely at the symbols on the map.";
        return "Your key card is complete. Try the gate.";
    }
    if (currentRoom == "gym") {
        if (!locker.machineShowsCode()) return "Think about what you do at the gym.";
        if (!locker.isSolved()) return "The machine gave you a new code. Which locker has a keypad?";
        return "The door to the courts is open.";
    }
    if (!court.bleachersOpen()) return "You have a basketball, and there's a hoop.";
    if (!court.keyFound()) return "Think about how the code may relate to this location.";
    return "Take the key back down to the exit door.";
}
