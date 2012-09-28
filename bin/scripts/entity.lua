
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
    --print("Entity died")
    ENTITIES[self] = nil
end

function entity.foreach(f)
    -- FIXME: make sure the table isn't modified during traversal, ie. when a callback creates a new entity.
    local valid = isObjectValid
    for e, _ in pairs(ENTITIES) do
        if valid(e) then
            f(e)
        else
            print("Lua WARNING: Found invalid entity in foreach(), did you miss forwarding a call to entity.onEndOfLife() ?")
            ENTITIES[e] = nil
        end
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

function entity:moveOutOfEntity(e, maxsteps, vlen)
    maxsteps = maxsteps or 1
    vlen = vlen or 1
    
    local coll = false
        
    while maxsteps > 0 do
        local c, x, y = self:collideWith(e)
        if not c then
            break
        end
        coll = true
        
        local px, py = self:getPosition()
        
        local dx, dy = px - x, py - y
        dx, dy = vector.setLen(dx, dy, vlen)
        
        local sx, sy = self:getPosition()
        self:position(sx + dx, sy + dy)
        
        maxsteps = maxsteps - 1
    end
    return coll
end

function entity:moveApart(e, maxsteps, vlen)
    maxsteps = maxsteps or 1
    vlen = vlen or 1
    
    local coll = false
        
    while maxsteps > 0 do
        local c, x, y = self:collideWith(e)
        if not c then
            break
        end
        coll = true
        
        local sx, sy = self:getPosition()
        
        local dx, dy = px - x, py - y
        dx, dy = vector.setLen(dx, dy, vlen)
        
        self:position(sx + dx, sy + dy)
        if self:collideGrid() then
            self:position(sx, sy)
        end
        
        sx, sy = e:getPosition()
        e:position(sx - dx, sy - dy)
        if e:collideGrid() then
            e:position(sx, sy)
        end
        
        maxsteps = maxsteps - 1
    end
    
    return coll
end

function entity:moveOutOfWall(maxsteps, vlen, area, skip) -- all optional
    maxsteps = maxsteps or 1
    vlen = vlen or 1
    
    local coll = false
    while maxsteps > 0 do
        local c, x, y = self:collideGrid()
        if not c then
            break
        end
        coll = true
        local nx, ny = getWallNormal(x, y, area, skip)
        if nx == 0 and ny == 0 then return end
        nx, ny = vector.setLen(nx, ny, vlen)
        local px, py = self:getPosition()
        self:position(px + ny, py + ny)
        maxsteps = maxsteps - 1
    end
    return coll
end

