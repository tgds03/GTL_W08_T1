
local cube = nil
local speed = 3
local direction = -1
local aaa = 0

function BeginPlay()
    PrintObject(setmetatable)
    PrintLog("BeginPlay")
    PrintObject(obj:GetActorLabel())
    PrintLog("BeginPlay--")
    -- cube = SpawnActor("ACube", "CubeActor")
end

function Tick(dt)
    local pos = obj:GetActorLocation()
    pos.x = pos.x + direction * speed * dt
    if (pos.x > 2) then
        direction = -1
    elseif (pos.x < -2) then
        direction = 1
    end
    obj:SetActorLocation(pos)
    -- PrintObject(obj:GetActorLocation().x)
    PrintObject(obj:GetActorLabel())
end
