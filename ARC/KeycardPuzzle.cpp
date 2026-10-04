#include "KeycardPuzzle.h"

KeyCardPuzzle::KeyCardPuzzle(const std::vector<Location>& fragmentLocations)
    : fragmentLocations_(fragmentLocations.begin(), fragmentLocations.end()) {}

void KeyCardPuzzle::viewMap() {
    mapViewed = true;
}

bool KeyCardPuzzle::searchLocation(Location loc) {
    if (!mapViewed) {
        return false;
    }
    if (fragmentLocations_.count(loc) > 0) {
        found_.insert(loc);
        return true;
    }
    return false;
}

bool KeyCardPuzzle::scanCard() {
    if (hasCard()) {
        doorOpen = true;
    }
    return doorOpen;
}

bool KeyCardPuzzle::hasViewedMap() const {
    return mapViewed;
}

bool KeyCardPuzzle::wasFound(Location loc) const {
    return found_.count(loc) > 0;
}

bool KeyCardPuzzle::hasCard() const {
    return found_.size() == fragmentLocations_.size();
}

bool KeyCardPuzzle::isSolved() const {
    return doorOpen;
}

int KeyCardPuzzle::fragmentsFound() const {
    return static_cast<int>(found_.size());
}

int KeyCardPuzzle::fragmentsNeeded() const {
    return static_cast<int>(fragmentLocations_.size());
}