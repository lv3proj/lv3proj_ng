

local inh = 
{
    __index = function(tab, k)
        print("RO method lookup failed: " .. tostring(k))
    end
}

setmetatable(ro, inh)

