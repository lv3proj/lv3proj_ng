
local map = select(1, ...)

local tilesize = 16
local tilesPerRow = 16

if not map then
    msgbox("Missing tileset/data name, use \"import <name>\" as map name")
    return
end

local path = "data/lv1_tilemaps/" .. map .. ".txt"
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


local layerdata = {}
local c = 0
local path = "data/lv1_tilemaps/" .. map .. "_layers.txt"
local f = io.open(path)
if f then
    for line in f:lines() do
        local a = line:explode(" ", true)
        layerdata[c] = tonumber(a[1]) -- skip the rest
        c = c + 1
    end
    f:close()
end


local LAYER_DEBUG = 7        -- markers
local LAYER_DESTRUCTIBLE = 6 -- solid foreground, destructible
local LAYER_WALLOVERLAY = 5
local LAYER_WALL = 4         -- solid foreground
local LAYER_NOWALL = 3       -- foreground, not solid
local LAYER_BG = 2            -- background, not solid
local LAYER_SLOPES = 1        -- solid background
local LAYERS = { LAYER_DESTRUCTIBLE, LAYER_WALL, LAYER_NOWALL, LAYER_BG, LAYER_SLOPES, LAYER_DEBUG, LAYER_WALLOVERLAY }
for _, i in pairs(LAYERS) do
    setTileGridSize(i, math.max(w, h)) 
end

local bits = setmetatable({}, {__index = {}})

----------------
local BITS_SLOPE = bit32.bor(0x8000, 0x4000)
local BITS_SOLID = 0x400
local BITS_DESTRUCTIBLE_TRIGGER = 0x800
local BITS_DESTRUCTIBLE_TRIGGER2 = bit32.bor(0x400, 0x1000)
local BITS_DESTRUCTIBLE = 0x8000
local BITTAB_SLOPE =
{
    [bit32.bor(BITS_SLOPE, 0x000        )] = 0,
    [bit32.bor(BITS_SLOPE, 0x800        )] = 1,
    [bit32.bor(BITS_SLOPE, 0x400, 0x800 )] = 2,
    [bit32.bor(BITS_SLOPE, 0x400        )] = 3,
    [bit32.bor(BITS_SLOPE, 0x400, 0x1000)] = 4,
    [bit32.bor(BITS_SLOPE, 0x1000       )] = 5,
}
----------------

local tiles = {}

local function gettile(num, tex, ...)
    local t = tiles[num]
    if not t then
        t = createTile(tex, ...)
        tiles[num] = t
    end
    return t
end

local marker16 = createTile("debug/marker16.png")

local didx = 0
for y = 0, h-1 do
    bits[y] = {}
    for x = 0, w-1 do
        didx = didx + 1
        local num = data[didx]
        bits[y][x] = bit32.rshift(num, 10)
        local bit = bit32.band(num, bit32.bnot(0x3ff))
        num = bit32.band(num, 0x3ff)
        
        local layer = LAYER_BG

        
        if bit ~= 0 and bit ~= BITS_SOLID and not BITTAB_SLOPE[bit] then
            -- FIXME
            setTile(LAYER_DEBUG, x, y, marker16)
        end
        
        if bit == BITS_SOLID then -- solid
            layer = LAYER_WALL
        elseif bit == BITS_DESTRUCTIBLE or bit == BITS_DESTRUCTIBLE_TRIGGER or bit == BITS_DESTRUCTIBLE_TRIGGER2 then
            layer = LAYER_DESTRUCTIBLE
        elseif BITTAB_SLOPE[bit] then
            -- FIXME
            local slope = gettile("slope" .. bit, "lv1_tilesets/slopes.png", BITTAB_SLOPE[bit]*tilesize, 0, tilesize, tilesize)
            --local slope = string.format("lv1_tilesets/slopes.png:%d:0:%d:%d", BITTAB_SLOPE[bit]*tilesize, tilesize, tilesize)
            setTile(LAYER_SLOPES, x, y, slope)
        end
        
        if layerdata[num] ~= 0 then
            if bit == BITS_SOLID then
                layer = LAYER_WALL
            elseif bit == BITS_DESTRUCTIBLE or bit == BITS_DESTRUCTIBLE_TRIGGER or bit == BITS_DESTRUCTIBLE_TRIGGER2 then
                layer = LAYER_DESTRUCTIBLE
            elseif bit == 0 then
                layer = LAYER_WALLOVERLAY
            end
        end
        
        local col = math.floor(num / tilesPerRow)
        local row = num % tilesPerRow
        local tex = "lv1_tilesets/" .. map .. ".png"
        --row*tilesize, col*tilesize, tilesize, tilesize
        local tileidx = gettile(num, tex, row*tilesize, col*tilesize, tilesize, tilesize)
        setTile(layer, x, y, tileidx)
    end
end

setTileGridCollision(LAYER_DESTRUCTIBLE, true)
setTileGridCollision(LAYER_WALL, true)
setTileGridCollision(LAYER_SLOPES, true)
initObsGrid(math.max(w, h), tilesize)


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
    
    local bitsy = bits[ty]
    local b = (bitsy and bitsy[tx]) or 0
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
