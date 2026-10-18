// Game.cpp
#include "Game.h"

#include "Draw.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>

const float HIDE_TIME = 12;       // seconds you get to reach the hiding spot
const float CAUGHT_PENALTY = 60;  // seconds lost if the Dean finds you

const std::string TITLE = "GAEL JAIL";
const std::string PROMPT = "Choose a room to begin";

// home page room buttons
const float BUTTON_X = 440;
const float BUTTON_W = 400;
const float BUTTON_H = 60;

// the Home button in the top right of every room
const float HOME_X = 1170;
const float HOME_Y = 4;
const float HOME_W = 100;
const float HOME_H = 32;

static std::string formatTime(float seconds) {
    if (seconds < 0) seconds = 0;
    int total = (int)std::ceil(seconds);
    int minutes = total / 60;
    int secs = total % 60;
    std::string text = std::to_string(minutes) + ":";
    if (secs < 10) text += "0";
    return text + std::to_string(secs);
}

Game::Game() {
    window.create(sf::VideoMode({1280, 720}), "Gael Jail", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    handCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Hand);
    arrowCursor = sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow);

    state = HOME;
    map = nullptr;
    mapIndex = 0;
    selectedItem = -1;
    showDebug = false;
    mouseX = 0;
    mouseY = 0;
    clock = 0;
    timePlayed = 0;
    starsEarned = 0;
    overClickable = false;
    cursorIsHand = false;
    deanComing = false;
    hideTimer = 0;
    nextVisit = 0;

    for (int i = 0; i < getMapCount(); i++) {
        Map* m = createMap(i);
        mapNames.push_back(m->getName());
        delete m;
    }
}

Game::~Game() { delete map; }

void Game::run() {
    if (!loadFonts()) return;

    sf::Clock frameClock;
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;  // don't jump ahead if the window was dragged

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                saveAndQuit();
            }
            if (const sf::Event::MouseMoved* move = event->getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f pos = window.mapPixelToCoords(move->position);
                mouseX = pos.x;
                mouseY = pos.y;
            }
            if (const sf::Event::MouseButtonPressed* press = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (press->button == sf::Mouse::Button::Left) {
                    sf::Vector2f pos = window.mapPixelToCoords(press->position);
                    onMouseClick(pos.x, pos.y);
                }
            }
            if (const sf::Event::KeyPressed* key = event->getIf<sf::Event::KeyPressed>()) {
                onKey(key->code);
            }
            if (const sf::Event::TextEntered* text = event->getIf<sf::Event::TextEntered>()) {
                if (text->unicode < 128) onText((char)text->unicode);
            }
        }

        if (!window.isOpen()) break;
        update(dt);
        draw();
    }
}

void Game::startMap(int index) {
    delete map;
    map = createMap(index);
    mapIndex = index;
    map->setup();
    map->checkSetup();
    // visits are checked in order, so put the earliest one (most time left) first
    std::sort(map->deanVisits.begin(), map->deanVisits.end(), std::greater<float>());

    state = PLAYING;
    keypad.close();
    selectedItem = -1;
    deanComing = false;
    nextVisit = 0;
    timePlayed = 0;
}

void Game::goHome() {
    delete map;
    map = nullptr;
    keypad.close();
    state = HOME;
}

void Game::saveAndQuit() {
    profile.save();
    window.close();
}

void Game::onMouseClick(float x, float y) {
    if (state == HOME) {
        clickHome(x, y);
    } else if (state == HOW_TO_PLAY) {
        if (isInside(x, y, 590, 540, 100, 50)) state = HOME;
    } else if (state == PROFILE) {
        clickProfile(x, y);
    } else if (state == WON) {
        if (isInside(x, y, BUTTON_X, 490, BUTTON_W, BUTTON_H)) goHome();
        if (isInside(x, y, BUTTON_X, 570, BUTTON_W, BUTTON_H)) saveAndQuit();
    } else if (state == LOST) {
        if (isInside(x, y, BUTTON_X, 490, BUTTON_W, BUTTON_H)) startMap(mapIndex);
        if (isInside(x, y, BUTTON_X, 570, BUTTON_W, BUTTON_H)) goHome();
    } else if (state == PLAYING) {
        if (isInside(x, y, HOME_X, HOME_Y, HOME_W, HOME_H)) {
            goHome();
            return;
        }
        // a message has to be closed before you can do anything else
        if (map->messages.size() > 0) {
            map->messages.erase(map->messages.begin());
            return;
        }
        if (keypad.isOpen()) {
            if (keypad.click(x, y)) map->onKeypadSolved(keypad.getId());
            return;
        }
        if (y >= INVENTORY_Y) clickInventory(x, y);
        else clickRoom(x, y);
    }
}

