
local cube = nil
local a = -1

function BeginPlay()
    PrintObject(obj)
end

function Tick(dt)
    if cube == nil then
        cube = SpawnActor("Cube", "TestCube")
        -- cube:SetLocation(FVector(0, 0, 0))
    end
    a = a + 1
    -- PrintLog(string.format("%d", obj))
    -- PrintLog(obj.RootComponent)
end