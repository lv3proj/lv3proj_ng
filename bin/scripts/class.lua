
--original from https://love2d.org/wiki/32_lines_of_goodness


local function classtostring(class)
    return "class " .. class.__classname
end

local function objtostring(obj)
    return "instance of " .. tostring(obj.__class)
end


local mt_class = {}

function mt_class:extends(parent)
    print("extends()", parent)
   self.super = parent
   parent.__members__ = parent.__members__ or {}
   setmetatable(mt_class, {__index = parent} )
   return self
end

local function define(class, members)
    print("define()", class, members)
   class.__members__ = class.__members__ or {}
   for k, v in pairs(members) do
      class.__members__[k] = v
   end
   function class:new(...)
      local newvalue = { __class = class }
      for k, v in pairs(class.__members__) do
         newvalue[k] = v
      end
      setmetatable(newvalue, { __index = class, __tostring = objtostring } )
      if newvalue.super then
        local ctor = newvalue.super.__init
        if ctor then
            ctor(newvalue, ...)
        end
      end
      if newvalue.__init then
         newvalue:__init(...)
      end
      return newvalue
   end
end

rawset(_G, "class", function(name)
    local newclass = { __classname = name }
    print("Create class " .. name)
    rawset(_G, name, newclass)
    return setmetatable(newclass, { __index = mt_class, __call = define, __tostring = classtostring } )
end)



----------------------------------------------------------



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
    obj.super = oldcls
    obj.__classname = cls.__classname
end


rawset(_G, "newclass", newclass)
rawset(_G, "setclass", setclass)

