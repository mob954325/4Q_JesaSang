#pragma once
#include "../../Base/pch.h"
#include "System/Singleton.h"
#include "UIData/TextureResource.h"

class TextureResourceMaager : public Singleton<TextureResourceMaager>
{
public:
    TextureResourceMaager(token) {};
    ~TextureResourceMaager() = default;

    std::shared_ptr<TextureResource> LoadTextureResourceByPath(std::string path);

private:
    ComPtr<ID3D11Device> device{};
    ComPtr<ID3D11DeviceContext> context{};

    std::map<std::string, std::weak_ptr<TextureResource>> resources;
};
