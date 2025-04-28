local speed = 50
local isMoveForward = false
local isMoveRight = false
local isMoveLeft = false
local isMoveBack = false

function BeginPlay()
    PrintLog("Input Test")
    PrintObject(EKeys)
end
function Tick(dt)
    local moveX = 0
    local moveY = 0

    if isMoveForward then
        moveY = moveY - 1
    end
    if isMoveBack then
        moveY = moveY + 1
    end
    if isMoveLeft then
        moveX = moveX - 1
    end
    if isMoveRight then
        moveX = moveX +1
    end

    local len = math.sqrt(moveX * moveX + moveY * moveY)
    if len > 0 then
        moveX = moveX / len
        moveY = moveY / len
    end

    -- 이동
    local pos = obj:GetActorLocation()
    pos.x = pos.x + moveX * speed * dt
    pos.y = pos.y + moveY * speed * dt
    obj:SetActorLocation(pos)
end

function OnKeyDown(keycode)
    PrintLog("KeyDown")
    if keycode == EKeys.W then 
        isMoveForward = true
        PrintLog("W Key Down")
    elseif keycode == EKeys.S then
        isMoveBack = true
        PrintLog("S Key Down")
    elseif keycode == EKeys.A then
        isMoveLeft = true
        PrintLog("A Key Down")
    elseif keycode == EKeys.D then
        isMoveRight = true
        PrintLog("D Key Down")
    end
end

function OnKeyUp(keycode)
    if keycode == EKeys.W then
        isMoveForward = false
        PrintLog("W Key Up")
    elseif keycode == EKeys.S then
        isMoveBack = false
        PrintLog("S Key Up")
    elseif keycode == EKeys.A then
        isMoveLeft = false
        PrintLog("A Key Up")
    elseif keycode == EKeys.D then
        isMoveRight = false
        PrintLog("D Key Up")
    end
end
function OnOverlap(overlappedComp, otherComp)
    _G["State"]  = false
end
-- function OnMouseDown(button)
--     if button == EKeys.LeftMouseButton then
--         PrintLog("Mouse1 Click Down")
--     end
-- end

-- function OnMouseMove(x, y)
--     PrintLog("Mouve Mouse:", x, y)
-- end