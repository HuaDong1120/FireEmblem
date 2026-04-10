-- Fire Emblem Terrain Tiles Generator
-- 32x32 pixels per tile, GBA style
-- Tiles: Plain, Forest, Mountain, Castle, Village, River, Sea, Desert, Fort

local TILE_W = 32
local TILE_H = 32
local COLS = 3
local ROWS = 3
local IMG_W = TILE_W * COLS
local IMG_H = TILE_H * ROWS

-- Create sprite
local spr = Sprite(IMG_W, IMG_H)
spr:setPalette(Palette())
app.activeSprite = spr

local img = spr.cels[1].image

-- ============================================================
-- Color Palette
-- ============================================================
local C = {
    -- Plain / Grass
    grass_light   = Color(112, 176, 64),
    grass_mid     = Color(80,  152, 48),
    grass_dark    = Color(56,  120, 32),
    grass_shadow  = Color(40,   96, 24),

    -- Forest
    tree_light    = Color(48,  128, 48),
    tree_mid      = Color(32,  100, 32),
    tree_dark     = Color(16,   72, 16),
    trunk         = Color(120,  80, 32),
    trunk_dark    = Color( 88,  56, 16),

    -- Mountain
    rock_light    = Color(200, 192, 176),
    rock_mid      = Color(160, 152, 136),
    rock_dark     = Color(112, 104,  88),
    rock_shadow   = Color( 72,  64,  56),
    snow          = Color(240, 240, 255),

    -- Castle
    wall_light    = Color(208, 200, 184),
    wall_mid      = Color(168, 160, 144),
    wall_dark     = Color(120, 112,  96),
    gate          = Color( 80,  56,  32),
    gate_dark     = Color( 48,  32,  16),
    flag_red      = Color(200,  48,  32),

    -- Village
    roof_red      = Color(192,  64,  48),
    roof_dark     = Color(136,  40,  24),
    house_wall    = Color(240, 224, 192),
    house_shadow  = Color(184, 168, 136),
    door          = Color( 96,  64,  32),
    chimney       = Color(128, 112,  96),

    -- River
    water_light   = Color( 96, 176, 232),
    water_mid     = Color( 64, 144, 208),
    water_dark    = Color( 40, 112, 176),
    water_foam    = Color(192, 224, 248),

    -- Sea
    sea_light     = Color( 64, 128, 200),
    sea_mid       = Color( 40,  96, 168),
    sea_dark      = Color( 24,  64, 136),
    sea_foam      = Color(160, 208, 240),

    -- Desert
    sand_light    = Color(240, 216, 144),
    sand_mid      = Color(208, 184, 112),
    sand_dark     = Color(168, 144,  80),
    sand_shadow   = Color(136, 112,  56),
    cactus        = Color( 64, 136,  48),

    -- Fort
    fort_light    = Color(176, 168, 152),
    fort_mid      = Color(136, 128, 112),
    fort_dark     = Color( 96,  88,  72),
    fort_gate     = Color( 56,  40,  24),

    black         = Color(  0,   0,   0),
}

-- ============================================================
-- Helper: set pixel
-- ============================================================
local function px(image, x, y, color)
    if x >= 0 and x < image.width and y >= 0 and y < image.height then
        image:drawPixel(x, y, color)
    end
end

-- ============================================================
-- Helper: fill rect
-- ============================================================
local function fillRect(image, x, y, w, h, color)
    for dy = 0, h - 1 do
        for dx = 0, w - 1 do
            px(image, x + dx, y + dy, color)
        end
    end
end

-- ============================================================
-- TILE 0,0 : Plain (草原)
-- ============================================================
local function drawPlain(img, ox, oy)
    for y = 0, TILE_H - 1 do
        for x = 0, TILE_W - 1 do
            local c
            if (x + y) % 4 == 0 then
                c = C.grass_light
            elseif (x * 3 + y * 2) % 7 == 0 then
                c = C.grass_dark
            else
                c = C.grass_mid
            end
            px(img, ox + x, oy + y, c)
        end
    end
    -- Grass tufts
    local tufts = {{4,6},{10,14},{18,8},{24,20},{8,24},{20,4},{28,12},{14,26}}
    for _, t in ipairs(tufts) do
        px(img, ox+t[1],   oy+t[2],   C.grass_dark)
        px(img, ox+t[1]+1, oy+t[2],   C.grass_dark)
        px(img, ox+t[1],   oy+t[2]-1, C.grass_light)
        px(img, ox+t[1]+1, oy+t[2]-1, C.grass_light)
    end
    for x = 0, TILE_W - 1 do
        px(img, ox + x, oy + TILE_H - 1, C.grass_shadow)
    end
