#pragma once
#include "IMinigame.h"

class Game_Assembling : public IMiniGame
{
public:
    Game_Assembling() = default;
    ~Game_Assembling() override = default;

    void StartGame() override;
    void UpdateGame(float delta)override;
    void EndGame() override;
};

