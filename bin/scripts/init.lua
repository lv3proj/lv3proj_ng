
print("----- Lua init... -----")

collectgarbage("generational")
clearGarbage(true)


dofile("defines.lua")
dofile("debug.lua")
dofile("string.lua")
dofile("table.lua")
dofile("event.lua")
dofile("tq.lua")
dofile("mouse.lua")
dofile("ro.lua")
dofile("quad.lua")
dofile("entity.lua")
dofile("sound.lua")
dofile("class.lua")
dofile("font.lua")
dofile("map.lua")
dofile("pixfont.lua")
dofile("quadtext.lua")
dofile("camera.lua")
dofile("ui.lua")
dofile("textinput.lua")
dofile("player.lua")
dofile("editor_tileset.lua")
dofile("debugkeys.lua")

-- forbid os functions, these are dangerous.
--os = nil -- TODO: add replacement functions in engine
package = nil
require = nil


TQ = tq_create()



rawset(_G, "onInit", function()

    local    f = pixfont:new()
    
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
    
end)


local firstUpdate = true

rawset(_G, "onUpdate", function(dt)

    TQ:update(dt)

    if firstUpdate then
        firstUpdate = false
        --dofile("demo.lua")
        dofile("lv3.lua")
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
    --print(key, mod)
    triggerEvent("onKeyDown", key, mod)
end)

rawset(_G, "onKeyUp", function(key, mod)
    triggerEvent("onKeyUp", key, mod)
end)

rawset(_G, "onJoystickEvent", function(device, type, id, val)

end)

rawset(_G, "getUserInputText", function()
    TEXTINP:alpha(1)
    local r = TEXTINP:getInput()
    TEXTINP:alpha(0)
    return r
end)

