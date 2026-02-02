#pragma once
#include "../RenderComponent.h"
#include "../../Manager/UIData/TextureResource.h"
#include "../../Components/RectTransform.h"

enum class ImageType
{
    Simple,		// 기본 이미지 출력
    Sliced,		// 이미지 중간 부분만 늘려지는거 ( 9-Sliced는 아님 )
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

    nlohmann::json Serialize() override;
    void Deserialize(nlohmann::json data) override;

    void GetTextureByPath(std::string path);
    void OnRender(RenderQueue& queue) override;

    void SetMouseCheck(bool value);
    bool GetMouseCheck() const;

    ImageType GetType() const { return type; }
    void SetType(ImageType value) { type = value; }

    const Color& GetColor() const { return color; }
    void SetColor(const Color& value) { color = value; }

    float GetFillAmount() const { return fillAmount; }
    void SetFillAmount(float value) { fillAmount = std::clamp(value, 0.0f, 1.0f); }

    const Vector4& GetBorderPx() const { return sliceBorderPx; }
    void SetBorderPx(const Vector4& value) { sliceBorderPx = value; }

    const std::string& GetPath() const { return path; }
    void SetPath(const std::string& path) { this->path = path; }

    void ChangeData(std::string path);

private:
    void Init();

    std::shared_ptr<TextureResource> resource{};
    RectTransform* rect;

    ImageType type = ImageType::Simple;
    Color color{};

    // fillMode에서 사용할 값
    float fillAmount = 1.0f;

    // 9-slice 보더 px (L, R, T, B) 
    Vector4 sliceBorderPx{ 0.0f, 0.0f, 0.0f, 0.0f }; 

    std::string path{};
    bool isMouseCheck = false;
};
