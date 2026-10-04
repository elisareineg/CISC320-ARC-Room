#ifndef ROOM_H
#define ROOM_H

#include <SFML/Graphics.hpp>

class Room {
public:
    virtual ~Room() = default;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual bool isSolved() const = 0;
};

#endif

// to be adjusted further