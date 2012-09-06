
dofile("testmap.lua")

--[[for i = 1, 50 do
    quad.new("test.png", 15):scale(0.2, 0.2):position(math.random(-100, 900), math.random(-100, 700)):alpha(0.2)
end]]


local sh = entity.new()
--sh:setCircleCollider(150)
sh:texture("test3.png"):position(400, 300)
--sh:setAABBCollider()
sh:setLineCollider(-80, 150, 80, 150)
sh:rotate(-30):rotate(30, 5, -1, true, true)

function sh:update(dt)
    --local x, y = self:getPosition()
    --self:position(x + math.random(-2, 2), y + math.random(-2, 2))
end

local q = quad.new("debug/pixels.png", 28):scale(4, 4)
local q2 = quad.new("debug/pixels.png", 28):scale(4, 4):rotate(360, 0.6, -1)

local w = entity.new()
w:setCircleCollider(30)
--w:setAABBCollider(-20, -20, 20, 20)
--w:setLineCollider(40, -50, -30, 20)
--w:rotate(360, 5, -1)

local vq = quad.new("debug/vector.png", 28):alpha(0)

function w:update(dt)
    
    self:position(getMouseWorldPos())
    
    local c, x, y = self:collideWith(sh)
    if c then
        q:position(x, y)
        q:alpha(1)
        --camera.shake(30, 0.5)
    else
        q:alpha(0)
    end
    
    local c, x, y = self:collideGrid()
    if c then
        q2:position(x, y)
        q2:alpha(1)
        --camera.shake(10, 0.5)
    else
        q2:alpha(0)
    end
    
    if isMouseButton(1) then
        local mx, my = getMouseWorldPos()
        local wx, wy = getWallNormal(mx, my, 30, 5)
        if wx == 0 and wy == 0 then
            vq:alpha(0)
        else
            vq:alpha(1)
            vq:position(mx, my)
            vq:rotate(vector.angle(wx, wy))
        end
    end
end

local function showVector(x, y, dx, dy)
    return quad.new("debug/vector.png", 28):scale(0.3, 0.3):position(x, y):alpha(0, 1):rotate(vector.angle(dx, dy)):delete(1)
end

local BALLS  = {}

local function spawnBall(spx, spy)

    local ball = entity.new():texture("debug/boulder.png"):setCircleCollider(15)
    ball:position(spx, spy)
    
    if ball:collideGrid() then
        ball:delete()
        return
    end
    
    --local ballv = quad.new("debug/vector.png", 29)
    ball:gravity(0, 800)
    BALLS[ball] = ball

    function ball:updateFixed(dt)
        
        local vx, vy = self:getVelocity()
        self:rotate(self:getRotation() + dt*vx*2)
        
        --local va = vector.angle(vx, vy)
        --local s = vector.len(vx, vy) / 300
        --ballv:rotate(va)
        --ballv:scale(s, s)
        --ballv:position(self:getPosition())

        local px, py = self:getPosition()
        --if self:collideGrid() then
        --    print("ERROR")
        --end
        
        self:updatePhysics(dt)
        
        local c, x, y = self:collideGrid()
        if c then
            self:position(px, py)
            
            --quad.new(self:getTexture(), 29):position(self:getPosition()):alpha(0.8):alpha(0, 1):delete(1)

            local nx, ny = getWallNormal(x, y)
            showVector(x, y, nx, ny)
            local vx, vy = self:getVelocity()
            vx, vy = vector.reflection(vx, vy, nx, ny)
            vx = vx * 0.99
            vy = vy * 0.75
            --if vector.len(vx, vy) < 20 then vx, vy = vector.setLen(vx, vy, 20) end
            --print(dt, vx, vy)
            --vx, vy = vector.setLen(vx, vy, vector.len(vx, vy) * 0.6)
            self:velocity(vx, vy)
            self:updatePhysics(dt)
        else
            --self.cx, self.cy = self:getPosition()
        end
    end
    
    function ball:onEndOfLife()
        --ballv:delete()
    end
end

local function bounce(b, x, y, dt)
    local px, py = b:getPosition()
    local dx = px - x
    local dy = py - y
    local vx, vy = b:getVelocity()
    local vl = vector.len(vx, vy)
    dx, dy = vector.setLen(dx, dy, vl / 2)
    vx = vx + dx
    vy = vy + dy
    --if vector.len(vx, vy) < 150 then
    --    vx, vy = vector.setLen(vx, vy, 150)
    --end
    vx, vy = vector.setLen(vx, vy, vl * (1 - dt))
    b:velocity(vx, vy)
end

local function stop(b, x, y, dt)
    local px, py = b:getPosition()
    local dx = px - x
    local dy = py - y
    --local a = vector.angle(dx, dy)
    local vx, vy = b:getVelocity()
    local vl = vector.len(vx, vy) * (1 - dt)
    --vx, vy = vector.setLen(vx, vy, vector.len(dx, dy)
    --vx, vy = vector.setLen(dx, dy, vector.len(vx, vy))
    dx, dy = vector.setLen(dx, dy, vl)
    --vx, vy = dx, dy
    vx, vy = dx, dy
    --vx, vy = -vx, -vy
    --vx, vy = vector.reflection(vx, vy, dx, dy)
    showVector(x, y, vx, vy)
    b:velocity(vx, vy)
end
    
local dummy = entity.new()
dummy.lmb = false
dummy.rmb = false
function dummy:updateFixed(dt)
    local rmb = isMouseButton(3)
    if rmb and not self.rmb then
        spawnBall(getMouseWorldPos())
    end
    self.rmb = rmb
    
    for b, _ in pairs(BALLS) do
    
        for b2, _ in pairs(BALLS) do
            if b ~= b2 then
                local c, x, y = b:collideWith(b2)
                if c then
                    bounce(b, x, y, dt)
                    bounce(b2, x, y, dt)
                end
            end
        end
        
        local c, x, y = sh:collideWith(b)
        if c then
            --bounce(b, x, y)
            stop(b, x, y, dt)
        end
        
        x, y = b:getPosition()
        if y > 600 then
            b:delete()
            BALLS[b] = nil
        end
    end
end


--local s = sound.new("klaxonloop.ogg")
--local s = sound.new("alarm.ogg")
--s:play()
--sound.new("klaxonloop.ogg"):play(2)

--music.play("green.spc")


--f:createText("Hello world!", 30, 30, empty, 2)


--local ff = quad.new("insanity.png", 10):blend(BLEND_ADD)

--q:addChild(ff)




--[[local p = player.new()
p:position(400, 300)
p:texture("sprites/olaf1.anim")
camera.follow(p)]]

--local b = button.new(300, 150, "Blarg"):position(400, 400)