void Game::clickHome(float x, float y) {
    for (int i = 0; i < (int)mapNames.size(); i++) {
        if (isInside(x, y, BUTTON_X, 300 + i * 80, BUTTON_W, BUTTON_H)) {
            startMap(i);
            return;
        }
    }
    if (isInside(x, y, 760, 20, 140, 44)) state = PROFILE;
    if (isInside(x, y, 910, 20, 170, 44)) state = HOW_TO_PLAY;
    if (isInside(x, y, 1090, 20, 170, 44)) saveAndQuit();
}

void Game::clickProfile(float x, float y) {
    if (!profile.loggedIn()) {
        if (isInside(x, y, 440, 420, 190, 55) && typedName != "") {
            profile.login(typedName);
            typedName = "";
        }
        if (isInside(x, y, 650, 420, 190, 55)) state = HOME;
    } else {
        if (isInside(x, y, 440, 540, 190, 55)) profile.logout();
        if (isInside(x, y, 650, 540, 190, 55)) state = HOME;
    }
}

void Game::clickRoom(float x, float y) {
    Hotspot* spot = hotspotAt(x, y);
    if (spot == nullptr) {
        selectedItem = -1;
        return;
    }

    if (deanComing && spot->hidingSpot) {
        deanComing = false;
        nextVisit++;
        map->say("You squeeze in and hold your breath. Footsteps stop right beside you... then keep going. "
                 "He's gone, for now.");
        return;
    }

    if (selectedItem >= 0) {
        std::string item = map->inventory[selectedItem];
        selectedItem = -1;
        map->onUseItem(item, spot->id);
    } else {
        map->onClick(spot->id);
    }
}

void Game::clickInventory(float x, float y) {
    for (int i = 0; i < (int)map->inventory.size(); i++) {
        if (isInside(x, y, 20 + i * 132, INVENTORY_Y + 10, 125, 60)) {
            if (selectedItem == i) selectedItem = -1;
            else selectedItem = i;
            return;
        }
    }
    selectedItem = -1;
}

Hotspot* Game::hotspotAt(float x, float y) {
    // go backwards so a hotspot added later (drawn on top) wins
    for (int i = (int)map->hotspots.size() - 1; i >= 0; i--) {
        Hotspot& h = map->hotspots[i];
        if (h.room == map->currentRoom && isInside(x, y, h.x, h.y, h.w, h.h)) return &h;
    }
    return nullptr;
}

void Game::onKey(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Escape) {
        if (keypad.isOpen()) keypad.close();
        else if (state == HOW_TO_PLAY || state == PROFILE) state = HOME;
        else if (state != HOME) goHome();
        return;
    }

    if (state == PROFILE && !profile.loggedIn()) {
        if (key == sf::Keyboard::Key::Backspace && typedName.size() > 0) typedName.pop_back();
        if (key == sf::Keyboard::Key::Enter && typedName != "") {
            profile.login(typedName);
            typedName = "";
        }
        return;
    }

    if (state != PLAYING) return;

    if (keypad.isOpen()) {
        int k = (int)key;
        char button = ' ';
        if (k >= (int)sf::Keyboard::Key::Num0 && k <= (int)sf::Keyboard::Key::Num9)
            button = '0' + (k - (int)sf::Keyboard::Key::Num0);
        if (k >= (int)sf::Keyboard::Key::Numpad0 && k <= (int)sf::Keyboard::Key::Numpad9)
            button = '0' + (k - (int)sf::Keyboard::Key::Numpad0);
        if (key == sf::Keyboard::Key::Enter) button = 'E';
        if (key == sf::Keyboard::Key::Backspace) button = 'C';
        if (button != ' ' && keypad.press(button)) map->onKeypadSolved(keypad.getId());
        return;
    }

    if (key == sf::Keyboard::Key::D) showDebug = !showDebug;
    if (key == sf::Keyboard::Key::H && map->messages.size() == 0) map->say("Hint: " + map->getHint());
}

void Game::onText(char c) {
    // usernames become file names, so only letters, numbers, - and _
    if (state != PROFILE || profile.loggedIn()) return;
    if ((std::isalnum((unsigned char)c) || c == '-' || c == '_') && typedName.size() < 16) typedName += c;
}

