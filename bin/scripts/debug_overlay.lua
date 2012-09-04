
local dbgstring =
              "Lua mem:       %u KB\n"
           .. "ObsGrid mem:   %u KB\n"
           .. "Resource mem:  %u KB, amount: %u\n"
           .. "Rendered Objs: %u, Verts: %u\n"
           .. "Video mem free:%u KB\n"
           .. "Cam:  (%.3f, %.3f)\n"
           .. "Zoom: (%.3f, %.3f)\n"
           .. "Recursion: %d\n"
           .. "ScreenCenter: (%.3f, %.3f)\n"
    
local function updateDebugText(obj)
    local zx, zy = camera.getScale()
    local cx, cy = camera.getPosition()
    local rec = getRecursionDepth()
    local scx, scy = getScreenCenter()
    local s = string.format(dbgstring,
        collectgarbage("count"),
        stats.getObsGridMem(),
        stats.getResourceMem(), stats.getResourceCount(),
        stats.getRenderedObjects(), stats.getRenderedVertices(),
        stats.getFreeVideoMemory(),
        cx, cy,
        zx, zy,
        rec,
        scx, scy
    )
    obj:setText(s)
end

local debugtext = entity.new(31)
debugtext:setPauseLevel(99)
debugtext.text = quadtext.new(FONTS.default, 0)
debugtext:addChild(debugtext.text)
debugtext:alpha(0)

local dbgTextT = 0
local keyDown = false

function debugtext:update(dt)
    if dbgTextT >= 0 and self:getAlpha() > 0.1 then
        dbgTextT = dbgTextT - dt
        if dbgTextT <= 0 then
            local ox, oy = getVirtualOffs()
            self:position(10 - ox, 10 - oy) -- FIXME
            updateDebugText(self.text)
            dbgTextT = 0.2
        end
    end
    
    local k = isKey(KEY_F1)
    if k and not keyDown then
        if self:getAlpha() > 0.5 then
            self:alpha(0, 0.1)
        else
            self:alpha(1, 0.1)
        end
    end
    keyDown = k
end

