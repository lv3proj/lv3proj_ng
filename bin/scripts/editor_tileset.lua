
newclass("tileset", uibase)

function tileset.new(img)
    local q = uibase.new(31):texture(img)
    setclass(q, tileset)
    return q
end

