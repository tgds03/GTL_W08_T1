#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Components/SceneComponent.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/JungleMath.h"

class USpringArmComponent : public USceneComponent
{
    DECLARE_CLASS(USpringArmComponent, USceneComponent)

public:
    USpringArmComponent();

    // 매 프레임 업데이트 함수
    virtual void TickComponent(float DeltaTime) override;


private:
    /** 월드 인스턴스를 가져오는 헬퍼 함수 (엔진 구조에 맞게 수정 필요) */
    UWorld* GetWorld() const;

};
