
rawset(_G, "FONTS", {})

class "font"
{
    glyphsize = 0;
    glyphs = false;
}

function font:__init()
    print("font:__init()")
    self.glyphs = {}
    self.glyphsize = 0
end

function font:load()
    msgbox("font:load() base call")
end

function font:getChar(c)
    return self.glyphs[c] or self.glyphs["?"]
end

function font:getCharSize()
    return self.glyphsize
end


-- OBSOLETE
--[[
function font:createText(s, x, y, parent, scale)

    s = s:upper() -- HACK FIXME
    
    print("font:createText()", s, self.glyphsize)

    if not scale then scale = 1 end
    local startx = x
    local gs = self.glyphs
    local len = s:len()
    local sz = scale * self.glyphsize
    local c, g, q
    
    for i = 1, len do
        c = s:sub(i, i)
        if c == "\n" then
            x = startx
            y = y + sz
        else
            g = gs[c] or gs["?"]
            q = quad.new(g, parent:getLayer()):scale(scale, scale):position(x, y)
            print(string.format("Char %s at (%u, %u) = %s", c, x, y, g))
            x = x + sz
            parent:addChild(q)
        end
    end

end
]]
