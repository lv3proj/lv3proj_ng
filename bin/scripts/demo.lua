
local function HSVtoRGB(h, s, v)
 
  local r, g, b
 
  local i = math.floor(h * 6)
  local f = h * 6 - i
  local p = v * (1 - s)
  local q = v * (1 - f * s)
  local t = v * (1 - (1 - f) * s)
 
  local switch = i % 6
  if switch == 0 then
    r = v g = t b = p
  elseif switch == 1 then
    r = q g = v b = p
  elseif switch == 2 then
    r = p g = v b = t
  elseif switch == 3 then
    r = p g = q b = v
  elseif switch == 4 then
    r = t g = p b = v
  elseif switch == 5 then
    r = v g = p b = q
  end
 
  return r, g, b
 
end

local TQ = tq_create()

local PI2 = math.pi * 2
local RADTODEG = 180.0 / 3.14159265359
local DEGTORAD = 3.14159265359 / 180.0

local text = entity.new(31)
text:position(400, 300)
text.text = quadtext.new(FONTS.default, 0)
text:addChild(text.text)
text:alpha(0)

local function write(s, decay)
    decay = decay or 2
    local ss = s:explode("\n")
    local m = 0
    for _, x in pairs(ss) do
        m = math.max(m, #x)
    end
    text.text:setText(s)
    text.text:offset((-m/2) * text.text.font:getCharSize(), 0)
    
    text:alpha(1)
    text:alpha(0, decay, 0, false, true)
    
    text:scale(3.6, 3.6)
    text:scale(4.2, 32, decay, false, true)
end

setSpeed(1)


dofile("demomap.lua")

--music.play("_LufiaLastDuel.spc")
--music.play("heaven7.xm")
--music.volume(1)


local D -- main thing


local snd_jump = sound.new("jump.ogg")
local snd_hit = sound.new("hit.ogg"):volume(0.18)
local snd_exp1 = sound.new("ex1.ogg"):volume(0.5)
local snd_exp2 = sound.new("ex2.ogg")

local BOMBRANGE =
{
    { d = true, x = 0, y = 0 },
    
    { d = true, x = 0, y = -1 },
    { d = true, x = 0, y = 1 },
    { d = true, x = -1, y = 0 },
    { d = true, x = 1, y = 0 },
    
    { d = false, x = -1, y = -1 },
    { d = false, x = -1, y = 1 },
    { d = false, x = 1, y = -1 },
    { d = false, x = 1, y = 1 },
    
    { d = false, x = -2, y = 0 },
    { d = false, x = 2, y = 0 },
    { d = false, x = 0, y = 2 },
    { d = false, x = 0, y = -2 },
}

local TILE_SIZE = 16

local BLOCK_NORMAL = 0
local BLOCK_CRACKED = 1
local BLOCK_SOLID = 2

-- hacky
local RAIL_LEFT = 4 * TILE_SIZE
local RAIL_RIGHT = 800 - RAIL_LEFT
local RAIL_Y = 25


local BLOCKTYPES = 
{
    ["greyblock.png"] = BLOCK_NORMAL,
    ["greyblockcracked.png"] = BLOCK_CRACKED,
    ["block1.png"] = BLOCK_SOLID,
}

local BALLS = {}


local function spawnBlockFrag(x, y)
    local e = entity.new()
    e:texture("greyblockfrag.png")
    e:setCircleCollider(3)
    
    e.t = 2
    
    function e:update(dt)
        self:updatePhysics(dt)
        local vx, vy = self:getVelocity()
        self:rotate(self:getRotation() + vx*dt*5)
        
        local c, x, y = self:collideGrid()
        if c then
            local nx, ny = getWallNormal(x, y)
            vx, vy = vector.reflection(vx, vy, nx, ny)
            self:velocity(0, 0)
            self:addVel(vx * 0.7, vy * 0.4 - 7)
        end
        
        
        if self.t >= 0 then
            self.t = self.t - dt
            if self.t <= 0 then
                self:delete(0.5)
                self:alpha(0, 0.5)
            end
        end
    end
    
    e:gravity(0, 300)
    e:position(x, y)
    
    e:addVel(math.random(-200, 200), -math.random(100, 300))

    return e
end

local explode

local function gameover(pp)
    if D.gameover then
        return
    end
    D.gameover = true
    D.p[pp.pid] = nil
    
    explode(pp, true, 4)
    
    camera.shake(30, 0.3)
    setSpeed(0.2)
    setSpeed(1, 1, 0, false, true)
    
    write("Player " .. pp.pid .. " boomed!")
    
    for i = 1, 3 do
        TQ:push(i, write, "New round in " .. 4 - i .. "...", 1)
    end
    TQ:push(4, resetEngine)
end


explode = function(self, alt, sz)
    sz = sz or 1
    for i = 0, 1 do
        quad.new("lines.png", 29):rotate(40, 0.3):scale(sz, sz):scale((2+i)*sz, (3-i)*sz, 0.3):blend(BLEND_ADD):position(self:getPosition()):alpha(0, 0.3):color(1, 0.5, 0):delete(3)
        quad.new("energyring.png", 29):scale(0.7*sz, 0.7*sz):scale((2+i)*sz, (3-i)*sz, 0.3):blend(BLEND_ADD):position(self:getPosition()):alpha(0, 0.3):color(1, 0.5, 0):delete(3)
    end
    local q = quad.new("boom.png", 27):scale(2.3*sz, 2.3*sz):scale(3*sz, 3*sz, 1):position(self:getPosition()):alpha(0, 1):delete(1)
    
    camera.shake(10 * sz, 0.5)
        
    for b, _ in pairs(BALLS) do
        if b ~= self then
            local dx, dy = self:vectorTo(b)
            local L = vector.len(dx, dy)
            if L < 50 then
                b.health = b.health - 5
            elseif L < 200 then
                dx, dy = vector.setLen(dx, dy, 300)
                b:addVel(dx, dy)
            end
        end
    end
    
    if not D.gameover then
        for i, pp in pairs(D.p) do
            if pp then
                local dx, dy = self:vectorTo(pp)
                local L = vector.len(dx, dy)
                if L < 40 then
                    gameover(pp)
                end
            end
        end
    end
    
    if alt then
        snd_exp1:play()
    else
        snd_exp2:play()
    end
    
    self:alpha(0, 0.2)
    self:delete(1)

    self.update = nil
    
end


local function crushWall(x, y)
    for _, t in pairs(BOMBRANGE) do
        local xx, yy = (x/TILE_SIZE) + t.x, (y/TILE_SIZE) + t.y
        local block = getTile(4, xx, yy)
        
        if block and block ~= "" then
            local tt = BLOCKTYPES[block]
            --print(block, xx, yy, tt)
            if tt ~= BLOCK_SOLID then
                if t.d or tt == BLOCK_CRACKED then
                    setTile(4, xx, yy, nil)
                elseif tt == BLOCK_NORMAL then
                    setTile(4, xx, yy, "greyblockcracked.png")
                end
                
                for i = 1, 4 do
                    spawnBlockFrag(xx * TILE_SIZE, yy * TILE_SIZE)
                end
            end
        end
    end
end



-------------------
local function slide(b, nx, ny)
    -- position + vel
    local vx, vy = b:getVelocity()
    local px, py = b:getPosition()
    
    local a = vector.angle(nx, ny)
    
    local rx, ry = vector.setLen(nx, ny, vector.len(vx, vy)) --vector.reflection(vx, vy, nx, ny)
    --showVector(x, y, rx, ry, "r")
    
    vx, vy = vx + rx * 0.5, vy + ry * 0.5
    
    --showVector(px, py, vx, vy, "y")
    b:velocity(vx, vy)
end
-------------------


local BALLT = 10

local function spawnBall(spx, spy)

    local ball = entity.new():texture("boulder.png"):setCircleCollider(7)
    ball.soundT = 0
    ball:position(spx, spy)
    ball:scale2(0.5, 0.5)
    ball.health = BALLT
    ball.vib = false
    
    if ball:collideGrid() then
        ball:delete()
        return
    end
    
    
    ball:friction(20, 20)
    
    ball:gravity(0, 800)
    BALLS[ball] = ball
    
    function ball:playSoundImpact(hard)
        if self.soundT <= 0 then
            snd_hit:play()
            self.soundT = 0.2
        end
    end
    
    ball.explode = explode

    
    
    function ball:update(dt)
        local gc = ((self.health / BALLT) * 0.66) + 0.33
        local bc = ((self.health / BALLT) * 0.9) + 0.1
        self:color(1, gc, bc)
        
        if self.health < 1.5 and not self.vib then
            self.vib = true
            self:offset(-2, 0)
            self:offset(2, 0, 0.04, -1, true)
        end
        
        local dx, dy = self:getVelocity()
        if vector.len(dx, dy) > 450 then
            dx, dy = vector.setLen(dx, dy, 450)
            self:velocity(dx, dy)
        end
        
        if self.soundT >= 0 then
            self.soundT = self.soundT - dt
        end
        
        self.health = self.health - dt
        if self.health < 0 then
            self.health = 999
            self:explode(true, 0.6)
            crushWall(self:getPosition())
        end
        
        local x, y = self:getPosition()
        if y > 700 then
            self:delete()
        end
    end
    
    function ball:onImpact()
        local vl = vector.len(self:getVelocity())
        
        if vl > 180 then
            if vl > 300 then
                --camera.shake(vl / 100, 0.3)
                self:playSoundImpact()
            end
        end
    end

    function ball:updateFixed(dt)
        
        local vx, vy = self:getVelocity()
        local px, py = self:getPosition()
        
        self:rotate(self:getRotation() + dt*vx*2)
        
        local vl = vector.len(vx, vy)
        if vl > 300 then
            quad.new("boulder.png", 8):rotate(self:getRotation()):position(px, py):alpha(0.2):alpha(0, 0.2):scale(self:getScale2()):delete(5)
        end
        
        self:updatePhysics(dt)
        
        local c, x, y = self:collideGrid()
        if c then
            self:position(px, py)
            
            local nx, ny = getWallNormal(x, y, 3)
            --showVector(x, y, nx, ny, "w", 0.3)
            local vx, vy = self:getVelocity()

            self:onImpact()
            slide(self, nx, ny)
            self:moveOutOfWall(3, 3, 9)
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
    vx, vy = vector.setLen(vx, vy, vl * m)
    b:velocity(vx, vy)
end


local function bombUpdate(self, dt)
    self:updatePhysics(dt)
    
    local px, py = self:getPosition()
    local vx, vy = self:getVelocity()
    
    if (vx < 0 and px <= RAIL_LEFT) or (vx > 0 and px >= RAIL_RIGHT) then
        self:velocity(-vx, vy)
    end
    
    local c, x, y = self:collideGrid()
    
    if not c and not D.gameover then
        for i, pp in pairs(D.p) do
            if pp then
                c, x, y = self:collideWith(pp)
                if c then break end
            end
        end
    end
    
    if c then
        self:explode()
        crushWall(x, y)
    end
end

local function spawnBomb(x, y, fall)
    local e = entity:new()
    e.explode = explode
    e.update = bombUpdate
    e:setCircleCollider(5)
    
    e:texture("bomb.png")
    if fall then
        e:gravity(0, fall)
    end
    e:scale(1.2, 1.2)
    e:scale(2.5, 2.5, 0.5, -1, true, true)
    
    e:position(x, y)
    
    return e
end



local function spawnFrac(h1, h2, rot)

    local q =  quad.new("fractal.png", 2):position(400, 300):parallax(0.8, 0.8):color(1, 0.4, 0)
    
    local t = 2
    local s = 8
    local ss = 0.4
    q:alpha(0)
    q:alpha(1, t, -1, true)
    q:scale(ss, ss)
    q:scale(s, s, 4)
    q:scale2(1.5, 0.6)
    q:scale2(0.6, 1.5, t*2, 1, true)
    q:rotate(rot)
    q:rotate(rot-60, t*2)
    q:color(HSVtoRGB(h1, 1, 1))
    local r, g, b = HSVtoRGB(h2, 1, 1)
    q:color(r, g, b, t*2, -1, true, false)
    q:delete(1/(t*2))
    return q
end


local function getPlayerColor(p)
    if p == 1 then
        return 1, 0, 1
    elseif p == 2 then
        return 0, 1, 0
    end
end



------------------------------------------------

D = entity.new()
D.mt = 0
D.rt = 0
D.ballT = 4
D.bomb = { false, false }
D.bombT = { 0, 0 }
D.gameover = false
D.rbT = 3

function D:update(dt)

    TQ:update(dt)

    self.rt = (self.rt + dt) % PI2
    
    if self.mt <= 0 then
        self.mt = self.mt + 1
        local m = self.rt * 5
        spawnFrac(math.sin(m), math.sin(m+0.5), RADTODEG * self.rt)
        spawnBomb(math.random(100, 700), -20)
    else
        self.mt = self.mt - dt
    end
    
    if self.ballT <= 0 then
        spawnBall(math.random(100, 700), -20)
        self.ballT = self.ballT + math.random(8, 30) / 10
    else
        self.ballT = self.ballT - dt
    end
    
    if self.rbT <= 0 then
        spawnBomb(math.random(100, 700), -20, 200)
        self.rbT = self.rbT + math.random(20, 50) / 10
    else
        self.rbT = self.rbT - dt
    end
    
    for i = 1, 2 do
        if self.bomb[i] and not isObjectValid(self.bomb[i]) then
            self.bomb[i] = false
            self.bombT[i] = 0
        end
        if not self.bomb[i] then
            self.bombT[i] = self.bombT[i] + dt
            if self.bombT[i] >= 1 then
                local b = spawnBomb(math.random(RAIL_LEFT, RAIL_RIGHT), RAIL_Y)
                self.bomb[i] = b
                self.bombT[i] = 0
                b:color(getPlayerColor(i))
                if math.random(0, 100) < 50 then
                    b:addVel(100, 0)
                else
                    b:addVel(-100, 0)
                end
            end
        end
    end
end

function D:updateFixed(dt)
    
    for b, _ in pairs(BALLS) do
        for b2, _ in pairs(BALLS) do
            if b ~= b2 then
                local c, x, y = b:collideWith(b2)
                if c then
                    local tx, ty = b:vectorTo(b2)
                    bounce(b, x, y, 0.9)
                    bounce(b2, x, y, 0.9)
                    b:moveOutOfEntity(b2)
                end
            end
        end
        
        for _, pp in pairs(D.p) do
        local c, x, y = b:collideWith(pp)
            if c then
                local tx, ty = pp:vectorTo(b)
                tx, ty = vector.setLen(tx, ty, 100)
                bounce(b, x, y, 0.9)
                b:addVel(tx, -40)
                b:moveOutOfEntity(pp)
                b:moveOutOfWall()
            end
        end
    end
end

local function createPlayer(p, x, y)
    local e = entity.new(15)
    e.pid = p
    e:setCircleCollider(15)
    e:texture("orb.png")
    e:friction(300, 0)
    e:color(getPlayerColor(p))
    e:gravity(0, 760)
    e.movedir = 0
    e:position(x, y)
    e.jT = 0
    
    function e:update(dt)
        local vx, vy = self:getVelocity()
        self:rotate(self:getRotation() + vx*dt*5)
        self.jT = self.jT - dt
        
        local x, y = self:getPosition()
        if y > 600 then
            gameover(self)
        end
    end
    
    function e:updateFixed(dt)
        local px, py = self:getPosition()
        self:updatePhysics(dt)
        local c, x, y = self:collideGrid()
        if c then
            local vx, vy = self:getVelocity()
            local nx, ny = getWallNormal(self:getPosition())
            self:position(px, py)
            
            local nx, ny = getWallNormal(x, y, 3)
            --showVector(x, y, nx, ny, "w", 0.3)
            local vx, vy = self:getVelocity()

            --self:onImpact()
            slide(self, nx, ny)
            self:moveOutOfWall()
            
            self.collx = x
            self.colly = y
            self.jT = 0.2
        end
        
        self:addVel(self.movedir * dt * 2000, 0)
        --print(self.pid, self.movedir)
        
        local vx, vy = self:getVelocity()
        
        -- x
        local maxv = 200
        if vx > maxv then
            vx = maxv
        elseif vx < -maxv then
            vx = -maxv
        end
        
        -- y
        maxv = 300
        if vy > maxv then
            vy = maxv
        elseif vy < -maxv then
            vy = -maxv
        end
        
        self:velocity(vx, vy)
    end
    
    function e:jump()
        if self.jT > 0 then
            local x, y = self:getPosition()
            local nx, ny = getWallNormal(self.collx, self.colly)
            local a = (vector.angle(nx, ny) + 360) % 360
            if a > 300 or a < 60 then
                self.canjump = false
                self:addVel(0, -300)
            end
        end
    end
    
    return e
end

D.p = { createPlayer(1, 100, 100), createPlayer(2, 700, 100) }

local ACTION_LEFT = 0
local ACTION_RIGHT = 1
local ACTION_JUMP = 2
local ACTION_DROP = 3

local function dropBomb(p, down)
    if down and D.bomb[p] then
        D.bomb[p]:gravity(0, 300)
        D.bomb[p] = false
    end
end

local function playerJump(p, down)
    if down then
        D.p[p]:jump()
    end
end

local function playerMove(p, left, down)
    local player = D.p[p]
    if down then
        if left then
            player.movedir = -1
        else
            player.movedir = 1
        end
    else
        player.movedir = 0
    end
end

local function handlePlayerInput(ac, p, down)
    if not D.p[p] then
        return
    end
    
    if ac == ACTION_DROP then
        dropBomb(p, down)
    elseif ac == ACTION_LEFT then
        playerMove(p, true, down)
    elseif ac == ACTION_RIGHT then
        playerMove(p, false, down)
    elseif ac == ACTION_JUMP then
        playerJump(p, down)
    end
end



local ACTIONS = 
{
    [KEY_UP]        = { action = ACTION_JUMP, player = 1 },
    [KEY_DOWN]      = { action = ACTION_DROP, player = 1 },
    [KEY_RIGHT]     = { action = ACTION_RIGHT, player = 1 },
    [KEY_LEFT]      = { action = ACTION_LEFT, player = 1 },
    
    [KEY_w]         = { action = ACTION_JUMP, player = 2 },
    [KEY_s]         = { action = ACTION_DROP, player = 2 },
    [KEY_d]         = { action = ACTION_RIGHT, player = 2 },
    [KEY_a]         = { action = ACTION_LEFT, player = 2 },
}

local function handleJoystickAxis(device, axis, left, on)
    if axis == 0 then -- L/R
        --print(device, axis, left, on)
        if left then
            handlePlayerInput(ACTION_LEFT, device+1, on)
        else
            handlePlayerInput(ACTION_RIGHT, device+1, on)
        end
    end
end

local function handleJoystickButton(device, button, down)
    if button == 0 then
        handlePlayerInput(ACTION_JUMP, device+1, down)
    elseif button == 1 then
        handlePlayerInput(ACTION_DROP, device+1, down)
    end
end


function onKeyDown(key, mod)
    local ac = ACTIONS[key]
    if ac then
        handlePlayerInput(ac.action, ac.player, true)
    end
    math.random()
    if key == KEY_ESCAPE then
        os.exit()
    end
end

function onKeyUp(key, mod)
    local ac = ACTIONS[key]
    if ac then
        handlePlayerInput(ac.action, ac.player, false)
    end
    math.random()
end

local joyaxis = {}

local function filterJoyDeadzone(device, axis, val)
    local old = joyaxis[device]
    joyaxis[device] = math.abs(val) > 10000 -- dead zone, in [-32767, 32768]
    --print(device, axis, val)
    if old ~= joyaxis[device] then
        handleJoystickAxis(device, axis, val < 0, joyaxis[device])
    end
end

function onJoystickEvent(device, type, id, val)
    --print(device, type, id, val)
    if type == SDL_JOYAXISMOTION then
        filterJoyDeadzone(device, id, val)
    elseif type == SDL_JOYBUTTONDOWN then
        handleJoystickButton(device, id, val)
    elseif type == SDL_JOYBUTTONUP then
        handleJoystickButton(device, id, val)
    end
end


