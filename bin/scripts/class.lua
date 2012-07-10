
local function makeclass(base)
    local cls = {
        __index = 0,
        __classname = "UNDEFINED",
    }
    cls.__index = cls
    setmetatable(cls, { __index = base } )
    return cls
end

local function newclass(name, base)
    local cls = makeclass(base)
    rawset(_G, name, cls)
    cls.__classname = name
    return cls
end

local function setclass(obj, cls)
    local members = getmetatable(obj)
    local oldcls = getmetatable(members)
    setmetatable(members, cls)
    obj.__classname = cls.__classname
end


rawset(_G, "newclass", newclass)
rawset(_G, "setclass", setclass)

