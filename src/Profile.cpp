// Profile.cpp
#include "Profile.h"

#include <fstream>

void Profile::login(const std::string& name) {
    username_ = name;
    load();
}

void Profile::logout() {
    save();
    username_.clear();
    stats_.clear();
}

void Profile::recordResult(const std::string& room, double seconds, int stars) {
    RoomStats& s = stats_[room];
    s.timesCompleted++;
    if (stars > s.bestStars) s.bestStars = stars;
    if (s.bestTimeSec < 0 || seconds < s.bestTimeSec) s.bestTimeSec = seconds;
}

RoomStats Profile::stats(const std::string& room) const {
    std::map<std::string, RoomStats>::const_iterator it = stats_.find(room);
    if (it == stats_.end()) return RoomStats();
    return it->second;
}

// each room is two lines: the room name, then completed / stars / time
void Profile::save() const {
    if (!loggedIn()) return;
    std::ofstream out(username_ + ".gaeljail");
    std::map<std::string, RoomStats>::const_iterator it;
    for (it = stats_.begin(); it != stats_.end(); it++) {
        out << it->first << "\n"
            << it->second.timesCompleted << " " << it->second.bestStars << " " << it->second.bestTimeSec << "\n";
    }
}

void Profile::load() {
    stats_.clear();
    std::ifstream in(username_ + ".gaeljail");
    std::string room;
    RoomStats s;
    while (std::getline(in, room) && in >> s.timesCompleted >> s.bestStars >> s.bestTimeSec) {
        stats_[room] = s;
        in.ignore(1000, '\n');
    }
}

int calculateStars(double seconds) {
    if (seconds <= 10 * 60) return 3;  // 10 min or less
    if (seconds <= 20 * 60) return 2;  // 20 min or less
    return 1;                          // finished at all
}
