
local TILES =
{
    ["#"] = "ship/block1.png",
    ["+"] = "sprites/greyblock.png",
    ["<"] = "sprites/outlet1.png",
    [">"] = "sprites/outlet2.png",
    ["^"] = "sprites/outlet3.png",
    ["v"] = "sprites/outlet4.png",
}


local LAYERS = {}

LAYERS[4] =
{
    "########################################",
    "########################################",
    "########################################",
    "#######                    +      ######",
    "#######                    +      ######",
    "#######                    +      ######",
    "######################++################",
    "###############^######++################",
    "######                      ############",
    "######                      ############",
    "######  #######v########<   >###########",
    "######  #################   ############",
    "###########          ####   ############",
    "###########                 ############",
    "#######         ^           ############",
    "#######                     ############",
    "#####    ###              ##############",
    "#####    ###              ##############",
    "############++##v#######################",
    "############++##########################",
    "############                      ######",
    "############                      ######",
    "##############                    ######",
    "###############                   ######",
    "################                  ######",
    "#################                 ######",
    "##################                ######",
    "###################               ######",
    "####################              ######",
    "#####################             ######",
    "######################            ######",
    "########################################",
    "########################################",
    "########################################",
    "########################################",
}



setTileGridSize(4, 64)
setTileGridCollision(4, true)

for id, lr in pairs(LAYERS) do
    for y, line in pairs(lr) do
        for x = 1, #line do
            local tile = line:sub(x, x)
            local file = TILES[tile]
            if tile and file then
                setTile(id, x-1, y-1, file)
            end
        end
    end
end

initObsGrid(64, 16)


--return TILES, LAYERS

