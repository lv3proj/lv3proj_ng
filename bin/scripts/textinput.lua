
newclass("textinput", uibase)

function textinput.new(layer)
    local e = uibase.new(layer)
    setclass(e, textinput)
    e:setPauseLevel(PAUSELEVEL_INPUT)
    e:makeInternal()
    
    e.text = quadtext.new(FONTS.default, 0)
    e:addChild(e.text)
    
    return e
end

local shifttab =
{
    ["-"] = "_",
    ["."] = ":",
    [","] = ";",
}

function textinput:getInput()
    local w, h = self:getWH()
    self.text:position(w * -0.5 + self.text.font:getCharSize() + 32, 0)
    
    local oldpause = getPause()
    setPause(PAUSELEVEL_INPUT)
    local oldOnKeyDown = onKeyDown
    local str = ""
    local lastkey
    -- patch global interface function while the input box is open,
    -- so that no other scripts can receive input in the meantime
    onKeyDown = function(key, mod)
        if key > 0 and key <= 127 then
            if key == KEY_RETURN or key == KEY_ESCAPE then
                -- do nothing
            elseif key == KEY_BACKSPACE then
                if str:len() > 0 then
                    str = str:sub(1, str:len() - 1)
                end
            else
                local c = string.char(key)
                if bit32.btest(mod, KMOD_SHIFT) then
                    c = shifttab[c] or c:upper()
                end
                str = str .. c
            end
            self.text:setText(str .. "_")
        end
        lastkey = key
    end
    self.text:setText("_")
    while not (lastkey == KEY_RETURN or lastkey == KEY_ESCAPE) do
        wait(0.01)
    end
    onKeyDown = oldOnKeyDown
    setPause(oldpause)
    if lastkey == KEY_RETURN then
        print("textinput:getInput() returns: " .. str)
        return str
    end
    print("textinput:getInput() - aborted")
end

