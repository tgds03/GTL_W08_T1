local speed = 10
local isMoveForward = false
local isMoveRight = false
local isMoveLeft = false
local isMoveBack = false

local cam = nil
function BeginPlay()
    cam = as_ACamera(OBJ_CAMERA_221)
end

function EndPlay()
    --print("[EndPlay] " .. obj.UUID)
    --obj:PrintLocation()
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
        moveX = moveX + 1
    end

    local len = math.sqrt(moveX * moveX + moveY * moveY)
    if len > 0 then
        moveX = moveX / len
        moveY = moveY / len
    end

    local pos = obj:GetActorLocation()
    pos.x = pos.x + moveX * speed * dt
    pos.y = pos.y + moveY * speed * dt
    if (pos.x > 25) then pos.x = 25
    elseif (pos.x < -25) then pos.x = -25
    end
    if (pos.y > 25) then pos.y = 25
    elseif (pos.y < -25) then pos.y = -25
    end
    obj:SetActorLocation(pos)


    if (obj:GetActorLocation().x > 0) then
        cammgr = as_APlayerCameraManager(OBJ_PLAYERCAMMGR_223)
        cam:SetTargetCamera(cammgr, 1.5)
    else
        cammgr = as_APlayerCameraManager(OBJ_PLAYERCAMMGR_241)
        cam:SetTargetCamera(cammgr, 1.5)
    end
end

function OnKeyDown(keycode)
    if keycode == EKeys.W then 
        isMoveForward = true
    elseif keycode == EKeys.S then
        isMoveBack = true
    elseif keycode == EKeys.A then
        isMoveLeft = true
    elseif keycode == EKeys.D then
        isMoveRight = true
    end
end

function OnKeyUp(keycode)
    if keycode == EKeys.W then
        isMoveForward = false
    elseif keycode == EKeys.S then
        isMoveBack = false
    elseif keycode == EKeys.A then
        isMoveLeft = false
    elseif keycode == EKeys.D then
        isMoveRight = false
    end
end