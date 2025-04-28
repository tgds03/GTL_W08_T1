local fb = nil

function Update(dt)
    if testActor == nil then
        testActor = SpawnActor("AFireballActor")
        testActor:SetLocation(FVector(0, 10, 0))
    end
end

-- function OnPlayStart()
--     local fb = SpawnActor("AFireballActor")
--     if fb then
--         fb:SetLocation( FVector(0,0,0) )
--     end
-- end