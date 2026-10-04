#ifndef KEYCARDPUZZLE_H
#define KEYCARDPUZZLE_H

#include <set>
#include <vector>

enum class Location {
    Reception,
    MerchStore,
    Bleachers,
    Locker,
};

class KeyCardPuzzle {
public:
    explicit KeyCardPuzzle(const std::vector<Location>& fragmentLocations);

    void viewMap();
    bool searchLocation(Location loc);
    bool scanCard();

    bool hasViewedMap() const;
    bool wasFound(Location loc) const;
    bool hasCard() const;
    bool isSolved() const;
    int fragmentsFound() const;
    int fragmentsNeeded() const;

private:
    std::set<Location> fragmentLocations_;
    std::set<Location> found_;
    bool mapViewed = false;
    bool doorOpen = false;
};

#endif