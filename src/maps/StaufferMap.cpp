// StaufferMap.cpp
// Floor 3: flashlight on the whiteboard (3_1_) + note behind the books (_7_9) -> stairwell code 3719
// Floor 2: toonie -> vending machine screwdriver -> power panel -> terminal password 1841
// Ground:  magnet on string -> key from the book return -> keycard from the desk -> main doors
#include "StaufferMap.h"

std::string StaufferMap::getName() { return "Library"; }

void StaufferMap::setup() {
    setTimeLimit(900);
    addDeanVisit(600);
    addDeanVisit(300);

    addRoom("floor3", "Floor 3: Silent Study", "stauffer/floor3.png");
    addHotspot("floor3", "window", "Window", 70, 90, 230, 210);
    addHotspot("floor3", "whiteboard", "Whiteboard", 350, 100, 320, 180);
    addHotspot("floor3", "bookshelf", "Bookshelf", 720, 80, 260, 380);
    addHotspot("floor3", "carrel", "Study carrel", 80, 330, 330, 150);
    addHotspot("floor3", "lostfound", "Lost and found box", 450, 400, 170, 110);
    addHotspot("floor3", "stairs3", "Stairwell door", 1030, 150, 180, 330);
    setHidingSpot("carrel");

    addRoom("floor2", "Floor 2: Computer Commons", "stauffer/floor2.png");
    addHotspot("floor2", "vending", "Vending machine", 60, 110, 180, 360);
    addHotspot("floor2", "panel", "Electrical panel", 290, 150, 150, 200);
    addHotspot("floor2", "plaque", "Brass plaque", 520, 90, 200, 100);
    addHotspot("floor2", "terminal", "Staff computer", 480, 250, 360, 170);
    addHotspot("floor2", "sticky", "Sticky note", 846, 256, 70, 60);
    addHotspot("floor2", "desks", "Computer desks", 480, 430, 420, 80);
    addHotspot("floor2", "stairs2", "Stairwell door", 1030, 150, 180, 330);
    setHidingSpot("desks");

    addRoom("ground", "Ground Floor: Circulation Hall", "stauffer/ground.png");
    addHotspot("ground", "returnbin", "Book return", 60, 200, 160, 260);
    addHotspot("ground", "noticeboard", "Staff noticeboard", 260, 110, 220, 150);
    addHotspot("ground", "circdesk", "Circulation desk", 250, 360, 440, 140);
    addHotspot("ground", "cart", "Returns cart", 720, 390, 150, 120);
    addHotspot("ground", "doors", "Main doors", 930, 100, 300, 400);
    setHidingSpot("circdesk");

    say("You wake up with your face on a linear algebra textbook. The silent study is dark and everyone is gone.");
    say("Somewhere below, a door slams. Dean Deluzio does his rounds after closing, and you do not want him "
        "to find you.");
    say("Get down three floors and out the front doors.");
}

