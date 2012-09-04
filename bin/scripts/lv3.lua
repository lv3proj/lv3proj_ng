
--dofile("testmap.lua")


local e = entity.new()
--e:setCircleCollider(150)
e:texture("test3.png"):position(400, 300)
e:setAABBCollider()
e:rotate(-90):rotate(90, 5, -1, true, true)

function e:update(dt)
    local x, y = self:getPosition()
    self:position(x + math.random(-2, 2), y + math.random(-2, 2))
end


--local s = sound.new("klaxonloop.ogg")
--local s = sound.new("alarm.ogg")
--s:play()
--sound.new("klaxonloop.ogg"):play(2)

--music.play("green.spc")


--f:createText("Hello world!", 30, 30, empty, 2)


--local ff = quad.new("insanity.png", 10):blend(BLEND_ADD)

--q:addChild(ff)




--[[local p = player.new()
p:position(400, 300)
p:texture("sprites/olaf1.anim")
camera.follow(p)]]

--local b = button.new(300, 150, "Blarg"):position(400, 400)