void Game::update(float dt) {
    clock += dt;
    if (state != PLAYING) return;

    keypad.update(dt);
    if (map->keypadRequested) {
        map->keypadRequested = false;
        keypad.open(map->keypadId, map->keypadCode);
    }
    if (selectedItem >= (int)map->inventory.size()) selectedItem = -1;

    // wait until the last message is read before showing the win screen
    if (map->won && map->messages.size() == 0) {
        starsEarned = calculateStars(timePlayed);
        if (profile.loggedIn()) {
            profile.recordResult(mapNames[mapIndex], timePlayed, starsEarned);
            profile.save();
        }
        state = WON;
        return;
    }

    // the clock stops while you read a message
    if (map->messages.size() > 0) return;

    map->timeLeft -= dt;
    timePlayed += dt;
    updateDean(dt);

    if (map->timeLeft <= 0) {
        map->timeLeft = 0;
        keypad.close();
        state = LOST;
    }
}

void Game::updateDean(float dt) {
    if (!deanComing) {
        if (nextVisit < (int)map->deanVisits.size() && map->timeLeft <= map->deanVisits[nextVisit]) {
            deanComing = true;
            hideTimer = HIDE_TIME;
            keypad.close();
            selectedItem = -1;
        }
        return;
    }

    hideTimer -= dt;
    if (hideTimer <= 0) {
        deanComing = false;
        nextVisit++;
        map->timeLeft -= CAUGHT_PENALTY;
        map->say("A hand grabs your shoulder. You twist free and run, but it takes you a full minute to "
                 "lose him in the dark.");
    }
}

void Game::draw() {
    window.clear(sf::Color::Black);
    overClickable = false;

    if (state == HOME) {
        drawHome();
    } else if (state == HOW_TO_PLAY) {
        drawHome();
        drawHowToPlay();
    } else if (state == PROFILE) {
        drawHome();
        drawProfile();
    } else if (state == PLAYING) {
        drawRoom();
        drawTopBar();
        drawInventory();
        if (deanComing) drawDean();
        if (showDebug) drawDebug();
        if (keypad.isOpen()) keypad.draw(window);
        drawMessage();
    } else {
        drawEndScreen();
    }

    // the draw functions set overClickable when the mouse is over a button or hotspot
    if (overClickable != cursorIsHand && handCursor && arrowCursor) {
        window.setMouseCursor(overClickable ? *handCursor : *arrowCursor);
        cursorIsHand = overClickable;
    }
    window.display();
}

void Game::drawButton(float x, float y, float w, float h, std::string text, unsigned size) {
    bool hover = isInside(mouseX, mouseY, x, y, w, h);
    if (hover) overClickable = true;
    sf::Color fill = hover ? sf::Color(110, 20, 20) : sf::Color(30, 25, 25, 220);
    drawBox(window, x, y, w, h, fill, sf::Color(160, 140, 120));
    drawText(window, text, x + w / 2, y + (h - size) / 2 - 3, size, sf::Color::White, true);
}

void Game::drawHome() {
    sf::Texture* background = getTexture("img/menu_bg.png");
    if (background != nullptr) drawImage(window, *background, 0, 0, SCREEN_W, SCREEN_H);
    drawBox(window, 0, 0, SCREEN_W, SCREEN_H, sf::Color(0, 0, 0, 140));

    drawButton(760, 20, 140, 44, "Profile", 20);
    drawButton(910, 20, 170, 44, "How to Play", 20);
    drawButton(1090, 20, 170, 44, "Save & Quit", 20);

    drawTitle(window, TITLE, 640, 90, 120, sf::Color(190, 20, 20));
    drawText(window, PROMPT, 640, 240, 26, sf::Color::White, true);

    for (int i = 0; i < (int)mapNames.size(); i++) {
        drawButton(BUTTON_X, 300 + i * 80, BUTTON_W, BUTTON_H, mapNames[i], 26);
    }

    std::string who = profile.loggedIn() ? "Logged in as " + profile.username() : "Not logged in";
    drawText(window, who, 20, 685, 18, sf::Color(170, 170, 170));
}

void Game::drawHowToPlay() {
    drawBox(window, 0, 0, SCREEN_W, SCREEN_H, sf::Color(0, 0, 0, 160));
    drawBox(window, 240, 100, 800, 520, sf::Color(20, 18, 18), sf::Color(160, 140, 120));
    drawText(window, "How to Play", 640, 120, 34, sf::Color::White, true);

    std::string lines[7] = {
        "1. Choose a room on the home page to begin.",
        "2. Click things to look at them. Click an item at the bottom, then click something to use it.",
        "3. The mouse cursor changes when something is clickable.",
        "4. When the screen turns red the Dean is coming. Click the glowing hiding spot.",
        "5. Press H if you're stuck.",
        "6. Finish faster to earn 1, 2, or 3 stars.",
        "7. Use the Home button in the top right of any room to return home.",
    };
    for (int i = 0; i < 7; i++) {
        drawWrapped(window, lines[i], 280, 185 + i * 48, 720, 20, sf::Color(220, 220, 220));
    }
    drawButton(590, 540, 100, 50, "Close", 22);
}

