#pragma once
#include "../RenderComponent.h"
#include "../../Manager/UIData/TextureResource.h"
#include "../../Components/RectTransform.h"
#include "UITypeDatas.h"
#include "../../Util/Multidelegate.h"

enum class ImageType
{
    Simple,		// 기본 이미지 출력
    Sliced,		// 9-Sliced
    Fill		// 이미지 채우기
};

/// <summary>
/// UI 이미지 컴포넌트 
/// GameObject에 RectTransform을 추가한다. -> Transform, RectTransform 둘 다 가지고 있음.
/// </summary>
/// <remarks>
/// Transform과 RectTransform 둘 다 가지고 있는데 대게 스크린 기준 좌표계는 rect의 pos를 사용한다.
/// </remarks>
class Image : public RenderComponent
{
    RTTR_ENABLE(RenderComponent)
public:
    void OnInitialize() override;
    void OnUpdate(float delta) override;

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    void GetTextureByPath(std::string path);
    void OnRender(RenderQueue& queue) override;

    void SetMouseCheck(bool value);
    bool GetMouseCheck() const;

    ImageType GetType() const { return type; }
    void SetType(ImageType value) { type = value; }

    DrawSpaceType GetDrawSpace() const { return drawSpacetype; }
    void SetDrawSpace(DrawSpaceType value) { drawSpacetype = value; }

    const Color& GetColor() const { return color; }
    void SetColor(const Color& value) { color = value; }

    float GetFillAmount() const { return fillAmount; }
    void SetFillAmount(float value) { fillAmount = std::clamp(value, 0.0f, 1.0f); }

    const Vector4& GetBorderPx() const { return sliceBorderPx; }
    void SetBorderPx(const Vector4& value) { sliceBorderPx = value; }

    const std::string& GetPath() const { return path; }
    void SetPath(const std::string& path) { this->path = path; }

    int GetZOrder() const;
    void SetZOrder(int v);

    void ChangeData(std::string path);

    // Event
    MultiDelegate<> OnEnter;
    MultiDelegate<> OnClick;
    MultiDelegate<> OnExit;

private:
    void Init();
    
    std::shared_ptr<TextureResource> resource{};
    // RectTransform* rect;

    DrawSpaceType drawSpacetype = DrawSpaceType::Screen;
    ImageType type = ImageType::Simple;
    Color color{};

    // fillMode에서 사용할 값
    float fillAmount = 1.0f;

    // 9-slice 보더 px (L, R, T, B) 
    Vector4 sliceBorderPx{ 0.0f, 0.0f, 0.0f, 0.0f }; 

    std::string path{};
    bool isMouseCheck = false;  // 마우스 체크 확인
    bool isMouseHover = false;  // 마우스가 겹치는지 확인
    bool hoverd = false;        // 마우스가 이미지 위에 올라가있는지 확인
    bool prevLeft = false;      // 마우스 이전에 클릭했는지 저장 변수
    void CheckMouseHover();

    int zOrder = 0; // 해당 값을 item에 넘겨서 한 번 정렬한 다음에 출력한다.
};