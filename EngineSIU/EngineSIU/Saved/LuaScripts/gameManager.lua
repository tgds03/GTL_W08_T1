local Velocity = FVector(0.0, 1.0, 0.0)

function BeginPlay()
    _G["Timer"] = 0.0
    _G["State"] = true
end

function EndPlay()
end

function Tick(dt)
    _G["Timer"] = _G["Timer"] + dt
    --PrintLog(_G["Timer"] .. "")

    if _G["State"] == false then 
        obj:SetIsPlayingOff()
        PrintLog("Player Dead")
    end

end
