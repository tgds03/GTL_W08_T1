#pragma once

#include "GameFramework/Actor.h"

class ALuaTimer : public AActor
{
    DECLARE_CLASS(ALuaTimer, AActor)

public:
    ALuaTimer();
    ~ALuaTimer();

    UFUNCTION(void, Start)
    UFUNCTION(void, Stop)
    UFUNCTION(void, Tick, float)
    UFUNCTION(void, Reset)
    UFUNCTION(void, Pause)
    UFUNCTION(void, Resume)
    
    UFUNCTION(bool, IsRunning)
    UFUNCTION(bool, IsPaused)
    UFUNCTION(float, GetElapsedTime)

private:
    bool bIsRunning;
    bool bIsPaused;
    float ElapsedTime;
};
