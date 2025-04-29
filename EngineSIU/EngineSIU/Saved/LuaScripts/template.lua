local Velocity = FVector(0.0, 1.0, 0.0)

function BeginPlay()
    --print("[BeginPlay] " .. obj.UUID)
    --obj:PrintLocation()
end

function EndPlay()
    --print("[EndPlay] " .. obj.UUID)
    --obj:PrintLocation()
end

function OnOverlap(thisComp, otherComp)
    -- otherComp:GetUUID()
end

function Tick(dt)
    --obj.Location = obj.Location + obj.Velocity * dt
    --obj:PrintLocation()
end
