
local modifiedSpeed = false

local function keyDown(key, mod)

    if key == KEY_F11 and bit32.btest(mod, KMOD_CTRL) then
        msgbox(debug.traceback())
    end
    
    if isInEditor() then
        return
    end
    
    if key == KEY_g then
        if bit32.btest(mod, KMOD_SHIFT) then
            setSpeed(6)
        else
            setSpeed(3)
        end
        modifiedSpeed = true
    elseif key == KEY_f then
        if bit32.btest(mod, KMOD_SHIFT) then
            setSpeed(0.1)
        else
            setSpeed(0.5)
        end
        modifiedSpeed = true
    end
end

local function keyUp(key, mod)
    if isInEditor() then
        return
    end
    
    if modifiedSpeed and (key == KEY_g or key == KEY_f) then
        setSpeed(1)
        modifiedSpeed = false
    end
end

addEventListener("onKeyDown", keyDown)
addEventListener("onKeyUp", keyUp)

