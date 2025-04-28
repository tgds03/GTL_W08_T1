
local cube = nil
local speed = 3
local direction = -1
local time = 0

function BeginPlay()
    PrintObject(setmetatable)
end

function Tick(dt)
    time = time + dt

    local pos = obj:GetActorLocation()
    pos.x = pos.x + direction * speed * dt
    if (pos.x > 2) then
        direction = -1
    elseif (pos.x < -2) then
        direction = 1
    end
    obj:SetActorLocation(pos)

end

function EndPlay()
    cube = SpawnActor("ACube", "CubeActor")
end