// MapList.cpp
// all the maps in the game, in the order they show up in the menu
#include "../Map.h"
#include "StaufferMap.h"

Map* createMap(int index) {
    switch (index) {
    case 0:
        return new StaufferMap();
    }
    return nullptr;
}

// counts the maps by making them until createMap returns nullptr
int getMapCount() {
    int count = 0;
    Map* map = createMap(count);
    while (map != nullptr) {
        delete map;
        count++;
        map = createMap(count);
    }
    return count;
}
