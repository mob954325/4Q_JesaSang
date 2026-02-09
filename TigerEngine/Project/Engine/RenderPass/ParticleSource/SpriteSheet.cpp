#include "SpriteSheet.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../../Manager/TextureResourceManager.h"

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<SpriteSheet>("SpriteSheet")
        .constructor<>()(policy::ctor::as_object)

        .property("texturePath", &SpriteSheet::GetPath, &SpriteSheet::SetPath)
            (metadata(META_BROWSE, ""))
        .property("cols", &SpriteSheet::cols)
        .property("rows", &SpriteSheet::rows)
        .property("baseSizeScale", &SpriteSheet::baseSizeScale)

        .property("frameCount", &SpriteSheet::frameCount)
        .property("fps", &SpriteSheet::fps)
        .property("loop", &SpriteSheet::loop)
            (metadata(META_BOOL, true))

        // 원하면 계산값을 read-only로 노출 가능 (setter 없음)
        .property_readonly("flipbookDuration", &SpriteSheet::GetFilpbookDuration);
}

void SpriteSheet::SetPath(std::string str)
{
    texturePath = str;
    resource = TextureResourceManager::Instance().LoadTextureResourceByPath(texturePath);
}

std::string SpriteSheet::GetPath()
{
    return texturePath;
}
