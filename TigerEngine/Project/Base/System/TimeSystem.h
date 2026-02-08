/**
	@file      TimeSystem.h
	@brief     Thanks Frank Luna, this class is based on his code.
	@author    SKYFISH
	@date      JUNE.2023
	@notice    물방울 책의 코드이며 네이밍 일관성 때문에 파일명만 변경
**/

#pragma once
#include "../System/Singleton.h"

class GameTimer : public Singleton<GameTimer>
{
public:
	GameTimer(token);
	~GameTimer() = default;

    // scaled time :게임 시간
	float DeltaTime()const;  // in seconds

    // unscaled time : pause 상태와 관계 x
    float TotalTime()const;          // in seconds
    float UnscaledDeltaTime() const; // in seconds

    // time scale funcs
    void SetTimeScale(float scale);
    float GetTimeScale() const;


	void Reset(); // Call before message loop.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every frame.
	
private:
	double mSecondsPerCount{};
	double mDeltaTime{};          // scaled delta
    double mUnscaledDeltaTime{};  // raw delta

	__int64 mBaseTime{};
	__int64 mPausedTime{};
	__int64 mStopTime{};
	__int64 mPrevTime{};
	__int64 mCurrTime{};

	bool mStopped = false;
    float mTimeScale = 1.0f;        // time scale
};