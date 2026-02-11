#pragma once
#include <random>
#include <utility>
#include <stdexcept>
#include <algorithm>

struct EdgeToCenterSampler {
    int W, H;
    int step;      // inset 증가 폭 (px)
    int jitter;    // inset 흔들림 (0이면 없음)
    int inset;     // 현재 inset
    int maxInset;  // min(W,H)/2

    std::mt19937 rng;

    EdgeToCenterSampler(int w, int h, int stepPx = 10, int jitterPx = 0, uint32_t seed = std::random_device{}())
        : W(w), H(h), step(stepPx), jitter(jitterPx), inset(0), maxInset(std::min(w, h) / 2), rng(seed)
    {
        if (W <= 0 || H <= 0) throw std::invalid_argument("W/H must be > 0");
        if (step <= 0) throw std::invalid_argument("step must be > 0");
        if (jitter < 0) throw std::invalid_argument("jitter must be >= 0");
    }

    // [a, b] 정수 균등 분포
    int randint(int a, int b) {
        std::uniform_int_distribution<int> dist(a, b);
        return dist(rng);
    }

    // inset 사각형의 '테두리' 위 랜덤 점 반환
    std::pair<int, int> randomPointOnInsetBorder(int useInset) {
        int left = useInset;
        int top = useInset;
        int right = W - 1 - useInset;
        int bottom = H - 1 - useInset;

        if (left > right || top > bottom) throw std::runtime_error("Inset too large.");

        // 붕괴 케이스(점/선)
        if (left == right && top == bottom) {
            return { left, top };
        }
        if (top == bottom) { // 수평선
            return { randint(left, right), top };
        }
        if (left == right) { // 수직선
            return { left, randint(top, bottom) };
        }

        // 4변 중 하나 선택
        int side = randint(0, 3); // 0=top,1=bottom,2=left,3=right
        switch (side) {
        case 0: return { randint(left, right), top };
        case 1: return { randint(left, right), bottom };
        case 2: return { left, randint(top, bottom) };
        default:return { right, randint(top, bottom) };
        }
    }

    // 다음 좌표 1개 생성 (가장자리->중앙 진행, 중앙 도달 시 다시 가장자리로 리셋)
    std::pair<int, int> next() {
        int useInset = inset;

        if (jitter > 0) {
            useInset += randint(-jitter, jitter);
        }
        useInset = std::clamp(useInset, 0, maxInset);

        auto p = randomPointOnInsetBorder(useInset);

        inset += step;
        if (inset > maxInset) inset = 0; // 리셋(원하면 제거)

        return p;
    }
};
