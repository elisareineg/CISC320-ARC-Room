// Draw.cpp
#include "Draw.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

static sf::Font normalFont;
static sf::Font bigFont;
static std::string assetsFolder = "";
static std::map<std::string, sf::Texture> textures;
static std::set<std::string> missingTextures;

static bool fileExists(std::string path) {
    std::ifstream file(path);
    return file.good();
}

// look for assets/ in this folder and a couple of folders up
static void findAssetsFolder() {
    std::string tries[] = {"assets/", "../assets/", "../../assets/"};
    for (int i = 0; i < 3; i++) {
        if (fileExists(tries[i] + "img/menu_bg.png")) {
            assetsFolder = tries[i];
            return;
        }
    }
    std::cout << "Could not find the assets folder. Run the game from the project folder.\n";
    assetsFolder = "assets/";
}

// uses the first font file that exists
static bool openFirstFont(sf::Font& font, std::vector<std::string> files) {
    for (int i = 0; i < (int)files.size(); i++) {
        if (fileExists(files[i]) && font.openFromFile(files[i])) return true;
    }
    return false;
}

bool loadFonts() {
    findAssetsFolder();
    // use a font from assets/ if there is one, otherwise a Windows or Mac font
    if (!openFirstFont(normalFont, {assetPath("font.ttf"), "C:/Windows/Fonts/arial.ttf",
                                    "/System/Library/Fonts/Supplemental/Arial.ttf", "/Library/Fonts/Arial.ttf"})) {
        std::cout << "Could not load a font. Put any .ttf file in assets/ and name it font.ttf\n";
        return false;
    }
    if (!openFirstFont(bigFont, {assetPath("title.ttf"), "C:/Windows/Fonts/CHILLER.TTF",
                                 "C:/Windows/Fonts/impact.ttf", "/System/Library/Fonts/Supplemental/Impact.ttf"})) {
        bigFont = normalFont;
    }
    return true;
}

sf::Font& mainFont() { return normalFont; }
sf::Font& titleFont() { return bigFont; }

std::string assetPath(std::string file) { return assetsFolder + file; }

sf::Texture* getTexture(std::string file) {
    if (file == "" || missingTextures.count(file) > 0) return nullptr;
    if (textures.count(file) > 0) return &textures[file];

    sf::Texture texture;
    if (!fileExists(assetPath(file)) || !texture.loadFromFile(assetPath(file))) {
        // remember it so we don't try to load it again every frame
        missingTextures.insert(file);
        return nullptr;
    }
    texture.setSmooth(true);
    textures[file] = texture;
    return &textures[file];
}

bool isInside(float px, float py, float x, float y, float w, float h) {
    return px >= x && px <= x + w && py >= y && py <= y + h;
}

void drawBox(sf::RenderWindow& window, float x, float y, float w, float h, sf::Color fill, sf::Color outline,
             float thickness) {
    sf::RectangleShape box({w, h});
    box.setPosition({x, y});
    box.setFillColor(fill);
    if (outline != sf::Color::Transparent) {
        box.setOutlineColor(outline);
        box.setOutlineThickness(thickness);
    }
    window.draw(box);
}

void drawImage(sf::RenderWindow& window, sf::Texture& texture, float x, float y, float w, float h) {
    sf::Sprite sprite(texture);
    sprite.setPosition({x, y});
    // stretch the picture to fill the box
    sprite.setScale({w / texture.getSize().x, h / texture.getSize().y});
    window.draw(sprite);
}

static void drawWithFont(sf::RenderWindow& window, sf::Font& font, std::string text, float x, float y,
                         unsigned size, sf::Color color, bool centered) {
    sf::Text label(font, text, size);
    label.setFillColor(color);
    label.setOutlineColor(sf::Color::Black);
    label.setOutlineThickness(1);
    if (centered) {
        float width = label.getLocalBounds().size.x;
        x = x - width / 2;
    }
    label.setPosition({x, y});
    window.draw(label);
}

void drawText(sf::RenderWindow& window, std::string text, float x, float y, unsigned size, sf::Color color,
              bool centered) {
    drawWithFont(window, normalFont, text, x, y, size, color, centered);
}

void drawTitle(sf::RenderWindow& window, std::string text, float x, float y, unsigned size, sf::Color color) {
    drawWithFont(window, bigFont, text, x, y, size, color, true);
}

void drawWrapped(sf::RenderWindow& window, std::string text, float x, float y, float maxWidth, unsigned size,
                 sf::Color color) {
    // split the text into words
    std::vector<std::string> words;
    std::string word = "";
    for (int i = 0; i < (int)text.size(); i++) {
        if (text[i] == ' ') {
            words.push_back(word);
            word = "";
        } else {
            word += text[i];
        }
    }
    words.push_back(word);

    // add words to the line until it gets too wide, then start a new line
    std::string line = "";
    float lineHeight = size * 1.3f;
    for (int i = 0; i < (int)words.size(); i++) {
        std::string longer = line == "" ? words[i] : line + " " + words[i];
        sf::Text test(normalFont, longer, size);
        if (test.getLocalBounds().size.x > maxWidth && line != "") {
            drawText(window, line, x, y, size, color);
            y += lineHeight;
            line = words[i];
        } else {
            line = longer;
        }
    }
    drawText(window, line, x, y, size, color);
}

void drawStar(sf::RenderWindow& window, float x, float y, float radius, bool filled) {
    // 10 points going around the centre, every second one pulled in to make the star shape
    sf::ConvexShape star(10);
    for (int i = 0; i < 10; i++) {
        float angle = -3.14159f / 2 + i * 3.14159f / 5;
        float r = (i % 2 == 0) ? radius : radius * 0.45f;
        star.setPoint(i, {x + std::cos(angle) * r, y + std::sin(angle) * r});
    }
    star.setFillColor(filled ? sf::Color(240, 200, 60) : sf::Color(40, 40, 40));
    star.setOutlineColor(sf::Color(240, 200, 60));
    star.setOutlineThickness(2);
    window.draw(star);
}