end

-- ============================================================
-- TILE 1,0 : Forest (森林)
-- ============================================================
local function drawForest(img, ox, oy)
    for y = 0, TILE_H - 1 do
        for x = 0, TILE_W - 1 do
            px(img, ox + x, oy + y, C.grass_mid)
        end
    end

    local function drawTree(cx, base_y, size)
        fillRect(img, ox+cx-1, oy+base_y-4, 3, 5, C.trunk)
        local layers = {
            {y=base_y-10, r=size},
            {y=base_y-16, r=size-2},
            {y=base_y-21, r=size-4},
        }
        for _, layer in ipairs(layers) do
            local ly, r = layer.y, layer.r
            for dy = -r, r do
                for dx = -r, r do
                    local dist = math.sqrt(dx*dx + dy*dy*1.4)
                    if dist <= r then
                        local c
                        if dx < -1 then
                            c = C.tree_dark
                        elseif dist > r - 1.5 then
                            c = C.tree_dark
                        elseif dy < -r*0.3 and dx > 0 then
                            c = C.tree_light
                        else
                            c = C.tree_mid
                        end
                        px(img, ox+cx+dx, oy+ly+dy, c)
                    end
                end
            end
            px(img, ox+cx,   oy+ly-r+1, C.tree_light)
            px(img, ox+cx+1, oy+ly-r+2, C.tree_light)
        end
    end

    drawTree(10, 28, 7)
    drawTree(22, 26, 6)
    drawTree(16, 30, 5)
end

-- ============================================================
-- TILE 2,0 : Mountain (山地)
-- ============================================================
local function drawMountain(img, ox, oy)
    for y = 0, TILE_H - 1 do
        for x = 0, TILE_W - 1 do
            px(img, ox+x, oy+y, C.grass_mid)
        end
    end

    local function drawMountainShape(cx, peak_y, base_y, w_base)
        for y = peak_y, base_y do
            local t = (y - peak_y) / (base_y - peak_y)
            local half_w = math.floor(t * w_base * 0.5)
            for x = cx - half_w, cx + half_w do
                local rel = x - cx
                local c
                if rel < -half_w * 0.3 then
                    c = C.rock_shadow
                elseif rel < 0 then
                    c = C.rock_dark
                elseif rel < half_w * 0.4 then
                    c = C.rock_mid
                else
                    c = C.rock_light
                end
                px(img, ox+x, oy+y, c)
            end
        end
        local snow_h = math.floor((base_y - peak_y) * 0.22)
        for y = peak_y, peak_y + snow_h do
            local t = (y - peak_y) / (base_y - peak_y)
            local half_w = math.floor(t * w_base * 0.5)
            local sw = math.max(1, half_w - 1)
            for x = cx - sw, cx + sw do
                px(img, ox+x, oy+y, C.snow)
            end
        end
    end

    drawMountainShape(16, 4, 31, 28)
    drawMountainShape(27, 10, 31, 14)
    drawMountainShape(5,  12, 31, 12)
end

-- ============================================================
-- TILE 0,1 : Castle (城堡)
-- ============================================================
local function drawCastle(img, ox, oy)
    for y = 24, TILE_H-1 do
        for x = 0, TILE_W-1 do
            px(img, ox+x, oy+y, C.grass_mid)
        end
    end
    fillRect(img, ox+4,  oy+14, 24, 11, C.wall_mid)
    fillRect(img, ox+4,  oy+14, 24, 2,  C.wall_light)
    fillRect(img, ox+4,  oy+22, 24, 3,  C.wall_dark)
    fillRect(img, ox+2,  oy+8,  8,  17, C.wall_mid)
    fillRect(img, ox+2,  oy+8,  8,  2,  C.wall_light)
    fillRect(img, ox+2,  oy+22, 8,  3,  C.wall_dark)
    fillRect(img, ox+22, oy+8,  8,  17, C.wall_mid)
    fillRect(img, ox+22, oy+8,  8,  2,  C.wall_light)
    fillRect(img, ox+22, oy+22, 8,  3,  C.wall_dark)
    for i = 0, 2 do
        fillRect(img, ox+2+i*3,  oy+6, 2, 3, C.wall_mid)
        fillRect(img, ox+22+i*3, oy+6, 2, 3, C.wall_mid)
    end
    fillRect(img, ox+12, oy+18, 8, 7, C.gate_dark)
    fillRect(img, ox+13, oy+16, 6, 9, C.gate_dark)
    fillRect(img, ox+13, oy+18, 3, 7, C.gate)
    fillRect(img, ox+16, oy+18, 3, 7, C.gate)
    for fy = 2, 5 do
        for fx = 16, 19 do
            px(img, ox+fx, oy+fy, C.flag_red)
        end
    end
    px(img, ox+15, oy+2, C.wall_dark)
    px(img, ox+15, oy+3, C.wall_dark)
    px(img, ox+15, oy+4, C.wall_dark)
    fillRect(img, ox+4,  oy+12, 2, 4, C.gate_dark)
    fillRect(img, ox+26, oy+12, 2, 4, C.gate_dark)
