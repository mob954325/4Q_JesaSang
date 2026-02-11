#pragma once
#include "IMinigame.h"
#include <directxtk/Keyboard.h>

using namespace DirectX;
class RectTransform;
class Image;
class GameObject;

/*
    [ 불로 지지기 컨트롤 게임 ]

    스듀 낚시임
    음식 포인트가 좌우로(랜덤) 움직이는데 (스듀 낚시스타일)
    스페이스바 홀딩으로 해당 음식을 따라다녀야함 (영역 체크)
   
    Food
     움직이는 영역 (x) 200 ~ 800
     size (x) 70

    Spacebar
      움직이는 영역 (x) 200 ~ 800
      size (x) 150

    - 게임 성공 : 게이지가 다 차면 성공
    - 게임 실패 : 게이지가 다 떨어지면 실패
*/

class Game_FireControl : public IMiniGame
{
private:
    // UI
    RectTransform* foodPoint = nullptr;  // food
    RectTransform* spacbar = nullptr;    // player bar
    Image* gauge = nullptr;              // fill (0~1)

    GameObject* spacebarHold = nullptr;

    // --- data ------------------------------
    float xMin = 200.0f;
    float xMax = 800.0f;

    // food
    float foodSpeedMin = 260.0f;   // 목표점으로 이동 속도 랜덤 범위
    float foodSpeedMax = 520.0f;
    float foodStopMin = 0.22f;     // 멈춤 시간 랜덤 범위
    float foodStopMax = 0.45f;
    float foodTargetEps = 2.0f;

    // bar control
    float barVel = 0.0f;
    float barAccelUp = 2200.0f;    // 홀딩 가속
    float barAccelDown = 2600.0f;  // 미홀딩 가속(떨어짐)
    float barMaxVelUp = 900.0f;
    float barMaxVelDown = 900.0f;
    float barFriction = 0.12f;

    // gauge
    float gaugeUpPerSec = 0.35f;    // 맞추면 차는 속도
    float gaugeDownPerSec = 0.15f;  // 빗나가면 닳는 속도
    float gaugeValue = 0.5f;        // 초반 시작 게이지
     
    // ---------- Runtime ----------
    DirectX::Keyboard::Keys spaceKey = DirectX::Keyboard::Keys::Space;

    // food state
    float foodTargetX = 500.0f;
    float foodSpeed = 400.0f;
    bool  foodStopped = false;
    float foodStopTimer = 0.0f;
    float foodStopDuration = 0.2f;

private:
    // helpers
    float GetUIPosX(RectTransform* rt) const;
    void  SetUIPosX(RectTransform* rt, float x);
    float GetUIWidth(RectTransform* rt) const;

    bool  IsSpaceHold() const;

    float RandRange(float a, float b) const;
    float Clamp(float v, float mn, float mx) const;

    void  PickNextFoodTarget();
    void  UpdateFood(float delta);
    void  UpdateBar(float delta);
    void  UpdateGauge(float delta);

    bool  IsFoodInsideBar() const;

    Image* animImage{};
    void SetAnimPathes();
public:
    Game_FireControl() = default;
    ~Game_FireControl() override = default;

    void StartGame() override;
    void UpdateGame(float delta)override;
    void EndGame() override;

    void UpdateAnimation(float delta) override;
};



