-- currently hardcoded for xbox 360 controller

local snespad = {}
rawset(_G, "snespad", snespad)

snespad.__index = snespad

local INSTANCES = 0

function snespad.new()
    local self = setmetatable({ DX=0, DY=0, Start=false, Select=false, A=false, B=false, X=false, Y=false, L=false, R=false }, snespad)
    self.debugtext = quadtext.new(FONTS.default)
        :position(640, 540 + INSTANCES * 10)
        :color(1, 0, 0)
    INSTANCES = INSTANCES + 1
    return self
end

local buttonmap =
{
    [1] = "A",
    [0] = "B",
    [3] = "X",
    [2] = "Y",
    [4] = "L",
    [5] = "R",
    [7] = "Start",
    [6] = "Select",
}

function snespad:rawInput(evt, id, val)
    if evt == 7 then -- axis change
        if id == 0 then
            self:_rawAxis(val, "Left", -17000, -15000, "Right", 17000, 15000)
        elseif id == 1 then
            self:_rawAxis(val, "Up", -17000, -15000, "Down", 17000, 15000)
        end
    elseif evt == 10 or evt == 11 then -- button press/release
        local pressed = (evt == 10)
        local b = buttonmap[id]
        if b then
            self[b] = pressed
        end
        self:pushEvent(b, pressed)
    elseif evt == 9 then -- hat change
        -- WRITE ME
    end
end

function snespad:_rawAxis(val, Lname, Lv1, Lv2, Rname, Rv1, Rv2)
    if val < Lv1 and not self[Lname] then
        self[Lname] = true
        self:pushEvent(Lname, true)
    elseif val > Lv2 and self[Lname] then
        self[Lname] = false
        self:pushEvent(Lname, false)
    end
    if val > Rv1 and not self[Rname] then
        self[Rname] = true
        self:pushEvent(Rname, true)
    elseif val < Rv2 and self[Rname] then
        self[Rname] = false
        self:pushEvent(Rname, false)
    end
end

local function b(x, y)
    if x then return y end
    return "-"
end

function snespad:pushEvent(btn, value)
    local s = string.format("[%s%s%s%s] [%s%s] [%s%s%s%s%s%s]",
        b(self.Up, "U"), b(self.Down, "D"), b(self.Left, "L"), b(self.Right, "R"), 
        b(self.Select, "s"), b(self.Start, "S"),
        b(self.A, "A"), b(self.B, "B"), b(self.X, "X"), b(self.Y, "Y"), b(self.L, "L"), b(self.R, "R")
    )
    self.debugtext:setText(s)
end
