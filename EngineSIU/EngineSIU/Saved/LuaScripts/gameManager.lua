local Velocity = FVector(0.0, 1.0, 0.0)

function BeginPlay()
    _G["Timer"] = 0.0
    _G["State"] = true
    PrintLog("gameManger.lua")
end

function EndPlay()
end

function Tick(dt)
    if _G["State"] == true then
        _G["Timer"] = _G["Timer"] + dt
        local timerInt = math.floor(_G["Timer"])
        PrintLog(timerInt.."")
        obj:SetScore(timerInt)
    end
    if _G["State"] == false then 
        obj:SetIsPlayingOff()
        PrintLog("Player Dead")
    end

end
