
dofile("testmap.lua")

--[[for i = 1, 50 do
    quad.new("test.png", 15):scale(0.2, 0.2):position(math.random(-100, 900), math.random(-100, 700)):alpha(0.2)
end]]

local snd_rock = sound.new("bigrockhit.ogg")
local snd_land = sound.new("land3.ogg")
local snd_expl = sound.new("explosion.ogg"):volume(0.5)

local sh = entity.new()
--sh:setCircleCollider(150)
sh:texture("test3.png"):position(400, 300)
--sh:setAABBCollider()
sh:setLineCollider(-130, 150, 80, 150)
sh:rotate(-10):rotate(10, 3, -1, true, true)

function sh:update(dt)
    --local x, y = self:getPosition()
    --self:position(x + math.random(-2, 2), y + math.random(-2, 2))
end

--[[
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
]]

local function showVector(x, y, dx, dy, c, s)

    s = s or vector.len(dx, dy) / 100
    local q = quad.new("debug/vector.png", 26):scale(0.3, s):position(x, y):alpha(0.3):alpha(0, 0.4):rotate(vector.angle(dx, dy)):delete(2)
        if c == "r" then q:color(1, 0, 0)
    elseif c == "g" then q:color(0, 1, 0)
    elseif c == "b" then q:color(0, 0, 1)
    elseif c == "n" then q:color(1, 0, 1)
    elseif c == "v" then q:color(0, 0, 0)
    elseif c == "y" then q:color(1, 1, 0)
    elseif c == "w" then q:color(0, 1, 1) end
    return q
end
rawset(_G, "showVector", showVector)


-------------------
local function slide(b, nx, ny)
    -- position + vel
    local vx, vy = b:getVelocity()
    local px, py = b:getPosition()
    --showVector(px, py, vx, vy, "g")
    -- normal
    --local nx = px - x
    --local ny = py - y
    local a = vector.angle(nx, ny)
    
    local rx, ry = vector.setLen(nx, ny, vector.len(vx, vy)) --vector.reflection(vx, vy, nx, ny)
    --showVector(x, y, rx, ry, "r")
    
    vx, vy = vx + rx * 0.5, vy + ry * 0.5
    
    showVector(px, py, vx, vy, "y")
    b:velocity(vx, vy)
    
    b:checkSleep(vx, vy)
end
-------------------


local BALLS  = {}

local function spawnBall(spx, spy)

    local ball = entity.new():texture("debug/boulder.png"):setCircleCollider(15)
    ball.soundT = 0
    ball:position(spx, spy)
    ball.timer = 10
    ball:color(1, 0.33, 0.1, 10)
    --ball:velocity(-400, -400)
    
    if ball:collideGrid() then
        ball:delete()
        return
    end
    
    function ball:awake()
        self.sleeptime = 30
    end
    function ball:sleeping()
        return self.sleeptime <= 0
    end
    
    ball:friction(20, 20)
    ball:awake()
    
    --local ballv = quad.new("debug/vector.png", 29)
    ball:gravity(0, 800)
    BALLS[ball] = ball
    
    function ball:checkSleep(vx, vy, a)
        --print(vx, vy)
        if ((not a) or (a <= 1 or a >= 359)) and math.abs(vx) < 5 and math.abs(vy) < 30 then
            self.sleeptime = self.sleeptime - 1
            if self.sleeptime <= 0 then
                self:velocity(0, 0)
                return true
            end
        else
            self:awake()
        end
        return false
    end
    
    function ball:playSoundImpact(hard)
        if self.soundT <= 0 then
            if hard then
                snd_rock:play()
            else
                snd_land:play()
            end
            self.soundT = 0.2
        end
    end
    
    function ball:explode()
        for i = 0, 1 do
            quad.new("lines.png", 29):rotate(40, 0.3):scale(2+i, 3-i, 0.3):blend(BLEND_ADD):position(self:getPosition()):alpha(0, 0.3):color(1, 0.5, 0):delete(3)
            quad.new("energyring.png", 29):scale(2+i, 3-i, 0.3):blend(BLEND_ADD):position(self:getPosition()):alpha(0, 0.3):color(1, 0.5, 0):delete(3)
        end
        local q = quad.new("debug/boom.png", 27):scale(2.3, 2.3 ):scale(3, 3, 1):position(self:getPosition()):alpha(0, 1):delete(1)
    
        for b, _ in pairs(BALLS) do
            if b ~= self then
                local dx, dy = self:vectorTo(b)
                if vector.len(dx, dy) < 234 then
                    dx, dy = vector.setLen(dx, dy, 500)
                    b:awake()
                    b:addVel(dx, dy)
                end
            end
        end
        snd_expl:play()
        self:alpha(0, 0.2)
        self:delete(5)
        camera.shake(15, 0.5)
    end
    
    
    function ball:update(dt)
        if self.soundT >= 0 then
            self.soundT = self.soundT - dt
        end
        
        self.timer = self.timer - dt
        if self.timer < 0 then
            self.timer = 999
            self:explode()
        end
    end
    
    function ball:onImpact()
        local vl = vector.len(self:getVelocity())
        
        if vl > 180 then
            if vl > 300 then
                camera.shake(vl / 100, 0.3)
                self:playSoundImpact(true)
            else
                self:playSoundImpact(false)
            end
        end
    end

    function ball:updateFixed(dt)
    
        if self.sleeptime <= 0 then
            return
        end
        
        local vx, vy = self:getVelocity()
        local px, py = self:getPosition()
        
        self:rotate(self:getRotation() + dt*vx*2)
        
        local vl = vector.len(vx, vy)
        if vl > 300 then
            quad.new("debug/boulder.png", 8):rotate(self:getRotation()):position(px, py):alpha(0.2):alpha(0, 0.2):delete(5)
        end
        
        --local va = vector.angle(vx, vy)
        --local s = vector.len(vx, vy) / 300
        --ballv:rotate(va)
        --ballv:scale(s, s)
        --ballv:position(self:getPosition())

        --if self:collideGrid() then
        --    print("ERROR")
        --end
        
        self:updatePhysics(dt)
        
        local c, x, y = self:collideGrid()
        if c then
            self:position(px, py)
            
            --quad.new(self:getTexture(), 29):position(self:getPosition()):alpha(0.8):alpha(0, 1):delete(1)

            local nx, ny = getWallNormal(x, y, 3)
            showVector(x, y, nx, ny, "w", 0.3)
            local vx, vy = self:getVelocity()

            self:onImpact()
            
            --print(vl)
            --if vl < 60 and vx < 3 and nx == 0 then
            --    self:velocity(0, 0)
            --    self.moving = false
            --    return
            --end
            --vx, vy = vector.reflection(vx, vy, nx, ny)
            --vx = vx * 0.8
            --vy = vy * 0.3
            
            --if vector.len(vx, vy) < 20 then vx, vy = vector.setLen(vx, vy, 20) end
            --print(dt, vx, vy)
            --vx, vy = vector.setLen(vx, vy, vector.len(vx, vy) * 0.6)
            
            --self:velocity(vx, vy)
            
            
            slide(self, nx, ny)
            --showVector(x, y, vx, vy, "b")
            --self:updatePhysics(dt)
            self:moveOutOfWall()
        else
            --self.cx, self.cy = self:getPosition()
        end
    end
    
    function ball:onEndOfLife()
        --ballv:delete()
        entity.onEndOfLife(self)
        BALLS[self] = nil
    end
