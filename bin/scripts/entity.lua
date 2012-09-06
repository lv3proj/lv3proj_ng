
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
    --print("WARNING: entity.update() base call")
end

function entity:updateFixed()
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

function entity:unstuck()
    local c, cx, cy
    while true do
        c, cx, cy = self:collideGrid()
        if c then
            local nx, ny = getWallNormal(cx, cy)
            if nx == 0 and ny == 0 then
                print("Unstuck error")
                return
            end
            local px, py = self:getPosition()
            print(px+nx, py+ny)
            self:position(px+nx, py+ny)
            return
        else
            return
        end
    end
end

function entity:checkWall(dx, dy)
    local px, py = self:getPosition()
    self:position(px+dx, py+dy)
    local c, x, y = self:collideGrid()
    self:position(px, py)
    return c, x, y
end
