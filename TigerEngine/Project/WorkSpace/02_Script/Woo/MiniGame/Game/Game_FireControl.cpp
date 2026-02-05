#include "Game_FireControl.h"
#include <iostream>

void Game_FireControl::StartGame()
{
    timer = 0.0f;
    std::cout << "[Mini Game] : Game_FireControl Start!" << std::endl;
}

void Game_FireControl::UpdateGame(float delta)
{
    timer += delta;
    if (timer >= gamePlayTime)
    {
        timer = 0.0f;
        isFinished = true;
        isSuccess = true;
    }
}

void Game_FireControl::EndGame()
{
    std::cout << "[Mini Game] : Game_FireControl End!" << std::endl;

}