void Game::drawProfile() {
    drawBox(window, 0, 0, SCREEN_W, SCREEN_H, sf::Color(0, 0, 0, 160));
    drawBox(window, 240, 100, 800, 520, sf::Color(20, 18, 18), sf::Color(160, 140, 120));

    if (!profile.loggedIn()) {
        drawText(window, "Enter a username to log in", 640, 180, 30, sf::Color::White, true);
        drawBox(window, 390, 270, 500, 60, sf::Color(10, 10, 10), sf::Color(160, 140, 120));
        std::string shown = typedName;
        if ((int)(clock * 2) % 2 == 0) shown += "_";  // blinking cursor
        drawText(window, shown, 410, 282, 28, sf::Color::White);
        drawButton(440, 420, 190, 55, "Log in");
        drawButton(650, 420, 190, 55, "Cancel");
        return;
    }

    drawText(window, "Stats for " + profile.username(), 640, 130, 32, sf::Color::White, true);
    for (int i = 0; i < (int)mapNames.size(); i++) {
        RoomStats s = profile.stats(mapNames[i]);
        std::string line = mapNames[i] + ": ";
        if (s.timesCompleted == 0) {
            line += "not completed yet";
        } else {
            line += std::to_string(s.timesCompleted) + " completion(s), best " + std::to_string(s.bestStars) +
                    " star(s), best time " + formatTime((float)s.bestTimeSec);
        }
        drawText(window, line, 280, 210 + i * 50, 22, sf::Color(220, 220, 220));
    }
    drawButton(440, 540, 190, 55, "Log out");
    drawButton(650, 540, 190, 55, "Close");
}

void Game::drawRoom() {
    Room* room = map->findRoom(map->currentRoom);
    float roomH = INVENTORY_Y - TOP_BAR_H;
    sf::Texture* picture = nullptr;
    if (room != nullptr) picture = getTexture(room->image);

    if (picture != nullptr) {
        drawImage(window, *picture, 0, TOP_BAR_H, SCREEN_W, roomH);
    } else {
        // no picture yet, so draw every hotspot as a labelled box
        drawBox(window, 0, TOP_BAR_H, SCREEN_W, roomH, sf::Color(28, 28, 34));
        for (int i = 0; i < (int)map->hotspots.size(); i++) {
            Hotspot& h = map->hotspots[i];
            if (h.room != map->currentRoom) continue;
            drawBox(window, h.x, h.y, h.w, h.h, sf::Color(60, 55, 50), sf::Color(140, 130, 110));
            drawWrapped(window, h.label, h.x + 8, h.y + 6, h.w - 16, 16, sf::Color(220, 210, 190));
        }
    }

    // highlight whatever the mouse is over and say what clicking will do
    Hotspot* hover = nullptr;
    if (map->messages.size() == 0 && !keypad.isOpen() && mouseY < INVENTORY_Y) hover = hotspotAt(mouseX, mouseY);
    if (hover != nullptr) {
        overClickable = true;
        drawBox(window, hover->x, hover->y, hover->w, hover->h, sf::Color(255, 255, 255, 20),
                sf::Color(240, 220, 120));
        std::string text = hover->label;
        if (selectedItem >= 0) text = "Use " + map->inventory[selectedItem] + " on " + hover->label;
        drawText(window, text, mouseX + 18, mouseY + 12, 18, sf::Color(255, 240, 180));
    }
}

void Game::drawTopBar() {
    drawBox(window, 0, 0, SCREEN_W, TOP_BAR_H, sf::Color(15, 15, 18));
    drawText(window, map->getName(), 15, 8, 20, sf::Color(200, 190, 170));

    Room* room = map->findRoom(map->currentRoom);
    if (room != nullptr) drawText(window, room->title, 560, 8, 20, sf::Color::White, true);

    drawText(window, "H hint   D debug", 840, 12, 14, sf::Color(130, 130, 130));
    sf::Color timeColor = map->timeLeft < 60 ? sf::Color(230, 50, 50) : sf::Color::White;
    drawText(window, formatTime(map->timeLeft), 1070, 6, 24, timeColor);
    drawButton(HOME_X, HOME_Y, HOME_W, HOME_H, "Home", 18);
}

