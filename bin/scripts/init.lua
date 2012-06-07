
print("----- Lua init... -----")

dofile("debug.lua")
dofile("ro.lua")
dofile("quad.lua")

-- forbid os functions, these are dangerous.
os = nil
package = nil
--module = nil
require = nil

--[[
local q = quad.new()
print(q)
print(q:scale(2, 2))
q:delete(0.2)

for i = 1, 9999 do
    quad.new():delete(0.5)
end

q.derp = 42
print(q.derp)

-- testing safety measures
a = 0
local b = c
]]


print("----- Lua init done! -----")


function onInit()
    local q = quad.new("test.png", 1):position(400, 300):scale(2, 2, 1, -1, true, true):rotate(-30):rotate(30, 2, -1, true, true)
    --q:alpha(0, 3):delete(0.3) -- FIXME
end


function onUpdate(dt)
    --print("onUpdate(" .. dt .. ")")
end

