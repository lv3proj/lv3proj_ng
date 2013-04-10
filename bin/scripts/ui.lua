

local UIELEMS = {}

newclass("uibase", quad)


function uibase.new(layer)
    layer = layer or 30 -- FIXME
    local q = quad.new("", layer)
    setclass(q, uibase)
    
    UIELEMS[q] = q
    
    q:setPauseLevel(99)
    q.zorder = 0
    q.selectable = false
    
    return q
end

function uibase:onClick()
    print("uibase: onClick()")
end

function uibase:onAction()
    print("uibase: onAction()")
end

function uibase:onFocus()
end

function uibase:onFocusLost()
end

function uibase:onEndOfLife()
    UIELEMS[self] = nil
end



newclass("button", uibase)

function button.new(w, h, str)
    local b = uibase.new()
    setclass(b, button)
    b.selectable = true
    b:color2(0.5, 0.5, 0.5)
    b:texture("white.png")
    b:setWH(w, h)
    b:alpha(0.75)
    str = str or ""
    local text = quadtext.new(FONTS.default, 0)
    b.text = text
    b:addChild(text)
    b.centerText = false
    b:setText(str)
    return b
end

function button:onClick()
    self:color2(1, 1, 1)
    self:color2(0.8, 0.8, 0.8, 0.2)
    self:onAction()
end

function button:setText(str)
    self.text:setText(str)
    if self.centerText then
        self.text:position(self.text.textWidth * -0.5, 0)
    else
        local w, h = self:getWH()
        self.text:position(w * -0.5 + self.text.font:getCharSize(), 0)
    end
end

function button:onFocus()
    self:color2(0.8, 0.8, 0.8, 0.2)
end

function button:onFocusLost()
    self:color2(0.5, 0.5, 0.5, 0.2)
end


newclass("dropdown", uibase)

function dropdown.new()
    local d = uibase.new()
    d.ins = 0
    setclass(d, dropdown)
    return d
end

function dropdown:close()
    self:alpha(0)
end

function dropdown:open()
    self:alpha(1)
end

function dropdown:isOpen()
    return self:getAlpha() > 0.5
end

function dropdown:add(obj)
    self:addChild(obj)
    local w, h = obj:getWH()
    obj:position(0, self.ins)
    self.ins = self.ins + h
    
    -- monkey-patch callback to auto-close the dropdown menu
    -- if the associated object's action callback is triggered
    local a = obj.onAction
    if a then
        obj.onAction = function()
            a()
            self:close()
        end
    end
end


local function isInFocus(e, mx, my)
    local w, h = e:getWH()
    local x, y = e:getAbsolutePosition()
    w = w / 2
    h = h / 2
    
    return mx >= x-w and mx <= x+w and my >= y-h and my <= y+h
end

rawset(_G, "UI", {})

local wasLMB = false
local cursel = false

local function updateSelection(e)
    local lmb = isLeftMouse()
    if e and lmb and not wasLMB then
        e:onClick()
    end
    
    if cursel ~= e then
        if e then
            e:onFocus()
        end
        if cursel then
            cursel:onFocusLost()
        end
        cursel = e
    end
end
        

function UI.update(dt)
    local topz = -99999
    local top
    local mx, my = getMouseWindowPos()
    for _, e in pairs(UIELEMS) do
        if e.selectable and e.zorder > topz and e:isVisible() and isInFocus(e, mx, my) then
            top = e
            topz = e.zorder
        end
    end
    
    local lmb = isLeftMouse()
    updateSelection(top)
    wasLMB = lmb
end