void Game::drawInventory() {
    drawBox(window, 0, INVENTORY_Y, SCREEN_W, SCREEN_H - INVENTORY_Y, sf::Color(22, 20, 18));
    if (map->inventory.size() == 0) {
        drawText(window, "Items you pick up go here. Click one, then click something in the room to use it.",
                 20, INVENTORY_Y + 28, 16, sf::Color(130, 130, 130));
    }
    for (int i = 0; i < (int)map->inventory.size(); i++) {
        float x = 20 + i * 132;
        if (isInside(mouseX, mouseY, x, INVENTORY_Y + 10, 125, 60)) overClickable = true;
        sf::Color outline = selectedItem == i ? sf::Color(250, 210, 60) : sf::Color(110, 100, 90);
        drawBox(window, x, INVENTORY_Y + 10, 125, 60, sf::Color(45, 40, 36), outline, selectedItem == i ? 3 : 2);
        drawWrapped(window, map->inventory[i], x + 8, INVENTORY_Y + 16, 110, 16, sf::Color::White);
    }
}

void Game::drawMessage() {
    if (map->messages.size() == 0) return;
    drawBox(window, 140, 430, 1000, 180, sf::Color(10, 10, 12, 240), sf::Color(150, 130, 100));
    drawWrapped(window, map->messages[0], 165, 450, 950, 22, sf::Color::White);
    drawText(window, "click to continue", 1000, 580, 14, sf::Color(150, 150, 150));
}

void Game::drawDean() {
    float pulse = 0.5f + 0.5f * std::sin(clock * 8);
    drawBox(window, 0, TOP_BAR_H, SCREEN_W, INVENTORY_Y - TOP_BAR_H, sf::Color(140, 0, 0, 60 + (int)(pulse * 50)));
    drawTitle(window, "THE DEAN IS COMING. HIDE!", 640, 60, 64, sf::Color(240, 40, 40));
    drawTitle(window, std::to_string((int)std::ceil(hideTimer)), 640, 140, 80, sf::Color::White);

    Hotspot* spot = map->hidingSpotIn(map->currentRoom);
    if (spot != nullptr) {
        drawBox(window, spot->x, spot->y, spot->w, spot->h, sf::Color(255, 255, 0, 30 + (int)(pulse * 60)),
                sf::Color(255, 230, 0), 4);
    }
}

void Game::drawDebug() {
    for (int i = 0; i < (int)map->hotspots.size(); i++) {
        Hotspot& h = map->hotspots[i];
        if (h.room != map->currentRoom) continue;
        drawBox(window, h.x, h.y, h.w, h.h, sf::Color::Transparent, sf::Color::Magenta);
        drawText(window, h.id, h.x + 4, h.y + 4, 14, sf::Color::Magenta);
    }
    std::string pos = std::to_string((int)mouseX) + ", " + std::to_string((int)mouseY);
    drawText(window, pos, mouseX + 15, mouseY - 25, 16, sf::Color::Cyan);
}

void Game::drawEndScreen() {
    bool won = state == WON;
    sf::Texture* background = getTexture(won ? "img/exit_bg.png" : "img/caught_bg.png");
    if (background != nullptr) drawImage(window, *background, 0, 0, SCREEN_W, SCREEN_H);
    drawBox(window, 0, 0, SCREEN_W, SCREEN_H, sf::Color(0, 0, 0, 150));

    if (won) {
        drawTitle(window, "YOU ESCAPED", 640, 70, 110, sf::Color(220, 210, 180));
        drawText(window, "Congratulations! You escaped the " + mapNames[mapIndex] + "!", 640, 230, 28,
                 sf::Color::White, true);
        drawText(window, "Time: " + formatTime(timePlayed), 640, 280, 24, sf::Color::White, true);
        for (int i = 0; i < 3; i++) drawStar(window, 560 + i * 80, 370, 32, i < starsEarned);
        if (!profile.loggedIn())
            drawText(window, "Log in from Profile to save your stars.", 640, 420, 18, sf::Color(170, 170, 170), true);
        drawButton(BUTTON_X, 490, BUTTON_W, BUTTON_H, "Return home", 26);
        drawButton(BUTTON_X, 570, BUTTON_W, BUTTON_H, "Save & Quit", 26);
    } else {
        drawTitle(window, "TIME'S UP", 640, 120, 110, sf::Color(200, 20, 20));
        drawText(window, "The Dean found you in the " + mapNames[mapIndex] + ".", 640, 300, 26, sf::Color::White,
                 true);
        drawButton(BUTTON_X, 490, BUTTON_W, BUTTON_H, "Play again", 26);
        drawButton(BUTTON_X, 570, BUTTON_W, BUTTON_H, "Return home", 26);
    }
}
