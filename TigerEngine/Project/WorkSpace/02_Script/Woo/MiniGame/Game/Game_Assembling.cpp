#include "Game_Assembling.h"
#include <iostream>

void Game_Assembling::StartGame()
{
    timer = 0.0f;
    std::cout << "[Mini Game] : Game_Assembling Start!" << std::endl;
}

void Game_Assembling::UpdateGame(float delta)
{
    timer += delta;
    if (timer >= gamePlayTime)
    {
        timer = 0.0f;
        isFinished = true;
        isSuccess = true;
    }
}

void Game_Assembling::EndGame()
{
    std::cout << "[Mini Game] : Game_Assembling End!" << std::endl;
}
