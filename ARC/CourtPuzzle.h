#include <random>
#include <vector>

#ifndef COURTPUZZLE_H
#define COURTPUZZLE_H

enum class Side
{
    left,
    right
};

struct Seat
{
    Side side;
    int number;
    bool operator==(const Seat &seatSide) const
    {
        return side == seatSide.side && number == seatSide.number;
    }
};

class CourtPuzzle
{
public:
    explicit CourtPuzzle(Seat correctSeat);

    // Game uses this; tests use the constructor above with a known seat
    static CourtPuzzle withRandomSeat(std::mt19937 &rng, int seatsPerSide);

    // ArcRoom draws the locker note from this, so the note always matches the answer
    Seat correctSeat() const;

    bool shootBall(); // step 1: ball goes in, bleachers become clickable
    bool bleachersOpen() const;
    bool selectSeat(Seat seat);    // step 2: only works after the shot
    bool keyFound() const;

    bool useKey();                 // step 3: only works once the key is found
    bool isSolved() const;         // exit door is open

private:
    Seat correctSeat_;
    bool ballShot_ = false;
    bool keyFound_ = false;
    bool exitOpen_ = false;
};
#endif