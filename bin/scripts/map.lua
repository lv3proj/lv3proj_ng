
local currentMap
local currentMapParams

rawset(_G, "clearMap", function()
    for i = 0, getMaxLayerID() do
        setTileGridCollision(i, false)
        clearTiles(i)
        setLayerVisible(i, true)
    end
    initObsGrid(0, 0)
    clearEntities()
    clearQuads()
    clearGarbage()
    collectgarbage()
end)


rawset(_G, "loadMap", function(...)
    local name = select(1, ...) -- first param is map name without path or extension
    assert(type(name) == "string")
    
    local file = "maps/" .. name .. ".lua"
    local f, err = loadfile(file)
    if not f then
        msgbox("Error loading map script [" .. file .. "]: " .. err)
        return
    end
    currentMap = name
    currentMapParams = { select(2, ...) }
    clearMap()
    return f(select(2, ...)) -- all other params are passed to the map file function
end)

rawset(_G, "saveMap", function(file)
    assert(currentMap, "No map loaded")
    
    msgbox("NYI")
end)

rawset(_G, "reloadMap", function(file)
    return loadMap(currentMap, table.unpack(currentMapParams))
end)

rawset(_G, "getMapName", function(withParams)
    local s = currentMap
    if withParams then
        s = s .. " " .. table.concat(currentMapParams, " ")
    end
    return s
end)

