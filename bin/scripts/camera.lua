
-- supplemental camera functions

local DEFAULT_LERP_TIME = 0.1
local DEFAULT_ZOOM_TIME = 0.5
local DEFAULT_ZOOM = 1

camera._followRO = false
camera._lerpT = DEFAULT_LERP_TIME
camera._zoomT = DEFAULT_ZOOM_TIME
camera._zoom = DEFAULT_ZOOM

function camera.follow(ro)
    camera._followRO = ro or false
end

function camera.setLerpTime(t)
    camera._lerpT = t or 0.1
end

function camera.zoom(z)
    camera._zoom = z or false
end

function camera.setZoomTime(t)
    camera._zoomT = t or 0.5
end

function camera.update(dt)
    if isPause(1) then
        return
    end
    
    local z = camera._zoom
    if z then
        camera.scale(z, z, camera._zoomT)
    end
    
    local ro = camera._followRO
    if ro then
        local x, y = ro:getPosition()
        local sx, sy = camera.getScale()

        -- position correction
        local cx = x - (400 / sx)
        local cy = y - (300 / sy)
        
        -- TODO: correct the slight offset when zooming fast
        
        camera.position(cx, cy, camera._lerpT)
    end
    
end
