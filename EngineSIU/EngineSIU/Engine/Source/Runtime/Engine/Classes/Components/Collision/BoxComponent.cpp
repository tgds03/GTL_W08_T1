#include "Components/Collision/BoxComponent.h"
#include "UObject/Casts.h"

UBoxComponent::UBoxComponent()
{
    ShapeType = EShapeType::Box;
    BoxExtent = FVector(1.f, 1.f, 1.f);
}

UObject* UBoxComponent::Duplicate(UObject* InOuter)
{
    ThisClass* NewComponent = Cast<ThisClass>(Super::Duplicate(InOuter));


    return NewComponent;
}

// 두 개의 OBB(Oriented Bounding Box)가 겹치는지 확인합니다.
// Separating Axis Theorem (SAT) 알고리즘을 사용합니다.
// 두 볼록 물체 사이에 분리 축이 존재하면 충돌하지 않고,
// 모든 잠재적 분리 축을 검사해도 분리 축을 찾지 못하면 충돌하는 것으로 간주합니다.
bool UBoxComponent::AreBoxesOverlapping(UBoxComponent* Other)
{
    // 다른 컴포넌트가 유효한지 확인합니다.
    if (Other == nullptr)
    {
        return false;
    }

    // --- 데이터 준비 ---

    // 현재 박스의 월드 공간 중심 위치를 계산합니다.
    const FVector ThisCenter = GetWorldLocation();
    // 다른 박스의 월드 공간 중심 위치를 계산합니다.
    const FVector OtherCenter = Other->GetWorldLocation();

    // 현재 박스의 로컬 축(X, Y, Z)을 월드 공간 벡터로 가져옵니다.
    // 회전 행렬을 사용하여 로컬 축을 월드 공간 방향으로 변환합니다.
    const FMatrix ThisRotationMatrix = GetRotationMatrix();
    const FVector ThisAxisX = FMatrix::TransformVector(FVector(1.f, 0.f, 0.f), ThisRotationMatrix).GetSafeNormal();
    const FVector ThisAxisY = FMatrix::TransformVector(FVector(0.f, 1.f, 0.f), ThisRotationMatrix).GetSafeNormal();
    const FVector ThisAxisZ = FMatrix::TransformVector(FVector(0.f, 0.f, 1.f), ThisRotationMatrix).GetSafeNormal();
    const FVector ThisAxes[3] = { ThisAxisX, ThisAxisY, ThisAxisZ };

    // 다른 박스의 로컬 축(X, Y, Z)을 월드 공간 벡터로 가져옵니다.
    const FMatrix OtherRotationMatrix = Other->GetRotationMatrix();
    const FVector OtherAxisX = FMatrix::TransformVector(FVector(1.f, 0.f, 0.f), OtherRotationMatrix).GetSafeNormal();
    const FVector OtherAxisY = FMatrix::TransformVector(FVector(0.f, 1.f, 0.f), OtherRotationMatrix).GetSafeNormal();
    const FVector OtherAxisZ = FMatrix::TransformVector(FVector(0.f, 0.f, 1.f), OtherRotationMatrix).GetSafeNormal();
    const FVector OtherAxes[3] = { OtherAxisX, OtherAxisY, OtherAxisZ };

    // 현재 박스의 범위(반경) 벡터를 가져옵니다. 각 축 방향으로의 절반 길이입니다.
    // 월드 스케일을 적용하여 월드 스케일의 범위를 구합니다.
    const FVector ThisExtent = BoxExtent * GetWorldScale3D();
    // 다른 박스의 범위(반경) 벡터를 가져옵니다.
    const FVector OtherExtent = Other->BoxExtent * Other->GetWorldScale3D();

    // --- SAT 계산 준비 ---

    // 두 박스 중심 간의 월드 공간 변위 벡터를 계산합니다.
    const FVector TranslationVector = OtherCenter - ThisCenter;

    // 두 박스의 방향 축 간의 내적(dot product)을 미리 계산하여 회전 행렬을 만듭니다.
    // RotationMatrix[i][j] = DotProduct(ThisAxes[i], OtherAxes[j])
    // 이 행렬은 한 박스의 축을 다른 박스의 좌표계로 변환하는 데 사용됩니다.
    float RotationMatrix[3][3];
    // 또한, 투영 계산에 사용할 내적 값의 절대값을 저장하는 행렬도 만듭니다.
    float AbsRotationMatrix[3][3];
    // 작은 오차(epsilon)를 정의하여 거의 0인 내적 값을 처리합니다.
    const float Epsilon = KINDA_SMALL_NUMBER; // Unreal Engine의 작은 값 상수

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            // 두 박스의 i번째 축과 j번째 축의 내적을 계산합니다.
            RotationMatrix[i][j] = FVector::DotProduct(ThisAxes[i], OtherAxes[j]);
            // 계산된 내적 값의 절대값을 저장합니다. 투영 길이를 계산할 때 사용됩니다.
            AbsRotationMatrix[i][j] = FMath::Abs(RotationMatrix[i][j]) + Epsilon; // Epsilon을 더해 부동 소수점 오류 방지
        }
    }

    // --- 분리 축 테스트 ---
    // SAT는 총 15개의 잠재적 분리 축을 테스트합니다.
    // 1. 현재 박스의 3개 로컬 축 (면 법선)
    // 2. 다른 박스의 3개 로컬 축 (면 법선)
    // 3. 두 박스의 각 축 쌍의 외적(cross product) 결과인 9개 축

    float ProjectedRadiusA, ProjectedRadiusB; // 각 박스의 투영 반경(절반 길이)
    float ProjectedDistance; // 두 박스 중심 간의 투영 거리

    // 테스트 1-3: 현재 박스의 로컬 축 (X, Y, Z)을 분리 축으로 테스트
    for (int i = 0; i < 3; ++i)
    {
        // 현재 박스의 투영 반경은 해당 축의 Extent 값입니다.
        ProjectedRadiusA = ThisExtent[i];

        // 다른 박스의 투영 반경을 계산합니다.
        // 다른 박스의 각 축에 대한 Extent와 해당 축이 현재 테스트 축(ThisAxes[i])에 투영된 길이를 곱한 값들의 합입니다.
        // AbsRotationMatrix를 사용하여 계산합니다.
        ProjectedRadiusB = OtherExtent[0] * AbsRotationMatrix[i][0] +
            OtherExtent[1] * AbsRotationMatrix[i][1] +
            OtherExtent[2] * AbsRotationMatrix[i][2];

        // 두 박스 중심 간의 변위 벡터(TranslationVector)를 현재 테스트 축(ThisAxes[i])에 투영합니다.
        // 이 투영 값의 절대값이 두 박스 중심의 투영 거리입니다.
        ProjectedDistance = FMath::Abs(FVector::DotProduct(TranslationVector, ThisAxes[i]));

        // 분리 조건 확인: 두 박스 중심의 투영 거리가 두 박스의 투영 반경 합보다 크면,
        // 이 축은 분리 축이며, 박스들은 겹치지 않습니다.
        if (ProjectedDistance > ProjectedRadiusA + ProjectedRadiusB)
        {
            // 분리 축을 찾았으므로 false를 반환합니다.
            return false;
        }
    }

    // 테스트 4-6: 다른 박스의 로컬 축 (X, Y, Z)을 분리 축으로 테스트
    for (int i = 0; i < 3; ++i)
    {
        // 현재 박스의 투영 반경을 계산합니다. (위와 유사하지만, 다른 박스의 축 기준)
        ProjectedRadiusA = ThisExtent[0] * AbsRotationMatrix[0][i] +
            ThisExtent[1] * AbsRotationMatrix[1][i] +
            ThisExtent[2] * AbsRotationMatrix[2][i];

        // 다른 박스의 투영 반경은 해당 축의 Extent 값입니다.
        ProjectedRadiusB = OtherExtent[i];

        // 두 박스 중심 간의 변위 벡터를 다른 박스의 테스트 축(OtherAxes[i])에 투영합니다.
        ProjectedDistance = FMath::Abs(FVector::DotProduct(TranslationVector, OtherAxes[i]));

        // 분리 조건 확인
        if (ProjectedDistance > ProjectedRadiusA + ProjectedRadiusB)
        {
            return false; // 겹치지 않음
        }
    }

    // 테스트 7-15: 두 박스의 축 쌍의 외적(cross product)을 분리 축으로 테스트
    for (int i = 0; i < 3; ++i) // 현재 박스 축 인덱스
    {
        for (int j = 0; j < 3; ++j) // 다른 박스 축 인덱스
        {
            // 외적 축 계산 (ThisAxes[i] x OtherAxes[j])
            // 이 외적 벡터는 두 축 모두에 수직이며, 잠재적인 분리 방향이 될 수 있습니다.

            // 현재 박스의 투영 반경 계산:
            // 외적 축에 대한 투영 반경은 현재 박스의 다른 두 축의 Extent와 관련된 RotationMatrix 값들을 사용하여 계산됩니다.
            // 예를 들어 i=0 (X축)일 때, Y축과 Z축의 Extent 및 관련 회전 정보를 사용합니다.
            ProjectedRadiusA = ThisExtent[(i + 1) % 3] * AbsRotationMatrix[(i + 2) % 3][j] +
                ThisExtent[(i + 2) % 3] * AbsRotationMatrix[(i + 1) % 3][j];

            // 다른 박스의 투영 반경 계산: (위와 유사하지만, 다른 박스 기준)
            ProjectedRadiusB = OtherExtent[(j + 1) % 3] * AbsRotationMatrix[i][(j + 2) % 3] +
                OtherExtent[(j + 2) % 3] * AbsRotationMatrix[i][(j + 1) % 3];

            // 두 박스 중심 간의 변위 벡터를 외적 축에 투영합니다.
            // 외적 축 자체를 직접 계산하는 대신, 내적 연산을 통해 투영 거리를 계산합니다.
            // ProjectedDistance = abs( Dot(TranslationVector, Cross(ThisAxes[i], OtherAxes[j])) )
            // 이는 Box-Product(스칼라 삼중곱)의 절대값과 관련 있으며, RotationMatrix를 사용하여 효율적으로 계산 가능합니다.
            ProjectedDistance = FMath::Abs(TranslationVector[(i + 2) % 3] * RotationMatrix[(i + 1) % 3][j] -
                TranslationVector[(i + 1) % 3] * RotationMatrix[(i + 2) % 3][j]);

            // 분리 조건 확인
            // 참고: 외적 축의 길이는 1이 아닐 수 있지만, SAT는 투영 간격의 분리 여부만 확인하므로
            // ProjectedDistance와 (ProjectedRadiusA + ProjectedRadiusB)를 같은 외적 축 길이로 스케일링한 결과를 비교하는 것과 동일합니다.
            // 따라서 외적 축을 정규화할 필요는 없습니다.
            // 단, 외적 결과가 거의 0 벡터인 경우 (두 축이 거의 평행한 경우) 테스트를 건너뛰어야 합니다.
            // 이는 AbsRotationMatrix에 Epsilon을 더함으로써 간접적으로 처리됩니다.
            // (AbsRotationMatrix 값이 크면 ProjectedRadiusA/B가 커져서 분리 조건을 만족하기 어렵게 됨)
            if (ProjectedDistance > ProjectedRadiusA + ProjectedRadiusB)
            {
                return false; // 겹치지 않음
            }
        }
    }

    // 모든 15개 잠재적 분리 축을 테스트했지만, 분리 축을 찾지 못했습니다.
    // 따라서 두 박스는 겹치는 것으로 간주합니다.
    return true;
}

FVector UBoxComponent::GetBoxExtent() const
{
    return BoxExtent;
}
