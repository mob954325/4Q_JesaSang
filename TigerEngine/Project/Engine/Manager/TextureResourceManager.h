#pragma once
#include "../../Base/pch.h"
#include "System/Singleton.h"
#include "UIData/TextureResource.h"

class TextureResourceManager : public Singleton<TextureResourceManager>
{
public:
    TextureResourceManager(token) {};
    ~TextureResourceManager() = default;

    void Init(const ComPtr<ID3D11Device>& d, const ComPtr<ID3D11DeviceContext>& c);
    std::shared_ptr<TextureResource> LoadTextureResourceByPath(std::string path);

private:
    ComPtr<ID3D11Device> device{};
    ComPtr<ID3D11DeviceContext> context{};

    std::map<std::string, std::weak_ptr<TextureResource>> resources;
};
