#pragma once

/*
    [ Cook Minigame Interface ]

    미니게임은 총 3가지입니다.
    1) Game_Cutting
    2) Game_FireControl
    3) Game_Assembling
*/

class IMiniGame
{
public:
    IMiniGame() = default;
    virtual ~IMiniGame() = default;

    virtual void StartGame() = 0;
    virtual void UpdateGame(float delta) = 0;
    virtual void EndGame() = 0;
};

