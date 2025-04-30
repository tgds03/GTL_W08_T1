#pragma once
#include <Math/Vector.h>
struct FViewTarget {

    // FIXME : FViewTarget or SpringArm 둘 중
    // 따라갈 액터 
   // AActor* TargetActor = nullptr;
    // 카메라 위치
    FVector EyeLocation = FVector::Zero();
    // 카메라 회전(쿼터니언)
    FVector EyeRotation = FVector::Zero();
};
