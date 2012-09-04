
ENTITIES = rawget(_G, "ENTITIES") or {}

local enew = entity.new

function entity.new(...)
    local e = enew(...)
    local members = getmetatable(e)
    members.__index = members
    members.__newindex = members
    setmetatable(members, entity)
    ENTITIES[e] = e
    return e
end

entity.__index = entity
entity.__classname = "entity"
setmetatable(entity, { __index = quad } )

function entity:update()
    print("WARNING: entity.update() base call")
end

function entity:onEndOfLife()
    print("Entity died")
    ENTITIES[self] = nil
end

function entity.foreach(f)
    -- FIXME: make sure the table isn't modified during traversal, ie. when a callback creates a new entity.
    for e, _ in pairs(ENTITIES) do
        f(e)
    end
end

