#pragma once
#include "IMinigame.h"

class Game_FireControl : public IMiniGame
{
public:
    Game_FireControl() = default;
    ~Game_FireControl() override = default;

    void StartGame() override;
    void UpdateGame(float delta)override;
    void EndGame() override;
};



