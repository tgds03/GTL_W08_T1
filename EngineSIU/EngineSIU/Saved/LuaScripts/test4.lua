local Velocity = FVector(0.0, 1.0, 0.0)
local time = 0
local lights = {}

function BeginPlay()
    PrintLog("[BeginPlay] " .. tostring(obj:GetUUID()))
    for i = 1, 20 do
        lights[i] = SpawnActor("ACube", "ACube_"..i)
        pos = FVector()
        pos.x = math.random(-10, 10)
        pos.y = math.random(-10, 10)
        lights[i]:SetActorLocation(pos)
    end
    _G["Test"] = SpawnActor("ACube", "asdf")
end

function EndPlay()
    PrintLog("[EndPlay] " .. tostring(obj:GetUUID()))
    PrintObject(obj:GetActorLocation())
end

function OnOverlap(OtherActor)
    OtherActor:PrintLocation();
end

function Tick(dt)
    local pos = obj:GetActorLocation()
    pos = pos + Velocity * dt
    obj:SetActorLocation(pos) 

end