end

-- ============================================================
-- TILE 1,1 : Village (村庄)
-- ============================================================
local function drawVillage(img, ox, oy)
    for y = 0, TILE_H-1 do
        for x = 0, TILE_W-1 do
            px(img, ox+x, oy+y, C.grass_mid)
        end
    end
    fillRect(img, ox+6,  oy+16, 20, 13, C.house_wall)
    fillRect(img, ox+6,  oy+16, 20, 2,  C.house_shadow)
    fillRect(img, ox+6,  oy+26, 20, 3,  C.house_shadow)
    for y = 0, 9 do
        local w = y * 2 + 2
        local rx = 16 - y - 1
        for x = 0, w do
            local c = (x == 0 or x == w) and C.roof_dark or C.roof_red
            px(img, ox+rx+x, oy+7+y, c)
        end
    end
    fillRect(img, ox+22, oy+4,  3, 8, C.chimney)
    fillRect(img, ox+14, oy+21, 4, 8, C.door)
    px(img, ox+17, oy+25, C.house_wall)
    fillRect(img, ox+8,  oy+19, 4, 4, C.water_light)
    fillRect(img, ox+8,  oy+19, 4, 1, C.house_shadow)
    fillRect(img, ox+20, oy+19, 4, 4, C.water_light)
    fillRect(img, ox+20, oy+19, 4, 1, C.house_shadow)
end

-- ============================================================
-- TILE 2,1 : River (河流)
-- ============================================================
local function drawRiver(img, ox, oy)
    for y = 0, TILE_H-1 do
        for x = 0, TILE_W-1 do
            px(img, ox+x, oy+y, C.grass_mid)
        end
    end
    for y = 10, 21 do
        for x = 0, TILE_W-1 do
            local wave = math.floor(math.sin(x * 0.4 + y * 0.3) * 1.5)
            local wy = y + wave
            if wy >= 10 and wy <= 21 then
                local t = (wy - 10) / 11.0
                local c
                if t < 0.15 or t > 0.85 then
                    c = C.grass_shadow
                elseif t < 0.25 or t > 0.75 then
                    c = C.water_dark
                elseif (x + y) % 5 == 0 then
                    c = C.water_foam
                elseif x % 3 == 0 then
                    c = C.water_light
                else
                    c = C.water_mid
                end
                px(img, ox+x, oy+wy, c)
            end
        end
    end
    local foams = {{3,13},{8,16},{15,12},{22,17},{28,14}}
    for _, f in ipairs(foams) do
        px(img, ox+f[1],   oy+f[2],   C.water_foam)
        px(img, ox+f[1]+1, oy+f[2],   C.water_foam)
    end
end

-- ============================================================
-- TILE 0,2 : Sea (海洋)
-- ============================================================
local function drawSea(img, ox, oy)
    for y = 0, TILE_H-1 do
        for x = 0, TILE_W-1 do
            local depth = y / TILE_H
            local c
            if (x + y) % 8 == 0 then
                c = C.sea_foam
            elseif depth < 0.3 then
                c = C.sea_light
            elseif depth < 0.65 then
                c = C.sea_mid
            else
                c = C.sea_dark
            end
            px(img, ox+x, oy+y, c)
        end
    end
    for i = 0, 3 do
        local wy = 4 + i * 8
        for x = 0, TILE_W-1, 3 do
            px(img, ox+x,   oy+wy, C.sea_foam)
            px(img, ox+x+1, oy+wy, C.sea_foam)
        end
    end
