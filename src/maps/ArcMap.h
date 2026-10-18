// ArcMap.h
// the ARC (gym): lobby, fitness floor and court, one puzzle in each
#pragma once

#include "../Map.h"
#include "CourtPuzzle.h"
#include "KeycardPuzzle.h"
#include "LockerPuzzle.h"

#include <random>

// override = compiler error if the name doesn't match a function in Map
class ArcMap : public Map {
public:
    ArcMap();
    std::string getName() override;
    void setup() override;
    void onClick(std::string id) override;
    void onUseItem(std::string item, std::string id) override;
    void onKeypadSolved(std::string keypadId) override;
    std::string getHint() override;

private:
    void search(Location place, std::string name);
    void useMachine(std::string id);
    void openLocker(int number);
    void checkSeat(Seat seat);

    // the puzzles hold the rules, this map turns clicks into calls on them
    std::mt19937 rng;  // has to stay above the puzzles, they use it when they're made
    KeyCardPuzzle keycard;
    LockerPuzzle locker;
    CourtPuzzle court;
};
