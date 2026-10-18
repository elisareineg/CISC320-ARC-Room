#include "CourtPuzzle.h"

CourtPuzzle::CourtPuzzle(Seat correctSeat)
    : correctSeat_(correctSeat) {}

CourtPuzzle CourtPuzzle::withRandomSeat(std::mt19937 &rng, int seatsPerSide)
{
    std::uniform_int_distribution<int> side(0, 1);
    std::uniform_int_distribution<int> number(1, seatsPerSide); // seats numbered from 1
    Seat seat{side(rng) == 0 ? Side::left : Side::right, number(rng)};
    return CourtPuzzle(seat);
}

Seat CourtPuzzle::correctSeat() const
{
    return correctSeat_;
}

bool CourtPuzzle::shootBall() {
    ballShot_ = true;
    return ballShot_;
}

bool CourtPuzzle::bleachersOpen() const {
    return ballShot_;
}

bool CourtPuzzle::selectSeat(Seat seat) {
    if (bleachersOpen()) {
        if (seat == correctSeat_) {
            keyFound_ = true;
        }
    }
    return keyFound_;
}

bool CourtPuzzle::keyFound() const {
    return keyFound_;
}

bool CourtPuzzle::useKey() {
    if (keyFound()) {
        exitOpen_ = true;
    }
    return exitOpen_;
}

bool CourtPuzzle::isSolved() const {
    return exitOpen_;
}