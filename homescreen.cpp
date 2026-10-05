// Gael Jail - Homepage logic (no UI, console stand-in for button clicks)
// Compile: g++ -std=c++17 gael_jail_homepage.cpp -o gael_jail

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <limits>

// ---------------------------------------------------------------- Rooms ----
enum class Room { Gym = 0, Cafeteria = 1, Library = 2 };

const std::vector<Room> ALL_ROOMS = { Room::Gym, Room::Cafeteria, Room::Library };

std::string roomName(Room r) {
    switch (r) {
        case Room::Gym:       return "Gym / Rec Centre";
        case Room::Cafeteria: return "Cafeteria";
        case Room::Library:   return "Library";
    }
    return "Unknown";
}

enum class Hint { Easy, Medium, Hard };

// ------------------------------------------------------------- Profile ----
struct RoomStats {
    int timesCompleted = 0;
    int bestStars = 0;          // 0 = never completed
    double bestTimeSec = -1.0;  // -1 = never completed
};

class Profile {
public:
    bool loggedIn() const { return !username_.empty(); }
    const std::string& username() const { return username_; }

    void login(const std::string& name) {
        username_ = name;
        load();
    }
    void logout() { save(); username_.clear(); stats_.clear(); }

    void recordResult(Room r, double seconds, int stars) {
        RoomStats& s = stats_[static_cast<int>(r)];
        s.timesCompleted++;
        if (stars > s.bestStars) s.bestStars = stars;
        if (s.bestTimeSec < 0 || seconds < s.bestTimeSec) s.bestTimeSec = seconds;
    }

    void printStats() const {
        if (!loggedIn()) { std::cout << "Log in to see your stats.\n"; return; }
        std::cout << "--- Stats for " << username_ << " ---\n";
        for (Room r : ALL_ROOMS) {
            auto it = stats_.find(static_cast<int>(r));
            std::cout << roomName(r) << ": ";
            if (it == stats_.end() || it->second.timesCompleted == 0) {
                std::cout << "not completed yet\n";
            } else {
                const RoomStats& s = it->second;
                std::cout << s.timesCompleted << " completion(s), best "
                          << s.bestStars << " star(s), best time "
                          << s.bestTimeSec << "s\n";
            }
        }
    }

    // Simple text-file persistence: one file per user
    void save() const {
        if (!loggedIn()) return;
        std::ofstream out(username_ + ".gaeljail");
        for (const auto& [room, s] : stats_)
            out << room << " " << s.timesCompleted << " "
                << s.bestStars << " " << s.bestTimeSec << "\n";
    }

private:
    void load() {
        stats_.clear();
        std::ifstream in(username_ + ".gaeljail");
        int room;
        RoomStats s;
        while (in >> room >> s.timesCompleted >> s.bestStars >> s.bestTimeSec)
            stats_[room] = s;
    }

    std::string username_;
    std::map<int, RoomStats> stats_;
};

// ------------------------------------------------------- Star rating ----
// Thresholds are placeholders; tune per room once the rooms are built.
int calculateStars(double seconds) {
    if (seconds <= 10 * 60) return 3;   // 10 min or less
    if (seconds <= 20 * 60) return 2;   // 20 min or less
    return 1;                           // finished at all
}

// -------------------------------------------------------- Homepage ----
// Everything on the homepage that can be clicked.
enum class Button { RoomGym, RoomCafeteria, RoomLibrary, Profile, HowToPlay, SaveAndQuit };

class Homepage {
public:
    static const std::string TITLE;
    static const std::string PROMPT;

    // Cursor logic: true = cursor should change to a pointer.
    // (Per the design, we can "fake out" players by also returning true
    // for decorative spots later.)
    bool isClickable(Button) const { return true; }

