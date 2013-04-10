
local map = select(1, ...)

local tilesize = 16
local tilesPerRow = 16

if not map then
    msgbox("Missing tileset/data name, use \"import <name>\" as map name")
    return
end

local path = "vikings_tilemaps/" .. map .. ".txt"
local f = io.open(path)
if not f then
    msgbox("Unable to open tilemap file [" .. path .. "]")
    return
end

local hdr = f:read()
local w, h = table.unpack(hdr:explode(" ", true))
local data = {}
for line in f:lines() do
    for _, n in ipairs(line:explode(" ", true)) do
        table.insert(data, tonumber(n))
    end
end
f:close()
assert(#data >= w * h, "Not enough data")

setTileGridSize(3, math.max(w, h))
setTileGridSize(4, math.max(w, h))
setTileGridCollision(4, true)
initObsGrid(math.max(w, h), tilesize)

local bits = setmetatable({}, {__index = {}})

local didx = 0
for y = 0, h-1 do
    bits[y] = {}
    for x = 0, w-1 do
        didx = didx + 1
        local num = data[didx]
        
        bits[y][x] = bit32.rshift(num, 10)
        
        local layer = 3
        if num > 0x3ff then
            num = bit32.band(num, 0x3ff)
            layer = 4
        end
        
        local col = math.floor(num / tilesPerRow)
        local row = num % tilesPerRow
        
        local tile = string.format("vikings_tilesets/%s.png:%d:%d:%d:%d", map, row*tilesize, col*tilesize, tilesize, tilesize)
        
        setTile(layer, x, y, tile)

    end
end

local bittrans =
{
    --[[0x0400]] [0] = "Solid",
    --[[0x0800]] [1] = "Climb",
    --[[0x1000]] [2] = "Block enemies/Walk on top",
    --[[0x2000]] [3] = "No see through?",
    --[[0x4000]] [4] = "Slope/Instakill",
    --[[0x8000]] [5] = "Slope/Bombable",
}


local e = entity.new(29)
DEBUG_MOUSEOVER = e
e:setPauseLevel(99)

e.text = quadtext.new(FONTS.default, 0)
e:addChild(e.text)
e.lastx, e.lasty = -1, -1

function e:update(dt)
    local mx, my = getMouseWorldPos()
    local tx, ty = math.floor(mx / tilesize), math.floor(my / tilesize)
    self:position(mx, my)
    if tx == self.lastx and ty == self.lasty then
        return
    end
    self.lastx = tx
    self.lasty = ty
    
    local b = bits[ty][tx] or 0
    local desc = ""
    local bitstr = ""
    if b ~= 0 then
        local t = {}
        for i = 0, 5 do
            if bit32.btest(b, bit32.lshift(1, i)) then
                table.insert(t, string.format("[%X] = %s", bit32.lshift(0x400, i), bittrans[i] or "?"))
                bitstr = "1" .. bitstr
            else
                bitstr = "0" .. bitstr
            end
        end
        desc = table.concat(t, "\n")
    end
    self.text:setText("\n\n. (" .. tx .. "," .. ty .. ") = " .. b .. " [" .. bitstr .. "]\n" .. desc)
end
