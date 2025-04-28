local time = 0
local interval = 0
local fireballs = {}
local player = OBJ_CUBE_127

function BeginPlay()
    --print("[BeginPlay] " .. obj.UUID)
    --obj:PrintLocation()
end

function EndPlay()
    --print("[EndPlay] " .. obj.UUID)
    --obj:PrintLocation()
end

function OnOverlap(OtherActor)
    -- OtherActor:PrintLocation();
end

function Tick(dt)
    time = time + dt
    interval = interval + dt
    if (interval > 0.5) then 
        interval = 0
        fireballs[#fireballs+1] = SpawnFireball(#fireballs)
    end

    if (#fireballs > 0) then
        for i = 1, #fireballs do
            local fireball = fireballs[i]
            if (not fireball.destroyed) then
                fireball.position = fireball.position + fireball.velocity
                fireball.ball:SetActorLocation(fireball.position)
                fireball.light:SetActorLocation(fireball.position)
                if (fireball.position.x > 25 or fireball.position.x < -25) then
                    fireball:Destroy()
                elseif (fireball.position.y > 25) then
                    fireball:Destroy()
                end
            end
        end
    end
end


function SpawnFireball(index)
    local fireball = {}
    fireball.ball = SpawnActor("ACube", "FireballSphere_" .. index)
    fireball.light = SpawnActor("APointLight", "FireballLight_" .. index)
    fireball.position = FVector(math.random(-25, 25), -30, player:GetActorLocation().z)
    fireball.velocity = player:GetActorLocation() - fireball.position
    fireball.velocity = fireball.velocity:Normal(0.01) * 0.1
    fireball.destroyed = false
    fireball.Destroy = function (o) 
        o.ball:Destroy()
        o.light:Destroy()
        o.ball = nil
        o.light = nil
        o.destroyed = true
    end
    return fireball
end