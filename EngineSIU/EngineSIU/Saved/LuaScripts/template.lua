local Velocity = FVector(0.0, 1.0, 0.0)

function BeginPlay()
    PrintLog("[BeginPlay] " .. tostring(obj:GetUUID()))
    PrintObject(obj:GetActorLocation())
end

function EndPlay()
    PrintLog("[EndPlay] " .. tostring(obj:GetUUID()))
    PrintObject(obj:GetActorLocation())
end

function OnOverlap(OtherActor)
    OtherActor:PrintLocation();
end

function Tick(dt)
    obj:SetActorLocation(obj:GetActorLocation() + Velocity * dt)
end