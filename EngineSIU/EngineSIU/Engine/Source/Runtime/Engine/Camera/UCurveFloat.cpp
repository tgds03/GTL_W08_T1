#include "UCurveFloat.h"
#include "Core/Math/MathUtility.h"
void UCurveFloat::AddKey(float InTime, float InValue)
{
    Keys.Add(FKey{ InTime, InValue });
    std::sort(Keys.begin(), Keys.end(), [](const FKey& A, const FKey& B) {
        return A.Time < B.Time;
        });
}

float UCurveFloat::GetFloatValue(float InTime) const
{
    if (Keys.IsEmpty()) return 0.f;

    if (InTime <= Keys.front().Time)
        return Keys.front().Value;

    if (InTime >= Keys.back().Time)
        return Keys.back().Value;

    // 선형 보간
    for (size_t i = 0; i < Keys.Num() - 1; ++i)
    {
        const FKey& A = Keys[i];
        const FKey& B = Keys[i + 1];
        if (InTime >= A.Time && InTime <= B.Time)
        {
            float Alpha = (InTime - A.Time) / (B.Time - A.Time);
            return FMath::Lerp(A.Value, B.Value, Alpha);
        }
    }

    return 0.f; // fallback
}