end

-- ============================================================
-- TILE 1,2 : Desert (沙漠)
-- ============================================================
local function drawDesert(img, ox, oy)
    for y = 0, TILE_H-1 do
        for x = 0, TILE_W-1 do
            local c
            if (x + y) % 6 == 0 then
                c = C.sand_dark
            elseif (x * 2 + y) % 9 == 0 then
                c = C.sand_shadow
            elseif (x + y) % 3 == 0 then
                c = C.sand_light
            else
                c = C.sand_mid
            end
            px(img, ox+x, oy+y, c)
        end
    end
    local function drawCactus(cx, base)
        fillRect(img, ox+cx-1, oy+base-12, 3, 12, C.cactus)
        px(img, ox+cx-1, oy+base-12, C.tree_light)
        fillRect(img, ox+cx-4, oy+base-9,  4, 2, C.cactus)
        fillRect(img, ox+cx-4, oy+base-12, 2, 4, C.cactus)
        fillRect(img, ox+cx+1, oy+base-7,  4, 2, C.cactus)
        fillRect(img, ox+cx+2, oy+base-10, 2, 4, C.cactus)
    end
    drawCactus(10, 28)
    drawCactus(23, 26)
end

-- ============================================================
-- TILE 2,2 : Fort (要塞)
-- ============================================================
local function drawFort(img, ox, oy)
    for y = 0, TILE_H-1 do
        for x = 0, TILE_W-1 do
            px(img, ox+x, oy+y, C.grass_dark)
        end
    end
    fillRect(img, ox+2,  oy+22, 28, 8,  C.fort_mid)
    fillRect(img, ox+2,  oy+22, 28, 2,  C.fort_light)
    fillRect(img, ox+2,  oy+28, 28, 2,  C.fort_dark)
    fillRect(img, ox+2,  oy+8,  6,  22, C.fort_mid)
    fillRect(img, ox+2,  oy+8,  2,  22, C.fort_light)
    fillRect(img, ox+24, oy+8,  6,  22, C.fort_mid)
    fillRect(img, ox+28, oy+8,  2,  22, C.fort_dark)
    fillRect(img, ox+2,  oy+8,  28, 6,  C.fort_mid)
    fillRect(img, ox+2,  oy+8,  28, 2,  C.fort_light)
    local corners = {{2,2},{24,2},{2,22},{24,22}}
    for _, c in ipairs(corners) do
        fillRect(img, ox+c[1],   oy+c[2],   7, 7, C.fort_light)
        fillRect(img, ox+c[1]+5, oy+c[2],   2, 7, C.fort_dark)
        fillRect(img, ox+c[1],   oy+c[2]+5, 7, 2, C.fort_dark)
    end
    fillRect(img, ox+13, oy+22, 6, 8, C.fort_gate)
    fillRect(img, ox+14, oy+20, 4, 10, C.fort_gate)
    fillRect(img, ox+8,  oy+14, 16, 9, C.grass_dark)
    for fy = 10, 13 do
        for fx = 16, 19 do
            px(img, ox+fx, oy+fy, C.flag_red)
        end
    end
    px(img, ox+15, oy+10, C.fort_dark)
    px(img, ox+15, oy+11, C.fort_dark)
    px(img, ox+15, oy+12, C.fort_dark)
    for i = 0, 3 do
        fillRect(img, ox+6+i*5, oy+6, 3, 3, C.fort_mid)
    end
end

-- ============================================================
-- Draw all tiles onto the sprite
-- ============================================================
local tiles = {
    {0, 0, drawPlain},
    {1, 0, drawForest},
    {2, 0, drawMountain},
    {0, 1, drawCastle},
    {1, 1, drawVillage},
    {2, 1, drawRiver},
    {0, 2, drawSea},
    {1, 2, drawDesert},
    {2, 2, drawFort},
}

for _, tile in ipairs(tiles) do
    local col, row, drawFn = tile[1], tile[2], tile[3]
    drawFn(img, col * TILE_W, row * TILE_H)
end

-- Save PNG
local save_path = app.fs.joinPath(app.fs.userDocsPath, "FireEmblem_Terrain_32x32.png")
spr:saveCopyAs(save_path)
app.alert("Done! Tiles saved to:\n" .. save_path)
