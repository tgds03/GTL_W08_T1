local Velocity = FVector(0.0, 1.0, 0.0)

function BeginPlay()
    --print("[BeginPlay] " .. obj.UUID)
    --obj:PrintLocation()
end

function EndPlay()
    --print("[EndPlay] " .. obj.UUID)
    --obj:PrintLocation()
end

function OnOverlap(overlappedComp, otherComp)
    -- owner 액터 가져오기
    local ownerA = overlappedComp:GetOwner()
    local ownerB = otherComp:GetOwner()

    -- 액터 이름 추출 (GetName()은 AActor에 바인딩된 UFUNCTION)
    local nameA = ownerA and ownerA:GetName() or "UnknownA"
    local nameB = ownerB and ownerB:GetName() or "UnknownB"

    -- 로그 출력
    PrintLog("Overlap 발생: "..nameA.." <> "..nameB)
end

function Tick(dt)
    --obj.Location = obj.Location + obj.Velocity * dt
    --obj:PrintLocation()
end