    // Top-right buttons: 3 rooms, profile, how to play, X
    void handleClick(Button b) {
        switch (b) {
            case Button::RoomGym:       startRoom(Room::Gym);       break;
            case Button::RoomCafeteria: startRoom(Room::Cafeteria); break;
            case Button::RoomLibrary:   startRoom(Room::Library);   break;
            case Button::Profile:       openProfile();              break;
            case Button::HowToPlay:     showHowToPlay();            break;
            case Button::SaveAndQuit:   saveAndQuit();              break;
        }
    }

    bool running() const { return running_; }

    void show() const {
        std::cout << "\n=============== " << TITLE << " ===============\n"
                  << PROMPT << "\n"
                  << "[1] " << roomName(Room::Gym) << "\n"
                  << "[2] " << roomName(Room::Cafeteria) << "\n"
                  << "[3] " << roomName(Room::Library) << "\n"
                  << "[P] Profile   [H] How to Play   [X] Save & Quit\n";
    }

private:
    Profile profile_;
    bool running_ = true;

    void showHowToPlay() const {
        std::cout << "\n--- How to Play ---\n"
                  << "1. Choose a room on the map to begin.\n"
                  << "2. Each room has an easy, medium, and hard clue for every puzzle.\n"
                  << "3. The mouse cursor changes when something is clickable.\n"
                  << "4. Finish faster to earn 1, 2, or 3 stars.\n"
                  << "5. Use the button in the top right of any room to return home.\n"
                  << "(Close this pop-up with the X.)\n";
    }

    void openProfile() {
        if (!profile_.loggedIn()) {
            std::cout << "Enter username to log in (blank to cancel): ";
            std::string name;
            std::getline(std::cin, name);
            if (name.empty()) return;
            profile_.login(name);
            std::cout << "Logged in as " << name << ".\n";
        }
        profile_.printStats();
    }

    void saveAndQuit() {
        profile_.save();
        std::cout << "Progress saved. Goodbye!\n";
        running_ = false;
    }

    // Placeholder: swap the body of playRoom() for the real room logic.
    void startRoom(Room r) {
        std::cout << "\nEntering " << roomName(r) << "...\n";
        auto start = std::chrono::steady_clock::now();

        bool completed = playRoom(r);   // returns false if player quit to home

        double seconds = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - start).count();

        if (completed) showCongratulations(r, seconds);
        else           std::cout << "Returned to home.\n";
    }

    bool playRoom(Room) {
        // TODO: real room logic goes here.
        // Return true when the last clue is solved, false if the player
        // clicked the top-right "return home / quit" button.
        std::cout << "(Room not built yet. Type 'done' to finish, anything else to quit.) ";
        std::string in;
        std::getline(std::cin, in);
        return in == "done";
    }

    void showCongratulations(Room r, double seconds) {
        int stars = calculateStars(seconds);
        std::cout << "\n*** Congratulations! You escaped the " << roomName(r) << "! ***\n"
                  << "Time: " << seconds << "s   Stars: " << stars << "/3\n";
        if (profile_.loggedIn()) profile_.recordResult(r, seconds, stars);

        std::cout << "[H] Return home to play again   [X] Save & Quit: ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "X" || choice == "x") saveAndQuit();
        // otherwise fall through back to the homepage loop
    }
};

const std::string Homepage::TITLE  = "GAEL JAIL";
const std::string Homepage::PROMPT = "Choose a room to begin";

// ------------------------------------------------------------- main ----
// Console input stands in for mouse clicks until the UI exists.
int main() {
    Homepage home;
    while (home.running()) {
        home.show();
        std::cout << "> ";
        std::string in;
        if (!std::getline(std::cin, in)) break;
        if (in.empty()) continue;

        switch (in[0]) {
            case '1': home.handleClick(Button::RoomGym);       break;
            case '2': home.handleClick(Button::RoomCafeteria); break;
            case '3': home.handleClick(Button::RoomLibrary);   break;
            case 'P': case 'p': home.handleClick(Button::Profile);     break;
            case 'H': case 'h': home.handleClick(Button::HowToPlay);   break;
            case 'X': case 'x': home.handleClick(Button::SaveAndQuit); break;
            default: std::cout << "Not a button.\n";
        }
    }
    return 0;
}
