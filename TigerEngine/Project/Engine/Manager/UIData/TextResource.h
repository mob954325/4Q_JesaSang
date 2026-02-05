#pragma once
#include "../../Util/FontAtlasBuild.h"

struct TextResource
{
    FontAtlas atlas{};              // 아틀라스 정보
    float fontPx = 0;			    // 폰트 크기
    int atlasW = 0, atlasH = 0;     // 아틀라스 크기 
    int paddingPx = 1;              // 패딩 크기
};