
local snew = sound.new

function sound.new(...)
    local s = snew(...)
    local members = getmetatable(s)
    members.__index = members
    members.__newindex = members
    setmetatable(members, sound)
    return s
end

sound.__index = sound