end

local function bounce(b, x, y, m)
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
    vx, vy = vector.setLen(vx, vy, vl * m)
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
    showVector(x, y, vx, vy, "g")
    b:velocity(vx, vy)
end

local function slide2(b, x, y)
    -- position + vel
    local px, py = b:getPosition()
    local vx, vy = b:getVelocity()
    local vl = vector.len(vx, vy)
    showVector(x, y, vx, vy, "g")
    -- normal
    local nx = px - x
    local ny = py - y
    local na = vector.angle(nx, ny)
    local ux, uy = vector.setLen(nx, ny, vl)
    showVector(x, y, ux, uy)
    
    -- perpendicular to normal (represents the slope we're going down)
    local lx, ly --= vector.reflection(vx, vy, nx, ny)
    if na < 180 then
        lx, ly = -uy, ux
    else
        lx, ly = uy, -ux
    end
    
    -- project angle of normal onto the slope
    -- that way the more slanted the slope is, the faster we'll go down
    
    
    --lx, ly = vector.setLen(lx, ly, vl)
    
    showVector(x, y, lx, ly, "n")
    lx, ly = vector.projectOnto(ux, uy, 0, -1)
    --print(ux, uy)
    
    
    
    vx, vy = vector.reflection(vx, vy, nx, ny)
    showVector(x, y, vx, vy, "r")
    --vx = vx + lx
    --vy = vy + ly
    
    --vx, vy = lx, ly
    
    
    --vx, vy = vector.setLen(vx, vy, vl)
    --vy = vy * 0.5
    --vy = 0
    --vx = vx * 1.05
    
    vy = 0
    vx = vx * 1.5
    
    showVector(x, y, vx, vy, "y")
    b:velocity(vx, vy)
    
    b:moveOutOfEntity(sh)
end
    
local dummy = entity.new()
dummy.lmb = false
dummy.rmb = false
function dummy:updateFixed(dt)
    local rmb = isMouseButton(1)
    if rmb and not self.rmb then
        spawnBall(getMouseWorldPos())
    end
    self.rmb = rmb
    
    for b, _ in pairs(BALLS) do
    
        if not b:sleeping() then
        
            for b2, _ in pairs(BALLS) do
                if b ~= b2 then
                    local c, x, y = b:collideWith(b2)
                    if c then
                        b2:awake()
                        local tx, ty = b:vectorTo(b2)
                        showVector(x, y, tx, ty, "v")
                        --tx, ty = b2:vectorTo(b)
                        --showVector(x, y, tx, ty, "r")
                        --b:moveOutOfEntity(b2)
                        --b:moveApart(b2)
                            --stop(b, x, y, dt)
                        bounce(b, x, y, 0.9)
                        bounce(b2, x, y, 0.9)
                        
                        
                    end
                end
            end
            
            local c, x, y = sh:collideWith(b)
            if c then
                --bounce(b, x, y, 1)
                --stop(b, x, y, dt)
                b:onImpact()
                
                local px, py = b:getPosition()
                slide(b, px - x, py - y)
                b:moveOutOfEntity(sh, 7, 1)
            end
            
            x, y = b:getPosition()
            if y > 600 then
                b:delete()
            end
        end
    end
end


--local s = sound.new("klaxonloop.ogg")
--local s = sound.new("alarm.ogg")
--s:play()
--sound.new("klaxonloop.ogg"):play(2)

music.play("gwed-01.spc")
music.volume(0.5)


--f:createText("Hello world!", 30, 30, empty, 2)


--local ff = quad.new("insanity.png", 10):blend(BLEND_ADD)

--q:addChild(ff)




--[[local p = player.new()
p:position(400, 300)
p:texture("sprites/olaf1.anim")
camera.follow(p)]]

--local b = button.new(300, 150, "Blarg"):position(400, 400)