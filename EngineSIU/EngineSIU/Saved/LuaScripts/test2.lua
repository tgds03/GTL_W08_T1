
local cube = nil
local a = -1

function BeginPlay()
    -- PrintObject(obj:GetLocation().x)
    PrintObject(USERTYPES)
    PrintObject(obj)
    -- SpawnActor("ACube", "TestCube")
    PrintObject(cube)
end

function Tick(dt)
    obj:SetLocation(FVector(0, 0, 0))
end