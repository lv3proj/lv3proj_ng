
rawset(_G, "quadtext", {} )
quadtext.__index = quadtext
setmetatable(quadtext, { __index = quad } )

function quadtext.new(font, layer)
    layer = layer or 30
    local q = quad.new("", layer)
    q.lines = {}
    q.font = font
    local members = getmetatable(q)
    setmetatable(members, quadtext) -- we're now a class member of quadtext
    return q
end

function quadtext:_writeRow(row, s, y)
    --print("_WriteRow()", s, y)
    local len = s:len()
    local x = 0
    local c, q
    
    for i = 1, len do
        c = s:sub(i, i)
        q = row[i]
        local tex = self.font:getChar(c)
        if q then
            q:texture(tex):alpha(1)
        else
            q = quad.new(tex, 1):position(x, y)
            row[i] = q
        end
        x = x + self.font:getCharSize()
        self:addChild(q)
    end
    
    len = len + 1
    while row[len] do
        row[len]:alpha(0)
        len = len + 1
    end
end

function quadtext:setText(s)
    s = s:upper() -- HACK FIXME
    local a = s:explode("\n")
    local i = 1
    for _, line in ipairs(a) do
        local row = self.lines[i]
        if not row then
            row = {}
            self.lines[i] = row
        end
        self:_writeRow(row, line, (i - 1) * self.font:getCharSize() )
        i = i + 1
    end
end

