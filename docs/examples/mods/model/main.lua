-- a mob with a model of its own: boxes in text, a picture laid out for them, and legs and a head that move as the game's mobs do
local MODEL = [[
texture 64 32
part body 0 14 0 0 0 -4 -3 -6 8 6 12
part head 0 14 -6 0 18 -3 -3 -4 6 6 6 head
part leg1 -3 17 4 28 0 -1 0 -1 2 7 2 legA
part leg2 3 17 4 28 0 -1 0 -1 2 7 2 legB
part leg3 -3 17 -4 28 0 -1 0 -1 2 7 2 legB
part leg4 3 17 -4 28 0 -1 0 -1 2 7 2 legA
]]
mod.hook("Item_staticCtor_after", function(call)
  registry.entity("critter", "pig", 10, 1.0)
  registry.entity_model("critter", MODEL)
  local pic = surface.create(64, 32)
  pic:write(0, 0, 64, 32, string.rep("\255\140\0\255", 64 * 32))
  registry.entity_texture("critter", pic)
  registry.entity("critter2", "critter", 10, 1.0)
  registry.entity_name("critter", "Critter") -- gives it the game's own spawn egg
  registry.entity_spawn("critter", "creature", 10, 2, 4) -- and lets it spawn by itself in groups of 2 to 4, as the game's own creatures do
end)
