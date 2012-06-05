
local qnew = quad.new

function quad.new(...)
    local q = qnew(...)
    local members = getmetatable(q)
    members.__index = members
    members.__newindex = members
    setmetatable(members, quad)
    return q
end

quad.__index = quad
setmetatable(quad, { __index = ro } )
