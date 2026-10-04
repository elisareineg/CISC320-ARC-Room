#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include "ArcRoom.h"

// basic SFML window script, will ge adjusted

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Escape Room");
    std::unique_ptr<Room> room = std::make_unique<ArcRoom>();

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            room->handleEvent(*event);
        }

        room->update();
        if (room->isSolved()) {
            std::cout << "Room solved!\n";
            window.close();
        }

        window.clear();
        room->draw(window);
        window.display();
    }
}

