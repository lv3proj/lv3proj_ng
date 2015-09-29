
newclass("pixfont", font)

function pixfont.new()
    local f = font.new()
    setmetatable(f, pixfont)
    return f
end

function pixfont:load(file, chars, sz, rowlen, x, y) -- assumes (sz)x(sz) tiles
    x = x or 0
    y = y or 0
    
    self.glyphsize = sz
    
    local len = chars:len()
    local c, q
    local fn
    local gs = self.glyphs
    for i = 1, len do
        c = chars:sub(i, i)
        gs[c] = { file, x * sz, y * sz, sz, sz }
        x = x + 1
        if x >= rowlen then
            x = 0
            y = y + 1
        end
    end
end
