-- timerqueue module
-- for delayed function calls


local M = {}
M.__index = M

function M:push(delay, func, ...)
    assert(type(delay) == "number", "TQ: param 1: expected number, got " .. type(delay))
    assert(type(func) == "function", "TQ: param 2: expected function, got " .. type(func))
    
    local targ = self[self._tq_lock+1]
    if not targ then
        targ = {}
        self[self._tq_lock+1] = targ
    end
    if select("#", ...) > 0 then
        table.insert(targ, {f = func, t = delay, param = {...} } )
    else
        table.insert(targ, {f = func, t = delay } )
    end
end

-- compact tq[1...X] into tq[1]
function M:_compact()
    if self._tq_lock == 0 and #self > 1 then
        local targ = self[1] -- must exist
        for qi, tq in pairs(self) do
            if type(qi) == "number" and qi > 1 then -- skip tq[1]
                for _,e in pairs(tq) do
                    table.insert(targ, e)
                end
                self[qi] = nil
            end
        end
    end
end


-- re-entrant update function
-- that means calling wait()/watch() in a TQ callback is allowed,
-- and calling pushTQ() from a callback is no problem either.
-- (taking care of not adding entries to any table that is currently iterated over)
-- Lua docs say removing elems while iterating is okay.
function M:update(dt)
    if not self[1] then return end
    
    self._tq_lock = self._tq_lock + 1
    
    for qi, tq in pairs(self) do
        if type(qi) == "number" then
            for i,e in pairs(tq) do
                if e.t < dt then
                    tq[i] = nil
                    if e.param then
                        e.f(table.unpack(e.param))
                    else
                        e.f()
                    end
                else
                    e.t = e.t - dt
                end
            end
        end
    end 
    
    self._tq_lock = self._tq_lock - 1
    
    self:_compact()
end

function M:isEmpty()
    return #self == 1 and next(self[1]) == nil
end

local function tq_create()
    return setmetatable({ _tq_lock = 0 }, M)
end

rawset(_G, "tq_create", tq_create)


