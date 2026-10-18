// StaufferMap.h
// the library (Stauffer), 3 floors
#pragma once

#include "../Map.h"

// override = compiler error if the name doesn't match a function in Map
class StaufferMap : public Map {
public:
    std::string getName() override;
    void setup() override;
    void onClick(std::string id) override;
    void onUseItem(std::string item, std::string id) override;
    void onKeypadSolved(std::string keypadId) override;
    std::string getHint() override;
};
