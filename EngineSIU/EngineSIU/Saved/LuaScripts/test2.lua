
local cube = nil
local speed = 3
local direction = -1

function BeginPlay()
    -- PrintObject(obj:GetLocation().x)
    PrintObject(USERTYPES)
    PrintObject(obj)
    -- SpawnActor("ACube", "TestCube")
    PrintObject(cube)
end

function Tick(dt)
    local pos = obj:GetActorLocation()
    pos.x = pos.x + direction * speed * dt
    if (pos.x > 1) then
        direction = -1
    elseif (pos.x < -1) then
        direction = 1
    end
    obj:SetActorLocation(pos)
end