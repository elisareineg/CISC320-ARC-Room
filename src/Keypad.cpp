// Keypad.cpp
#include "Keypad.h"

#include "Draw.h"

#include <cmath>

// panel position and size
const float PANEL_X = 480;
const float PANEL_Y = 110;
const float PANEL_W = 320;
const float PANEL_H = 480;

// the 12 buttons, read left to right, top to bottom
const char BUTTONS[12] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'C', '0', 'E'};

Keypad::Keypad() {
    opened = false;
    wrongTimer = 0;
}

void Keypad::open(std::string newId, std::string newCode) {
    id = newId;
    code = newCode;
    typed = "";
    wrongTimer = 0;
    opened = true;
}

void Keypad::close() { opened = false; }

bool Keypad::isOpen() { return opened; }

std::string Keypad::getId() { return id; }

void Keypad::buttonRect(int index, float& x, float& y) {
    int col = index % 3;
    int row = index / 3;
    x = PANEL_X + 25 + col * 95;
    y = PANEL_Y + 150 + row * 75;
}

bool Keypad::buttonAt(float x, float y, char& button) {
    for (int i = 0; i < 12; i++) {
        float bx, by;
        buttonRect(i, bx, by);
        if (isInside(x, y, bx, by, 80, 60)) {
            button = BUTTONS[i];
            return true;
        }
    }
    return false;
}

bool Keypad::click(float x, float y) {
    // clicking outside the keypad closes it
    if (!isInside(x, y, PANEL_X, PANEL_Y, PANEL_W, PANEL_H)) {
        close();
        return false;
    }
    char button;
    if (buttonAt(x, y, button)) return press(button);
    return false;
}

bool Keypad::press(char button) {
    if (button == 'C') {
        typed = "";
    } else if (button == 'E') {
        if (typed == code) {
            close();
            return true;
        }
        typed = "";
        wrongTimer = 0.5f;
    } else if (typed.size() < code.size()) {
        typed += button;
    }
    return false;
}

void Keypad::update(float dt) {
    if (wrongTimer > 0) wrongTimer -= dt;
}

void Keypad::draw(sf::RenderWindow& window) {
    // darken the room behind the keypad
    drawBox(window, 0, 0, SCREEN_W, SCREEN_H, sf::Color(0, 0, 0, 150));

    float shake = 0;
    if (wrongTimer > 0) shake = std::sin(wrongTimer * 60) * 8;
    float x = PANEL_X + shake;

    drawBox(window, x, PANEL_Y, PANEL_W, PANEL_H, sf::Color(35, 35, 40), sf::Color(120, 120, 130), 3);
    drawText(window, "Enter the code", x + PANEL_W / 2, PANEL_Y + 15, 22, sf::Color::White, true);

    // the screen shows a _ for every digit still to type
    std::string screen = "";
    for (int i = 0; i < (int)code.size(); i++) {
        if (i < (int)typed.size()) screen += typed[i];
        else screen += "_";
        screen += " ";
    }
    sf::Color screenColor = wrongTimer > 0 ? sf::Color(230, 60, 60) : sf::Color(90, 230, 120);
    drawBox(window, x + 25, PANEL_Y + 60, PANEL_W - 50, 70, sf::Color(10, 20, 10));
    drawText(window, screen, x + PANEL_W / 2, PANEL_Y + 72, 38, screenColor, true);

    for (int i = 0; i < 12; i++) {
        float bx, by;
        buttonRect(i, bx, by);
        bx += shake;
        drawBox(window, bx, by, 80, 60, sf::Color(70, 70, 80), sf::Color(150, 150, 160));
        std::string label(1, BUTTONS[i]);
        if (BUTTONS[i] == 'E') label = "OK";
        drawText(window, label, bx + 40, by + 14, 26, sf::Color::White, true);
    }
    drawText(window, "Esc or click outside to close", x + PANEL_W / 2, PANEL_Y + PANEL_H - 30, 16,
             sf::Color(170, 170, 170), true);
}
