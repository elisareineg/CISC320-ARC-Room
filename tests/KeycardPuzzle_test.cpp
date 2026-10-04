#include <cassert>
#include <iostream>
#include "../ARC/KeycardPuzzle.h"

int main() {
    KeyCardPuzzle puzzle({Location::Reception, Location::MerchStore, Location::Bleachers});

    // Searching before viewing the map finds nothing
    assert(!puzzle.searchLocation(Location::Reception));

    assert(!puzzle.hasViewedMap());
    puzzle.viewMap();
    assert(puzzle.hasViewedMap());

    // Wrong location finds nothing
    assert(!puzzle.searchLocation(Location::Locker));

    // Correct locations, in any order
    assert(puzzle.searchLocation(Location::Bleachers));
    assert(puzzle.searchLocation(Location::Reception));
    assert(puzzle.wasFound(Location::Bleachers));
    assert(!puzzle.wasFound(Location::Locker));
    assert(!puzzle.hasCard());
    assert(!puzzle.scanCard());

    assert(puzzle.searchLocation(Location::MerchStore));
    assert(puzzle.hasCard());
    assert(puzzle.fragmentsFound() == 3);

    // Scanning the card solves the puzzle
    assert(puzzle.scanCard());
    assert(puzzle.isSolved());

    std::cout << "All KeyCardPuzzle tests passed\n";
}