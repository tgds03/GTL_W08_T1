local speed = 3

function BeginPlay()
    PrintLog("Input Test")
    PrintObject(EKeys)
end
function Tick(dt)
end

function OnKeyDown(keycode)
    local keyName = KeyCodeToString[keycode]
    if keycode == EKeys.W then
        local pos = obj:GetActorLocation()
        pos.y = pos.y + (-1) * speed
        obj:SetActorLocation(pos)
        PrintObject(obj:GetActorLabel())
        PrintLog("W Key Down")
    end
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