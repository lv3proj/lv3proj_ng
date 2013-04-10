
local evtab = {}

rawset(_G, "triggerEvent", function(name, ...)
    if evtab[name] then
        for f, _ in pairs(evtab[name]) do
            f(...)
        end
    end
end)

rawset(_G, "addEventListener", function(name, f)
    local t = evtab[name]
    if not t then
        t = {}
        evtab[name] = t
    end
    t[f] = true
end)

rawset(_G, "removeEventListener", function(name, f)
    local t = evtab[name]
    if t then
        t[f] = nil
    end
end)

