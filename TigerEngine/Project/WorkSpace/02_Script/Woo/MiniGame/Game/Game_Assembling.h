#pragma once
#include "IMinigame.h"
#include <directxtk/Keyboard.h>
#include <string>
#include <array>

using std::string;
using namespace DirectX;

class RectTransform;
class Image;

/*
    [ 화살표 키 입력 게임 ]

   매 턴마다 랜덤하게 키 패턴을 생성하고 Image를 바꿔 표시
   키 입력과 비교하여 순서대로 sucees/fail 이미지로 변경
   각 키마다 입력시간 제한이 있어 안누르면 자동으로 틀린걸로 체크됨

   키 종류 : 좌우위아래 화살표 키 (각 키마다 이미지 3개(보통/성공/실패)
   키 : 총 8개

    - 게임 성공 : 3번의 턴을 성공하면 성공
    - 게임 실패 : 한 턴에서 키를 2번 틀리면 실패
*/

class Game_Assembling : public IMiniGame
{
private:
    // UI
    Image* key1 = nullptr;
    Image* key2 = nullptr;
    Image* key3 = nullptr;
    Image* key4 = nullptr;
    Image* key5 = nullptr;
    Image* key6 = nullptr;
    Image* key7 = nullptr;
    Image* key8 = nullptr;

    // --- data -----------------------
    // ui image path
    string rightDefault = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow.png";
    string rightGood = "..\\Assets\\Resource\\UI\\MiniGame\\arrow_good.png";
    string rightFail = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow_fail.png";

    string leftDefault = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow01.png";
    string leftGood = "..\\Assets\\Resource\\UI\\MiniGame\\arrow_good01.png";
    string leftFail = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow_fail01.png";

    string upDefault = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow02.png";
    string upGood = "..\\Assets\\Resource\\UI\\MiniGame\\arrow_good02.png";
    string upFail = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow_fail02.png";

    string downDefault = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow03.png";
    string downGood = "..\\Assets\\Resource\\UI\\MiniGame\\arrow_good03.png";
    string downFail = "..\\Assets\\Resource\\UI\\MiniGame\\Arrow_fail03.png";


    enum class ArrowDir { Left = 0, Right = 1, Up = 2, Down = 3 };

    struct DirImages
    {
        string* def = nullptr;
        string* good = nullptr;
        string* fail = nullptr;
    };

    // 8칸 UI 배열로 다루기
    std::array<Image*, 8> uiKeys{};

    // 한 턴 패턴(8개)
    std::array<ArrowDir, 8> pattern{};

    // turn/state
    int curIndex = 0;             // 0~7 현재 입력 위치
    int curTurnSuccessCount = 0;  // 3번 성공하면 최종 성공
    int curTurnFailCount = 0;     // 한 턴에서 2번 틀리면 실패

    // time limit
    float keyTimeLimit = 1.5f;   // 각 키 입력 제한시간
    float keyTimer = 0.0f;

    // dir images table
    DirImages imgLeft;
    DirImages imgRight;
    DirImages imgUp;
    DirImages imgDown;



private:
    // funcs..
    void SetImage(Image* image, string& path);

    void BuildDirImageTable();
    const DirImages& GetDirImages(ArrowDir dir) const;

    void StartNewTurn();
    void GeneratePattern();
    void ApplyDefaultImages();

    // input
    bool TryGetArrowDown(ArrowDir& outDir) const;

    // step
    void ResolveHit(bool correct);
    void MarkCurrentSlotGood();
    void MarkCurrentSlotFail();
    void AdvanceIndex();

    void FinishTurnIfNeeded();

public:
    Game_Assembling() = default;
    ~Game_Assembling() override = default;

    void StartGame() override;
    void UpdateGame(float delta)override;
    void EndGame() override;
};

