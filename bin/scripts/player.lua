
newclass("player", entity)

function player.new(layer)
    layer = layer or 15 -- FIXME
    local p = entity.new(layer)
    setclass(p, player)
    return p
end

function player:update(dt)
    local x, y = self:getPosition()
    if isKey(KEY_LEFT) then
        self:position(x - 100*dt, y)
    end
    if isKey(KEY_RIGHT) then
        self:position(x + 100*dt, y)
    end
    
    if isKey(KEY_UP) then
        camera.zoom(camera.getScale() * 1.2)
    end
    if isKey(KEY_DOWN) then
        camera.zoom(camera.getScale() * 0.8)
    end
end

