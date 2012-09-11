
print("----- Lua init... -----")

collectgarbage("generational")
clearGarbage(true)


dofile("defines.lua")
dofile("debug.lua")
dofile("string.lua")
dofile("table.lua")
dofile("mouse.lua")
dofile("ro.lua")
dofile("quad.lua")
dofile("entity.lua")
dofile("sound.lua")
dofile("class.lua")
dofile("font.lua")
dofile("pixfont.lua")
dofile("quadtext.lua")
dofile("camera.lua")
dofile("ui.lua")
dofile("textinput.lua")
dofile("player.lua")
dofile("editor_tileset.lua")

-- forbid os functions, these are dangerous.
os = nil -- TODO: add replacement functions in engine
package = nil
require = nil


rawset(_G, "onInit", function()
    --local q = quad.new("test.png", 4):position(400, 300):scale(1, 1):scale(2, 2, 1, -1, true, true):rotate(-30):rotate(30, 2, -1, true, true)
    --q:alpha(0.2):alpha(0.5, 2, -1, true, true)
    
    --local q = quad.new("insanity.png", 4):position(400, 300):scale(1, 1):scale(2, 2, 1, -1, true, true):rotate(-30):rotate(30, 2, -1, true, true)
    --q:alpha(0.5):alpha(0.7, 2, -1, true, true):blend(BLEND_ADD)
    
    --local q = quad.new("en.anim", 3):position(400, 300):scale(5, 5)
    
    local fr  = quad.new("fractal.png", 2):position(400, 300):scale(3, 3):parallax(0.8, 0.8):alpha(0.1):alpha(0.25, 2, -1, true, true):color(1, 0.4, 0)
    local fr2 = quad.new("fractal.png", 1):position(400, 300):scale(4, 4):parallax(0.5, 0.5):alpha(0.2):alpha(0.4, 2.3, -1, true, true):color(0, 1, 0)
    local fr2 = quad.new("fractal.png", 6):position(600, 300):parallax(1.35, 1.35):rotate(40):alpha(0.3):alpha(0.4, 1, -1, true, true):scale(1.1, 1.2, 6, -1, true, true):color(0, 0, 1):color(1, 0, 0, 4, -1, true, true)
                :rotate(30, 10, -1, true, true)
        --:scale(4, 6):scale(6, 4, 3, -1, true, true)
    
    
    
    local    f = pixfont:new()
    print(tostring(f))
    for k, v in pairs(f) do
        print(k, " => ", v)
    end
    --local x = f.derp
    
    
    local empty = quad.new("", 30):scale(2, 2, 0.5, -1, true, true)

    local chars = "               °"
               .."´      !\"#$%&'()"
               .. "*+,-./0123456789"
               .. ":;<=>?@ABCDEFGHI"
               .. "JKLMNOPQRSTUVWXY"
               .."Z[\\]^_    ` µ  ~" -- FIXME: the rest?
               
    f:load("font/tlv2_font.png", chars, 8, 16, 0, 0)
    
    FONTS.lv2 = f
    FONTS.default = f
    
    dofile("debug_overlay.lua")
 
    
    TEXTINP = textinput.new(31):position(400, 300):texture("white.png"):color2(0,0,0.4):setWH(500, 50):alpha(0)
    
    dofile("editor.lua")
    
    setLayerParallax(30, 0, 0)
    setLayerParallax(31, 0, 0)
    
    dofile("lv3.lua")
    
end)


local firstUpdate = true

rawset(_G, "onUpdate", function(dt)

    if firstUpdate then
        firstUpdate = false
        clearGarbage()
    end
    
    camera.update(dt)
    
    UI.update(dt)
    
end)


rawset(_G, "onRender", function()
    --[[
    local mx, my = getMouseWorldPos()
    drawLine(mx, my, mx, my+20, 10, 0, 1, 0, 0.5)
    
    
    drawLine(0, 0, 800, 0,     10, 0, 1, 1, 0.5)
    drawLine(800, 0, 800, 600, 10, 0, 1, 1, 0.5)
    drawLine(800, 600, 0, 600, 10, 0, 1, 1, 0.5)
    drawLine(0, 600, 0, 0,     10, 0, 1, 1, 0.5)
    ]]
end)

rawset(_G, "onKeyDown", function(key, mod)
    if key == KEY_i then
        TEXTINP:alpha(1)
        TEXTINP:getInput()
        TEXTINP:alpha(0)
    end
end)

rawset(_G, "onKeyUp", function(key, mod)

end)

