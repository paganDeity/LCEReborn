-- A new biome made from the game's own classes, and put into the chunks the game makes: generated symbols only.
local biome
local function make()
  local properties = BiomeProperties.new("Mirror Plains")
  properties:setTemperatureAndDownfall(0.9, 0.3)
  biome = PlainsBiome.new(60, false, properties)
  game.log("made biome; registered as id 60: " .. tostring(Biome.byId(60) ~= nil))
end
local n = 0
mod.hook("OverworldLevelSource_lightChunk", function(call)
  n = n + 1
  if not biome then make() end
  call.chunk:setBiomes(string.rep(string.char(60), 256))
  if n == 1 then game.log("first chunk's first biome id now " .. string.byte(call.chunk:getBiomes(), 1)) end
end)
