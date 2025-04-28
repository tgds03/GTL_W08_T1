#include "LuaTimer.h"

ALuaTimer::ALuaTimer()
    : bIsRunning(false),
    bIsPaused(false),
    ElapsedTime(0.0f)
{
}

ALuaTimer::~ALuaTimer()
{
}

void ALuaTimer::Start()
{
    bIsRunning = true;
    ElapsedTime = 0.f;
}

void ALuaTimer::Stop()
{
    bIsRunning = false;
}

void ALuaTimer::Tick(float DeltaTime)
{
    if (bIsRunning && !bIsPaused)
    {
        ElapsedTime += DeltaTime;
    }
}

void ALuaTimer::Reset()
{
    ElapsedTime = 0.f;
}

void ALuaTimer::Pause()
{
    if (bIsRunning)
    {
        bIsPaused = true;
    }
    bIsRunning = false;
}

void ALuaTimer::Resume()
{
    bIsPaused = false;
}

bool ALuaTimer::IsRunning()
{
    return bIsRunning;
}

bool ALuaTimer::IsPaused()
{
    return bIsPaused;
}

float ALuaTimer::GetElapsedTime()
{
    return ElapsedTime;
}
