#include "TextureResourceManager.h"
#include "directxtk/WICTextureLoader.h"

void TextureResourceManager::Init(const ComPtr<ID3D11Device>& d, const ComPtr<ID3D11DeviceContext>& c)
{
    device = d;
    context = c;
}

std::shared_ptr<TextureResource> TextureResourceManager::LoadTextureResourceByPath(std::string path)
{
    // 1. 이미 로드한 텍스처인지 확인
    auto it = resources.find(path);

    if (it != resources.end()) // 존재함
    {
        if (!it->second.expired())
        {
            std::shared_ptr<TextureResource> resource = it->second.lock();
            return resource;
        }
        else
        {
            resources.erase(it); // 지우고 새로 만들기
        }
    }

    // 2. 새로 가져오기
    auto newResource = std::make_shared<TextureResource>();

    std::wstring wpath = std::wstring(path.begin(), path.end());
    HR_T(CreateWICTextureFromFile(device.Get(), context.Get(), wpath.c_str(), nullptr, newResource->srv.GetAddressOf()));

    ComPtr<ID3D11Resource> res;
    newResource->srv->GetResource(res.GetAddressOf());

    HR_T(res.As(&newResource->tex));

    D3D11_TEXTURE2D_DESC desc{};
    newResource->tex->GetDesc(&desc);

    newResource->texSizePx = Vector2((float)desc.Width, (float)desc.Height); // border(px)

    std::weak_ptr<TextureResource> weakResource = newResource;
    resources.insert({ path, weakResource });

    return newResource;
}
