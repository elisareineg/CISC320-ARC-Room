#include "LockerPuzzle.h"

LockerPuzzle::LockerPuzzle(Equipment correctEquipment, const std::vector<int>& machineSettings,
                           int correctLocker, const std::vector<int>& lockerCode)
    : correctEquipment_(correctEquipment), machineSettings_(machineSettings),
      correctLocker_(correctLocker), lockerCode_(lockerCode) {}

LockerPuzzle LockerPuzzle::withRandomCode(Equipment correctEquipment, const std::vector<int>& machineSettings,
                                          int correctLocker, std::mt19937& rng, int codeLength) {
    std::uniform_int_distribution<int> digit(0, 9);
    std::vector<int> code(codeLength);
    for (int& d : code) {
        d = digit(rng);
    }
    return LockerPuzzle(correctEquipment, machineSettings, correctLocker, code);
}

bool LockerPuzzle::isCorrectEquipment(Equipment equipment) const {
    return equipment == correctEquipment_;
}

bool LockerPuzzle::isCorrectLocker(int locker) const {
    return locker == correctLocker_;
}

bool LockerPuzzle::enterMachineSettings(const std::vector<int>& settings) { // check entered code matches machine code
    if (settings == machineSettings_) {
        machineDone_ = true;
    }
    return machineDone_;
}

std::vector<int> LockerPuzzle::displayedCode() const { 
    return machineDone_ ? lockerCode_ : std::vector<int>{};
}

bool LockerPuzzle::enterLockerCode(const std::vector<int>& code) {
    if (!machineDone_) {
        return false;          // step 2 is locked until step 1 is done
    }
    if (code == lockerCode_) {
        lockerOpen_ = true;
    }
    return lockerOpen_;
}


bool LockerPuzzle::machineShowsCode() const { // checks if step 1 is done
    return machineDone_;
}

 

bool LockerPuzzle::isSolved() const { // locker is open
    return lockerOpen_;
}

