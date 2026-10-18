// Keypad.h
// number pad popup, opened with openKeypad("id", "1234")
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Keypad {
public:
    Keypad();
    void open(std::string id, std::string code);
    void close();
    bool isOpen();
    std::string getId();

    // return true when the right code is entered
    bool click(float x, float y);
    bool press(char button);  // '0' to '9', 'C' for clear, 'E' for enter

    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    bool buttonAt(float x, float y, char& button);
    void buttonRect(int index, float& x, float& y);

    bool opened;
    std::string id;
    std::string code;
    std::string typed;
    float wrongTimer;  // shakes the keypad for a moment after a wrong code
};
