
-- supplemental camera functions

local DEFAULT_LERP_TIME = 0.1
local DEFAULT_ZOOM_TIME = 0.5
local DEFAULT_ZOOM = 1

camera._followRO = false
camera._lerpT = DEFAULT_LERP_TIME
camera._zoomT = DEFAULT_ZOOM_TIME
camera._zoom = DEFAULT_ZOOM
camera._shakeMag = 0
camera._shakeTime = 0

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

function camera.shake(mag, tm)
    camera._shakeMag = mag
    camera._shakeTime = tm
end

function camera.update(dt)
    if isPause(0) then
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
    
    if camera._shakeTime > 0 then
        camera._shakeTime = camera._shakeTime - dt
        local t = camera._shakeMag / 2
        camera.offset(math.random(-t, t), math.random(-t, t))
    else
        camera.offset(0, 0)
    end
        
    
end
