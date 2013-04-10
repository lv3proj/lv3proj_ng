
-- camera state
local beforeEditorScaleX = 1
local beforeEditorScaleY = 1
local beforeEditorPosX = 0
local beforeEditorPosY = 0
local editorZoom = 1
local editorPosX = 0
local editorPosY = 0

local editor = entity.new(30)
editor:makeInternal()
rawset(_G, "EDITOR_ENTITY", editor)
editor:setPauseLevel(PAUSELEVEL_EDITOR)
editor.on = false
editor.wasTab = false
editor.needinit = true
editor.ui = {}
editor.selectedLayer = 1

addEventListener("onKeyDown", function(...) editor:onKeyDown(...) end)
addEventListener("onKeyUp", function(...) editor:onKeyUp(...) end)

rawset(_G, "isInEditor", function()
    return editor.on
end)


local function loadMapUser()
    -- getUserInputText() enters a wait(), which is problematic and would cause infinte recursion,
    -- so we delay the call to the toplevel update().
    TQ:push(0, function()
        local inp = getUserInputText() 
        if inp and inp ~= "" then
            return loadMap(table.unpack(inp:explode(" ", true)))
        end
    end)
end

function editor:setOn(on)
    self.on = on
    if on then
        self:onEnable()
    else
        self:onDisable()
    end
end

function editor:addChild(ch)
    ro.addChild(self, ch)
    ch:setPauseLevel(PAUSELEVEL_EDITOR)
end

function editor:init()

    self.ui.bar = button.new(700, 20, ""):position(400, 15)
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
    
    local bLoad = button.new(250, 20,     "Load map...         (Shift+F1)")
    bLoad.onAction = loadMapUser
    
    local bReload = button.new(250, 20,   "Reload map          (F1)")
    bReload.onAction = reloadMap
    
    local bSave = button.new(250, 20,     "Save map            (F2)")
    bSave.onAction = saveMap
    
    dd:add(bLoad)
    dd:add(bReload)
    dd:add(bSave)
    
    --local ts = tileset.new("ship/tileset.png")
    --ts:position(400, 300)
    --self:addChild(ts)
    
    self.ui.layertext = quadtext.new(FONTS.default, 0):position(10, 570):alpha(0)
    self:addChild(self.ui.layertext)
    self:updateLayerText()
    self:updateBarText()
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
    
    local mrx, mry = getMouseWindowRel()
    if mrx ~= 0 or mry ~= 0 then
        self:updateBarText()
    end

end

function editor:updateBarText()
    local mx, my = getMouseWorldPos()
    local s = string.format('[Menu]  L:%d  Map:\"%s\" (%d, %d)',
        self.selectedLayer, getMapName(true), mx, my)
    self.ui.bar:setText(s)
end

function editor:updateLayerText()
    local s = ""
    local a, z
    for i = 1, getMaxLayerID() do
        if i == self.selectedLayer then
            a = "["
            z = "]"
        else
            a = " "
            z = " "
        end
        if isLayerVisible(i) then
            if i >= 10 then
                s = s .. a .. i .. z
            else 
                s = s .. a .. " " .. i .. z
            end
        else
            s = s .. a .. " -" .. z
        end
        if (i % 10) == 0 then
            s = s .. "\n"
        end
    end
    self.ui.layertext:setText(s)
end

function editor:updateSelection()

end

function editor:selectLayer(layer)
    self.selectedLayer = layer
    self:updateLayerText()
    self:updateBarText()
end

function editor:toggleLayerVisible(layer)
    setLayerVisible(layer, not isLayerVisible(layer))
    self:updateLayerText()
end

function editor:onKeyDown(key, mod)
    if not self.on then
        return
    end
    if key == KEY_F1 then
        if bit32.btest(mod, KMOD_SHIFT) then
            loadMapUser()
        else
            reloadMap()
        end
    end
    
    local isLayerKey = (key >= KEY_1 and key <= KEY_9)
    
    if isLayerKey then
        local hide = false
        local num = key - KEY_1 + 1
        if bit32.btest(KMOD_ALT, mod) then
            hide = true
        end
        if bit32.btest(KMOD_SHIFT, mod) then
            num = num + 10
        elseif bit32.btest(KMOD_CTRL, mod) then
            num = num + 20
        end
        
        if hide then
            self:toggleLayerVisible(num)
        else
            self:selectLayer(num)
        end
    end
    
    if key == KEY_LALT or key == KEY_RALT or isLayerKey then
        self.ui.layertext:alpha(1, 0.05)
    end
end

function editor:onKeyUp(key, mod)
    if not self.on then
        return
    end
    
    local isLayerKey = (key >= KEY_1 and key <= KEY_9)
    
    if key == KEY_LALT or key == KEY_RALT or isLayerKey then
        self.ui.layertext:alpha(0, 1)
    end
end

