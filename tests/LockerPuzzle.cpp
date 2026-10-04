#include <cassert>
#include <iostream>
#include "../ARC/LockerPuzzle.h"

int main() {
    std::mt19937 rng(42);   // fixed seed, same result every run
    auto p = LockerPuzzle::withRandomCode(Equipment::Rower, {3, 1, 4}, 7, rng); // auto tells the compiler to work out a variable's type from the value it's given

    assert(p.isCorrectEquipment(Equipment::Rower));
    assert(!p.isCorrectEquipment(Equipment::Treadmill));
    assert(!p.isCorrectEquipment(Equipment::Bike));
    assert(!p.isCorrectEquipment(Equipment::Weights));
    std::cout << "PASS: only the correct equipment is clickable\n";

    assert(p.isCorrectLocker(7));
    assert(!p.isCorrectLocker(1));
    assert(!p.isCorrectLocker(8));
    std::cout << "PASS: only the correct locker is clickable\n";

    assert(p.displayedCode().empty());
    std::cout << "PASS: code is hidden before machine is solved\n";

    assert(!p.enterLockerCode({0, 0, 0, 0}));      // locked before step 1
    std::cout << "PASS: locker rejects code before machine is solved\n";

    // Wrong settings must be tried before the right ones: once the machine
    // is solved it stays solved, so enterMachineSettings returns true after that
    assert(!p.enterMachineSettings({5, 6, 9}));
    assert(!p.machineShowsCode());
    std::cout << "PASS: wrong machine settings are rejected\n";

    assert(p.enterMachineSettings({3, 1, 4}));
    assert(p.machineShowsCode());
    std::cout << "PASS: correct machine settings show the code\n";

    assert(!p.enterLockerCode({-1, -1, -1, -1}));  // can never match a 0-9 code
    assert(!p.isSolved());
    std::cout << "PASS: wrong locker code is rejected\n";

    assert(p.enterLockerCode(p.displayedCode()));
    assert(p.isSolved());
    std::cout << "PASS: displayed code opens the locker\n";

    std::cout << "All LockerPuzzle tests passed\n";
}
