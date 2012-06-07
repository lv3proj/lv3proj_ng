
function table.shuffle(tab)
    local c = #tab
    for i = c, 2, -1 do
        local j = math.random(1, i)
        tab[i], tab[j] = tab[j], tab[i]
    end
    return tab
end
