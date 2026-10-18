// Profile.h
// Player profile: username and best results for each room, saved to <username>.gaeljail
#pragma once

#include <map>
#include <string>

struct RoomStats {
    int timesCompleted = 0;
    int bestStars = 0;          // 0 = never completed
    double bestTimeSec = -1.0;  // -1 = never completed
};

class Profile {
public:
    bool loggedIn() const { return !username_.empty(); }
    const std::string& username() const { return username_; }

    void login(const std::string& name);
    void logout();

    // room is the map's name, e.g. "Library"
    void recordResult(const std::string& room, double seconds, int stars);
    RoomStats stats(const std::string& room) const;

    // Simple text-file persistence: one file per user
    void save() const;

private:
    void load();

    std::string username_;
    std::map<std::string, RoomStats> stats_;
};

// Thresholds are placeholders; tune per room once the rooms are built.
int calculateStars(double seconds);
