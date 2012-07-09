
rawset(_G, "FONTS", {})

newclass("font")

function font.new()
    local f = {}
    setmetatable(f, font)
    f.glyphs = {}
    f.glyphsize = 0
    return f
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
