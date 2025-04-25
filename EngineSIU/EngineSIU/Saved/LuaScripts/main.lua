-- Scripts/main.lua

-- 전역 변수로 한 번만 생성할 큐브 핸들러
local cube = nil

-- 매 프레임 호출되는 함수
function Update(dt)
    -- 아직 스폰되지 않았다면 한 번만 스폰
    if cube == nil then
        cube = SpawnActor("Cube", "TestCube")
        -- 초기 위치 설정
        cube:SetLocation(FVector(0, 0, 0))
    end

    -- 매 프레임마다 X축으로 1 단위/초 속도로 이동
    local speed = 1.0
    -- 현재 위치를 가져와
    local loc = cube:GetLocation()
    -- dt(sec) 만큼 속도 곱해 더하고
    loc.x = loc.x + speed * dt
    -- 다시 설정
    cube:SetLocation(loc)
end