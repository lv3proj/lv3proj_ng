
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
    self.ui.test = button.new(200, 100, "Test"):position(200, 200)
    self:addChild(self.ui.test)
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

    local wr = getMouseWheelRel()
    if wr ~= 0 then
        local oldz = z
        wr = 1 + (wr * 0.1)
        z = z * wr
        if z < 0.01 then z = 0.01 end
        --print("Zoom: " .. z)
        camera.scale(z, z)
        
        local mx, my = getMouseWindowPos()
        local cx, cy = camera.getPosition()
        cx = cx + mx/oldz - mx/z
        cy = cy + my/oldz - my/z
        camera.position(cx, cy)
    end
    
    if isMouseButton(2) then
        local rx, ry = getMouseWindowRel()
        local cx, cy = camera.getPosition()
        local iz = 1 / z
        camera.position(cx - (iz * rx), cy - (iz * ry))
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
    
    editor:updateCamera()
    editor:updateSelection()
end


function editor:updateSelection()

end
