
local dbgstring =
      "Rendered Objs:  %u, Verts: %u\n"
   .. "Video mem free: %u KB\n"
   .. "Cam:           (%.3f, %.3f)\n"
   .. "Zoom:          (%.3f, %.3f)\n"
   .. "Recursion:      %d\n"
   .. "ScreenCenter:  (%.3f, %.3f)\n"
   .. "--- Memory --------------------\n"
   .. "Lua:            %u KB\n"

   .. "Resource:       %u KB, amount: %u\n"
   .. "Chunk:          %u KB (%u)\n"
   .. "> ObsGrid:      %u KB (%u)\n"
   .. "> Vector:       %u KB (%u)\n"
   .. "> Quad:         %u KB (%u)\n"
   .. "> Entity:       %u KB (%u)\n"
   .. "Fallback:       %u KB (%u)\n"
   .. "FallbackDelta: %5d, %5d B\n"
   .. "              (%5d, %5d)\n"


local function updateDebugText(obj)
    local zx, zy = camera.getScale()
    local cx, cy = camera.getPosition()
    local rec = getRecursionDepth()
    local scx, scy = getScreenCenter()
    local chunkKB, chunkC = stats.getChunkMem()
    local ogKB, ogC = stats.getObsGridMem()
    local vKB, vC = stats.getVectorMem()
    local qKB, qC = stats.getQuadMem()
    local eKB, eC = stats.getEntityMem()
    local fbKB, fbC = stats.getFallbackMem()
    local fbBa, fbCa, fbBf, fbCf = stats.getFallbackMemDelta()
    local s = string.format(dbgstring,
        stats.getRenderedObjects(), stats.getRenderedVertices(),
        stats.getFreeVideoMemory(),
        cx, cy,
        zx, zy,
        rec,
        scx, scy,
        
        collectgarbage("count"),
        stats.getResourceMem(), stats.getResourceCount(),
        chunkKB, chunkC,
        ogKB, ogC,
        vKB, vC,
        qKB, qC,
        eKB, eC,
        fbKB, fbC,
        fbBa, -fbBf, fbCa, -fbCf
    )
    obj:setText(s)
end

local debugtext = entity.new(31)
debugtext:makeInternal()
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
    
    local k = isKey(KEY_BACKQUOTE)
    if k and not keyDown then
        if self:getAlpha() > 0.5 then
            self:alpha(0, 0.1)
        else
            self:alpha(1, 0.1)
        end
    end
    keyDown = k
end

