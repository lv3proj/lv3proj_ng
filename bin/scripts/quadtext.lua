
newclass("quadtext", quad)

function quadtext.new(font, layer)
    layer = layer or 30
    local q = quad.new("", layer)
    setclass(q, quadtext)
    q.lines = {}
    q.font = font
    q.textWidth = 0
    q.textHeight = 0
    return q
end

function quadtext:_writeRow(row, s, y)
    --print("_WriteRow()", s, y)
    local len = #s
    local x = 0
    local c, q
    local font = self.font
    for i = 1, len do
        c = s:sub(i, i)
        q = row[i]
        local tex, tx, ty, tw, th = font:getChar(c)
        if q then
            q:texture(tex, tx, ty, tw, th):alpha(1)
            --q:delete()
        --end
        else
            q = quad.new(tex, 1, tx, ty, tw, th):position(x, y)
            q:setPauseLevel(99) -- HACK: remove this! Only check pause level for root objects!
            row[i] = q
            self:addChild(q)
        end
        x = x + font:getCharSize(c)
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
    self.textWidth = 0
    local cs = self.font:getCharSize()
    for _, line in ipairs(a) do
        self.textWidth = math.max(self.textWidth, #line * cs)
        local row = self.lines[i]
        if not row then
            row = {}
            self.lines[i] = row
        end
        self:_writeRow(row, line, (i - 1) * cs )
        i = i + 1
    end
    self.textHeight = #a * cs
    for i = #a+1, #self.lines do
        self:_writeRow(self.lines[i], "", (i - 1) * cs )
    end
end

