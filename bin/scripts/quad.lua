
local qnew = quad.new

QUADS = rawget(_G, "QUADS") or {}

rawset(_G, "clearQuads", function()
    local c = 0
    for _, q in pairs(QUADS) do
        if not (q:isInternal() or q:getParent()) then
            q:delete()
            c = c + 1
        end
    end
    print(string.format("clearQuads: %d quads removed", c))
end)

function quad.new(...)
    local q = qnew(...)
    local members = getmetatable(q)
    members.__index = members
    members.__newindex = members
    setmetatable(members, quad)
    QUADS[q] = q
    return q
end

function quad:onEndOfLife()
    QUADS[self] = nil
end

quad.__index = quad
quad.__classname = "quad"
setmetatable(quad, { __index = ro } )