void StaufferMap::onClick(std::string id) {
    // floor 3
    if (id == "window") {
        say("Union Street is empty. The streetlights are red. They have never been red.");
    } else if (id == "whiteboard") {
        if (hasFlag("whiteboard read")) say("The faded marker says: 3 _ 1 _");
        else say("Wiped clean, but you can almost see faded writing. You need some light to read it.");
    } else if (id == "bookshelf") {
        if (hasFlag("shelf searched")) {
            say("Just books now. The note said _ 7 _ 9.");
        } else {
            setFlag("shelf searched");
            giveItem("Toonie");
            say("Behind a row of books someone hid a toonie and a note that says: _ 7 _ 9");
        }
    } else if (id == "carrel") {
        say("A study carrel. There's just enough room under the desk to hide.");
    } else if (id == "lostfound") {
        if (hasFlag("got flashlight")) {
            say("Mittens, one AirPod, and a stats textbook with no cover.");
        } else {
            setFlag("got flashlight");
            giveItem("Flashlight");
            say("In the lost and found: mittens, one AirPod... and a flashlight that still works.");
        }
    } else if (id == "stairs3") {
        if (hasFlag("stairs3 open")) goToRoom("floor2");
        else openKeypad("stairs3", "3719");
    }

    // floor 2
    else if (id == "vending") {
        if (hasItem("Screwdriver")) say("Slot B4 is empty now.");
        else say("Chips, gum, and in slot B4, for some reason, a screwdriver. It costs $2.");
    } else if (id == "panel") {
        if (hasFlag("power on")) say("All the breakers are on.");
        else say("The electrical panel is screwed shut.");
    } else if (id == "plaque") {
        say("A brass plaque: Queen's University, Royal Charter 1841.");
    } else if (id == "sticky") {
        say("A sticky note on the monitor: \"Staff password = the year of the charter\".");
    } else if (id == "terminal") {
        if (!hasFlag("power on")) say("The computer is dead. The power on this floor is off.");
        else if (hasFlag("stairs2 open")) say("The screen says: STAIRWELL UNLOCKED.");
        else openKeypad("terminal", "1841");
    } else if (id == "desks") {
        say("Rows of computer desks. You could hide underneath one.");
    } else if (id == "stairs2") {
        if (hasFlag("stairs2 open")) goToRoom("ground");
        else say("The door is held shut by a magnetic lock. It must be controlled by the staff computer.");
    }

    // ground floor
    else if (id == "returnbin") {
        if (hasFlag("got key")) say("Just returned books in there now.");
        else say("Something small and shiny is at the bottom of the book return. Your arm doesn't reach.");
    } else if (id == "noticeboard") {
        if (hasFlag("got magnet")) {
            say("Just posters about midterm study sessions.");
        } else {
            setFlag("got magnet");
            giveItem("Magnet on a string");
            say("Someone tied a big magnet to a piece of string and left it on the noticeboard. Handy.");
        }
    } else if (id == "circdesk") {
        if (hasItem("Keycard") || hasFlag("escaped")) say("The drawer is empty. You could hide behind the desk.");
        else say("The desk drawer is locked with a small brass lock.");
    } else if (id == "cart") {
        say("A cart full of returned books. Someone returned the same textbook four times.");
    } else if (id == "doors") {
        say("The main doors. There's a card reader beside them.");
    }
}

void StaufferMap::onUseItem(std::string item, std::string id) {
    if (item == "Flashlight" && id == "whiteboard") {
        setFlag("whiteboard read");
        say("In the flashlight beam the faded marker shows up: 3 _ 1 _");
    } else if (item == "Toonie" && id == "vending") {
        removeItem("Toonie");
        giveItem("Screwdriver");
        say("The machine takes your toonie and drops the screwdriver.");
    } else if (item == "Screwdriver" && id == "panel") {
        removeItem("Screwdriver");
        setFlag("power on");
        say("You unscrew the cover and flip the main breaker. The lights and computers come back on.");
    } else if (item == "Magnet on a string" && id == "returnbin") {
        removeItem("Magnet on a string");
        setFlag("got key");
        giveItem("Small key");
        say("You lower the magnet into the book return. It comes back up with a small brass key.");
    } else if (item == "Small key" && id == "circdesk") {
        removeItem("Small key");
        giveItem("Keycard");
        say("The key opens the desk drawer. Inside is a staff keycard.");
    } else if (item == "Keycard" && id == "doors") {
        setFlag("escaped");
        say("The reader beeps green and the lock clunks open. Cold air. You're out.");
        win();
    } else {
        say("That doesn't do anything.");
    }
}

void StaufferMap::onKeypadSolved(std::string keypadId) {
    if (keypadId == "stairs3") {
        setFlag("stairs3 open");
        say("The keypad beeps and the stairwell door unlocks. Click the door to go down.");
    } else if (keypadId == "terminal") {
        setFlag("stairs2 open");
        say("ACCESS GRANTED. Somewhere behind you the stairwell door clicks open.");
    }
}

std::string StaufferMap::getHint() {
    if (currentRoom == "floor3") {
        if (!hasItem("Flashlight")) return "Look in the lost and found box.";
        if (!hasFlag("whiteboard read")) return "Click the flashlight in your items, then click the whiteboard.";
        if (!hasFlag("shelf searched")) return "Search the bookshelf.";
        return "Put the two codes together: 3 _ 1 _ and _ 7 _ 9 make 3719. Use it on the stairwell door.";
    }
    if (currentRoom == "floor2") {
        if (hasItem("Toonie")) return "Use the toonie on the vending machine.";
        if (hasItem("Screwdriver")) return "Use the screwdriver on the electrical panel.";
        if (!hasFlag("stairs2 open")) return "The password is the year on the brass plaque: 1841. Type it into the computer.";
        return "The stairwell door is open now.";
    }
    if (!hasFlag("got magnet") && !hasFlag("got key")) return "Check the staff noticeboard.";
    if (hasItem("Magnet on a string")) return "Use the magnet on the book return.";
    if (hasItem("Small key")) return "Use the small key on the circulation desk.";
    return "Use the keycard on the main doors.";
}
