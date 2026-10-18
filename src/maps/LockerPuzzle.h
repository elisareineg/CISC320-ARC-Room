#include <random>
#include <vector>
#ifndef LOCKERPUZZLE_H
#define LOCKERPUZZLE_H

enum class Equipment {
    Treadmill,
    Bike,
    Rower,
    Weights,
};

class LockerPuzzle {
public:
    LockerPuzzle(Equipment correctEquipment, const std::vector<int>& machineSettings,
                 int correctLocker, const std::vector<int>& lockerCode);

    // Game uses this; tests use the constructor above with a known code
    static LockerPuzzle withRandomCode(Equipment correctEquipment, const std::vector<int>& machineSettings,
                                       int correctLocker, std::mt19937& rng, int codeLength = 4);

    bool isCorrectEquipment(Equipment equipment) const; // only this equipment is clickable
    bool isCorrectLocker(int locker) const;             // only this locker is clickable

    bool enterMachineSettings(const std::vector<int>& settings);
    bool machineShowsCode() const;
    std::vector<int> displayedCode() const;   // empty until machine is solved
    bool enterLockerCode(const std::vector<int>& code);
    bool isSolved() const;

private:
    Equipment correctEquipment_;
    std::vector<int> machineSettings_;
    int correctLocker_;
    std::vector<int> lockerCode_;
    bool machineDone_ = false;
    bool lockerOpen_ = false;
};
#endif
