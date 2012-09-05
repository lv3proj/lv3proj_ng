
dofile("testmap.lua")

for i = 1, 50 do
    quad.new("test.png", 15):scale(0.2, 0.2):position(math.random(-100, 900), math.random(-100, 700)):alpha(0.2)
end


local e = entity.new()
--e:setCircleCollider(150)
e:texture("test3.png"):position(400, 300)
--e:setAABBCollider()
e:setLineCollider(0, 0, 100, 200)
e:rotate(90, 5, -1, true, true)

function e:update(dt)
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

local RADTODEG = 180.0 / 3.14159265359
local DEGTORAD = 3.14159265359 / 180.0

local function vector_getAngleDeg(vx, vy)
    return (math.atan2(vy, vx) * RADTODEG) + 90
end

function w:update(dt)
    self:position(getMouseWorldPos())
    
    local c, x, y = self:collideWith(e)
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
            vq:rotate(vector_getAngleDeg(wx, wy))
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