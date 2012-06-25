
class "pixfont" : extends(font)
{
}

function pixfont:__init()
    print("pixfont:__init()")
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
        fn = string.format("%s:%d:%d:%d:%d", file, x * sz, y * sz, sz, sz)
        gs[c] = fn
        print(string.format('(%d, %d) gs["%s"] = "%s"', x, y, c, fn))
        x = x + 1
        if x >= rowlen then
            x = 0
            y = y + 1
        end
    end
end
