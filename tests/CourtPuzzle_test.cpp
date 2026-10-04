#include <cassert>
#include <iostream>
#include "../ARC/CourtPuzzle.h"

int main() {
    Seat correct{Side::left, 12};
    CourtPuzzle p(correct);

    assert(p.correctSeat() == correct);
    std::cout << "PASS: puzzle stores the seat it was given\n";

    assert(!p.bleachersOpen());
    assert(!p.keyFound());
    assert(!p.isSolved());
    std::cout << "PASS: nothing is unlocked at the start\n";

    // Steps must be locked until the one before them is done
    assert(!p.selectSeat(correct));
    assert(!p.keyFound());
    std::cout << "PASS: correct seat is rejected before the ball is shot\n";

    assert(!p.useKey());
    assert(!p.isSolved());
    std::cout << "PASS: exit stays shut before the key is found\n";

    assert(p.shootBall());
    assert(p.bleachersOpen());
    std::cout << "PASS: shooting the ball opens the bleachers\n";

    assert(!p.selectSeat({Side::right, 12}));  // right number, wrong side
    assert(!p.selectSeat({Side::left, 13}));   // right side, wrong number
    assert(!p.keyFound());
    std::cout << "PASS: wrong seats are rejected\n";

    assert(p.selectSeat(correct));
    assert(p.keyFound());
    std::cout << "PASS: correct seat reveals the key\n";

    assert(!p.isSolved());
    std::cout << "PASS: finding the key alone does not open the exit\n";

    // Once found, the key stays found even if the player clicks another seat
    assert(p.selectSeat({Side::right, 1}));
    assert(p.keyFound());
    std::cout << "PASS: wrong seat after finding the key does not lose it\n";

    assert(p.useKey());
    assert(p.isSolved());
    std::cout << "PASS: using the key opens the exit\n";

    // Random seats: always a real seat, and the same seed gives the same seat
    std::mt19937 rng(42);
    for (int i = 0; i < 100; i++) {
        Seat s = CourtPuzzle::withRandomSeat(rng, 20).correctSeat();
        assert(s.number >= 1 && s.number <= 20);
    }
    std::cout << "PASS: random seats are always between 1 and seatsPerSide\n";

    std::mt19937 a(7), b(7);
    assert(CourtPuzzle::withRandomSeat(a, 20).correctSeat() == CourtPuzzle::withRandomSeat(b, 20).correctSeat());
    std::cout << "PASS: same seed gives the same seat\n";

    // Full playthrough with a random seat, reading the answer the way the note would
    auto r = CourtPuzzle::withRandomSeat(rng, 20);
    r.shootBall();
    assert(r.selectSeat(r.correctSeat()));
    assert(r.useKey());
    assert(r.isSolved());
    std::cout << "PASS: random seat puzzle can be solved using correctSeat()\n";

    std::cout << "All CourtPuzzle tests passed\n";
}
