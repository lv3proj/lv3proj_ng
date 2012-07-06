
local enew = entity.new

function entity.new(...)
    local e = enew(...)
    local members = getmetatable(e)
    members.__index = members
    members.__newindex = members
    setmetatable(members, entity)
    return e
end

entity.__index = entity
entity.__classname = "entity"
setmetatable(entity, { __index = quad } )

function entity:update()
    print("WARNING: entity.update() base call")
end

