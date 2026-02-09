#pragma once
#include <pch.h>

/*
    [ Cook Minigame Interface ]

    미니게임은 총 3가지입니다.
    1) Game_Cutting
    2) Game_FireControl
    3) Game_Assembling
*/

class IMiniGame
{
protected:
    // test
    float gamePlayTime = 5.0f;
    float timer = 0.0f;
    bool isFinished = false;
    bool isSuccess = false;

    // == mini game anim
    std::vector<std::string> currAnimPathes; // 미니 게임 애니메이션 패스 컨테이너 ( 해당 컨테이너에 있는 패스만큼 애니메이션 실행함 )
    int currAnimIndex = 0;
    float changeAnimTime = 0.4f;    // 이미지 교체할 때 까지 걸리는 시간
    float animTimer = 0.4f;         // 애니메이션 실행 시간
public:
    IMiniGame() = default;
    virtual ~IMiniGame() = default;

    virtual void StartGame() = 0;
    virtual void UpdateGame(float delta) = 0;
    virtual void EndGame() = 0;

    bool IsFinished() { return isFinished; }
    bool IsSuccess() { return isSuccess; }

    // == mini game anim
    virtual void UpdateAnimation(float delta) = 0; // 애니메이션 업데이트 함수
};

