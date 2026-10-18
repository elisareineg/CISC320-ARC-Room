// Draw.h
// helper functions for drawing and loading files
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// window size, the room is drawn between the top bar and the inventory
const float SCREEN_W = 1280;
const float SCREEN_H = 720;
const float TOP_BAR_H = 40;
const float INVENTORY_Y = 640;

bool loadFonts();
sf::Font& mainFont();
sf::Font& titleFont();

// e.g. assetPath("img/menu_bg.png")
std::string assetPath(std::string file);

// returns nullptr if the image is missing
sf::Texture* getTexture(std::string file);

bool isInside(float px, float py, float x, float y, float w, float h);

void drawBox(sf::RenderWindow& window, float x, float y, float w, float h, sf::Color fill,
             sf::Color outline = sf::Color::Transparent, float thickness = 2);
void drawImage(sf::RenderWindow& window, sf::Texture& texture, float x, float y, float w, float h);
void drawText(sf::RenderWindow& window, std::string text, float x, float y, unsigned size, sf::Color color,
              bool centered = false);
void drawTitle(sf::RenderWindow& window, std::string text, float x, float y, unsigned size, sf::Color color);
void drawWrapped(sf::RenderWindow& window, std::string text, float x, float y, float maxWidth, unsigned size,
                 sf::Color color);
// filled = gold star, not filled = dark outline
void drawStar(sf::RenderWindow& window, float x, float y, float radius, bool filled);
