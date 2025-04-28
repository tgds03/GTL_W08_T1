local Velocity = FVector(0.0, 1.0, 0.0)
local time = 0
local light

function BeginPlay()
    PrintLog("[BeginPlay] " .. tostring(obj:GetUUID()))
    PrintObject(obj:GetActorLocation())
    light = SpawnActor("APointLight", "MyLight")
end

function EndPlay()
    PrintLog("[EndPlay] " .. tostring(obj:GetUUID()))
    PrintObject(obj:GetActorLocation())
end

function OnOverlap(OtherActor)
    OtherActor:PrintLocation();
end

function Tick(dt)
    time = time + dt
    -- PrintLog(ACube167:GetOriginalActorLabel():ToString())
    -- PrintObject(GetActorByName("ACube167"))
    local pivot = ACube167:GetActorLocation()   -- 현재 씬의 액터 참조하기

    local radius = 10
    local pos = pivot
    pos.x = pivot.x + radius * math.cos(time)
    pos.y = pivot.y + radius * math.sin(time)
    -- obj:SetActorLocation(pos)
    _G["Test"]:SetActorLocation(pos)
end