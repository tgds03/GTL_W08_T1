local Velocity = FVector:new(0.0, 1.0, 0.0)

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
    PrintObject(Velocity)
    obj:SetActorLocation(obj:GetActorLocation() + Velocity * dt)
    PrintObject(obj:GetActorLocation())
end