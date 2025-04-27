function BeginPlay()
    PrintLog("Input Test")
end
function Tick(dt)
    if Input:IsKeyDown(EKeys.W) then
        PrintLog("W Key Down")
    end
end

function OnKeyDown(keycode)
    PrintLog("Key Down")
    if keycode == EKeys.SpaceBar then
        PrintLog("SpaceBar Key Down")
    end
end

function OnKeyUp(keycode)
    if keycode == EKeys.W then
        PrintLog("W Key Up")
    end
end

function OnMouseDown(button)
    if button == EKeys.LeftMouseButton then
        PrintLog("Mouse1 Click Down")
    end
end

function OnMouseMove(x, y)
    PrintLog("Mouve Mouse:", x, y)
end