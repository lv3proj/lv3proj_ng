
-- camera state
local beforeEditorScaleX = 1
local beforeEditorScaleY = 1
local beforeEditorPosX = 0
local beforeEditorPosY = 0
local editorZoom = 1
local editorPosX = 0
local editorPosY = 0

local editor = entity.new(30)
editor:setPauseLevel(PAUSELEVEL_EDITOR)
editor.on = false
editor.wasTab = false
editor.needinit = true
editor.ui = {}


function editor:setOn(on)
    self.on = on
    if on then
        self:onEnable()
    else
        self:onDisable()
    end
end

function editor:init()

    self.ui.bar = button.new(700, 20, "--[ Test ]--"):position(400, 15)
    self:addChild(self.ui.bar)
    
    local dd = dropdown.new():position(-200, 20):alpha(0)
    self.ui.bar:addChild(dd)
    self.ui.bar.onAction = function()
        if dd:isOpen() then
            dd:close()
        else
            dd:open()
        end
    end
    
    local bLoad = button.new(250, 20, "Load")
    local bSave = button.new(250, 20, "Save")
    
    dd:add(bLoad)
    dd:add(bSave)
    
    local ts = tileset.new("ship/tileset.png")
    ts:position(400, 300)
    self:addChild(ts)
end


function editor:onEnable()
    if self.needinit then
        self.needinit = false
        self:init()
    end
    
    print("Editor on")
    self:alpha(1)
    setPause(PAUSELEVEL_EDITOR)
    beforeEditorPosX, beforeEditorPosY = camera.getPosition()
    beforeEditorScaleX, beforeEditorScaleY = camera.getScale()
    
    camera.position(editorPosX, editorPosY)
    camera.scale(editorZoom, editorZoom)
end

function editor:onDisable()
    print("Editor off")
    self:alpha(0)
    setPause(0)
    
    editorPosX, editorPosY = camera.getPosition()
    editorZoom = camera.getScale()
    
    camera.position(beforeEditorPosX, beforeEditorPosY)
    camera.scale(beforeEditorScaleX, beforeEditorScaleY)
end

function editor:updateCamera()
    local z = camera.getScale()
    
    local mx, my = getMouseWindowPos()

    local wr = getMouseWheelRel()
    if wr ~= 0 then
        local oldz = z
        wr = 1 + (wr * 0.2)
        z = z * wr
        if z < 0.01 then z = 0.01 end
        --print("Zoom: " .. z)
        camera.scale(z, z)
        
        local cx, cy = camera.getPosition()
        cx = cx + mx/oldz - mx/z
        cy = cy + my/oldz - my/z
        camera.position(cx, cy)
    end
    
    if isMouseButton(2) then
        local rx, ry = getMouseWindowRel()
        if rx ~= 0 or ry ~= 0 then
            local cx, cy = camera.getPosition()
            local iz = 2 / z
            
            local rx, ry = getMouseWindowRel()
            camera.position(cx + (rx * iz), cy + (ry * iz))
            
            mx, my = getMouseWindowPos()
            setMousePos(mx - rx, my - ry)
        end
    end
end

function editor:update(dt)
    local tab = isKey(KEY_TAB)
    if not self.wasTab and tab then
        self:setOn(not self.on)
    end
    self.wasTab = tab
    
    if not self.on then
        return
    end
    
    local vw, vh = getVirtualOffs()
    self.ui.bar:position(400, 15-vh)
    
    editor:updateCamera()
    editor:updateSelection()

end


function editor:updateSelection()

end
