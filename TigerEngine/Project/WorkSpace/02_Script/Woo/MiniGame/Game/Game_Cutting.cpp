#include "Game_Cutting.h"
#include <iostream>

void Game_Cutting::StartGame()
{
    timer = 0.0f;
    std::cout << "[Mini Game] : Game_Cutting Start!" << std::endl;
}

void Game_Cutting::UpdateGame(float delta)
{
    timer += delta;
    if (timer >= gamePlayTime)
    {
        timer = 0.0f;
        isFinished = true;
        isSuccess = true;
    }
}

void Game_Cutting::EndGame()
{
    std::cout << "[Mini Game] : Game_Cutting End!" << std::endl;
}
