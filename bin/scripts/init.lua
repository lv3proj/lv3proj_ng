
print("----- Lua init... -----")


dofile("debug.lua")
dofile("string.lua")
dofile("table.lua")
dofile("ro.lua")
dofile("quad.lua")
dofile("sound.lua")

-- forbid os functions, these are dangerous.
os = nil
package = nil
module = nil
require = nil


function onInit()
    --local q = quad.new("test.png", 4):position(400, 300):scale(1, 1):scale(2, 2, 1, -1, true, true):rotate(-30):rotate(30, 2, -1, true, true)
    --q:alpha(0.2):alpha(0.5, 2, -1, true, true)
    
    --local q = quad.new("insanity.png", 4):position(400, 300):scale(1, 1):scale(2, 2, 1, -1, true, true):rotate(-30):rotate(30, 2, -1, true, true)
    --q:alpha(0.5):alpha(0.7, 2, -1, true, true):blend(BLEND_ADD)
    
    --local q = quad.new("en.anim", 3):position(400, 300):scale(5, 5)
    
    local fr  = quad.new("fractal02.png", 2):position(400, 300):scale(3, 3):parallax(0.8, 0.8)
    local fr2 = quad.new("fractal01.png", 1):position(400, 300):scale(4, 4):parallax(0.5, 0.5)
    local fr2 = quad.new("fractal02.png", 6):position(400, 300):parallax(1.35, 1.35):rotate(90):alpha(0.3):alpha(0.4, 1, -1, true, true)
        :scale(4, 6):scale(6, 4, 3, -1, true, true)
    
    --local s = sound.new("klaxonloop.ogg")
    --local s = sound.new("alarm.ogg")
    --s:play()
    --sound.new("klaxonloop.ogg"):play(2)
    
    music.play("green.spc")
end


function onUpdate(dt)
    
    local z = camera.getScale()

    local wr = getMouseWheelRel()
    if wr ~= 0 then
        local oldz = z
        wr = 1 + (wr * 0.1)
        z = z * wr
        if z < 0.01 then z = 0.01 end
        --print("Zoom: " .. z)
        camera.scale(z, z)
        
        local mx, my = getMouseWindowPos()
        local cx, cy = camera.getPosition()
        cx = cx + mx/oldz - mx/z
        cy = cy + my/oldz - my/z
        camera.position(cx, cy)
    end
    
    if isMouseButton(2) then
        local rx, ry = getMouseWindowRel()
        local cx, cy = camera.getPosition()
        local iz = 1 / z
        camera.position(cx - (iz * rx), cy - (iz * ry))
    end
    
end


function onRender()
    --[[
    local mx, my = getMouseWorldPos()
    drawLine(mx, my, mx, my+20, 10, 0, 1, 0, 0.5)
    
    
    drawLine(0, 0, 800, 0,     10, 0, 1, 1, 0.5)
    drawLine(800, 0, 800, 600, 10, 0, 1, 1, 0.5)
    drawLine(800, 600, 0, 600, 10, 0, 1, 1, 0.5)
    drawLine(0, 600, 0, 0,     10, 0, 1, 1, 0.5)
    ]]
end