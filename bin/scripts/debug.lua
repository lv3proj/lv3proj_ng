

local rawset = rawset

local function looksLikeGlobal(s)
    return not s:match("[^_%u%d]")
end

local function handleSetGlobal(tab, k, v)
    if not looksLikeGlobal(k) then
        print(debug.traceback("WARNING: Set global variable '" .. k .. "' = '" .. tostring(v) .. "'", 2))
    end
    rawset(_G, k, v)
end

local function handleUndefGlobal(tab, k)
    print(debug.traceback("WARNING: Tried to read undef global variable '" .. k .. "'", 2))
end

local meta =
{
    __newindex = handleSetGlobal,
    __index = handleUndefGlobal
}

setmetatable(_G, meta)



--[[
setmetatable(_OBJECTREGISTRY, {
    __newindex = function(tab, k, v)
        print("New object: [" .. tostring(k) .. "] = " .. tostring(v))
        rawset(tab, k, v)
    end
})
]]

