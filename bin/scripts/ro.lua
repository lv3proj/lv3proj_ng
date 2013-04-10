
--[[
local inh = 
{
    __index = function(tab, k)
        print("RenderObject method lookup failed: " .. tostring(k))
    end
}

setmetatable(ro, inh)
]]

ro.__classname = "ro"


function ro:makeInternal()
    self["._internal"] = true
end

function ro:isInternal()
    return self["._internal"] or false
end

