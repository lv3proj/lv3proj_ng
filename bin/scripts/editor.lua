
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

    self.ui.bar = button.new(700, 20, "--[ Menu ]--"):position(400, 15)
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

function editor:updateLayerText()
    local s = ""
    for i = 1, 31 do
        if isLayerVisible(i) then
            if i >= 10 then
                s = s .. " " .. i
            else 
                s = s .. "  " .. i
            end
        else
            s = s .. "  -"
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
    print("Select layer " .. layer)
end

function editor:toggleLayerVisible(layer)
    local vis = isLayerVisible(layer)
    if vis then
        print("Hide layer " .. layer)
        setLayerVisible(layer, false)
    else
        print("Show layer " .. layer)
        setLayerVisible(layer, true)
    end
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
    
    if key >= KEY_1 and key <= KEY_9 then
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
    
    if key == KEY_LALT or key == KEY_RALT then
        self.ui.layertext:alpha(1, 0.2)
    end
end

function editor:onKeyUp(key, mod)
    if key == KEY_LALT or key == KEY_RALT then
        self.ui.layertext:alpha(0, 0.4)
    end
end

