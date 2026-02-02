#pragma once
#include "IMinigame.h"

class Game_Cutting : public IMiniGame
{
public:
    Game_Cutting() = default;
    ~Game_Cutting() override = default;

    void StartGame() override;
    void UpdateGame(float delta)override;
    void EndGame() override;
};

