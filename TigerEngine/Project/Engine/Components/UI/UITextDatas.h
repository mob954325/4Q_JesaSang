#pragma once


struct GlyphDraw
{
    float x, y;   // 글리프 top-left (캔버스 좌표, y-down)
    float w, h;   // 글리프 bitmap size
    float u0, v0, u1, v1;
    int advance;
};

// 정렬 타입
enum class HAlign
{
    Left, Center, Right
};