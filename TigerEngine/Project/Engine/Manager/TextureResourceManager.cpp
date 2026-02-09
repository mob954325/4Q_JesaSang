#include "TextureResourceManager.h"
#include "directxtk/WICTextureLoader.h"
#include "DirectXTex.h"

static std::wstring GetLowerExt(const std::wstring& wpath)
{
    std::filesystem::path p(wpath);
    std::wstring ext = p.extension().wstring();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    return ext; // 예: L".tga"
}

void TextureResourceManager::Init(const ComPtr<ID3D11Device>& d, const ComPtr<ID3D11DeviceContext>& c)
{
    device = d;
    context = c;
}

std::shared_ptr<TextureResource> TextureResourceManager::LoadTextureResourceByPath(std::string path)
{
    // 1) 캐시 확인
    auto it = resources.find(path);
    if (it != resources.end())
    {
        if (!it->second.expired())
            return it->second.lock();

        resources.erase(it);
    }

    // 2) 새로 로드
    auto newResource = std::make_shared<TextureResource>();

    std::wstring wpath(path.begin(), path.end());
    const std::wstring ext = GetLowerExt(wpath);

    if (ext == L".tga")
    {
        // (A) TGA 로드
        DirectX::ScratchImage img;
        HR_T(DirectX::LoadFromTGAFile(wpath.c_str(), nullptr, img));

        // (B) 밉맵 생성(선택: 없으면 그대로 사용)
        const DirectX::Image* base = img.GetImage(0, 0, 0);
        if (!base) HR_T(E_FAIL);

        DirectX::ScratchImage mipChain;
        HRESULT hrMip = DirectX::GenerateMipMaps(
            base,
            1,
            img.GetMetadata(),
            DirectX::TEX_FILTER_DEFAULT,
            0,
            mipChain);

        const DirectX::ScratchImage& finalImg = SUCCEEDED(hrMip) ? mipChain : img;

        // (C) GPU 텍스처 생성
        ComPtr<ID3D11Resource> texRes;
        HR_T(DirectX::CreateTexture(
            device.Get(),
            finalImg.GetImages(),
            finalImg.GetImageCount(),
            finalImg.GetMetadata(),
            texRes.GetAddressOf()));

        // (D) SRV 생성
        HR_T(device->CreateShaderResourceView(
            texRes.Get(),
            nullptr,
            newResource->srv.GetAddressOf()));
    }
    else
    {
        HR_T(CreateWICTextureFromFile(
            device.Get(),
            context.Get(),
            wpath.c_str(),
            nullptr,
            newResource->srv.GetAddressOf()));
    }

    // 3) srv -> tex / size 채우기
    ComPtr<ID3D11Resource> res;
    newResource->srv->GetResource(res.GetAddressOf());

    HR_T(res.As(&newResource->tex));

    D3D11_TEXTURE2D_DESC desc{};
    newResource->tex->GetDesc(&desc);
    newResource->texSizePx = Vector2((float)desc.Width, (float)desc.Height);

    // 4) 캐시에 등록
    resources[path] = newResource;
    return newResource;
}