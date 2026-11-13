-- a block drawn with a model of boxes of its own on top of its shape: a block with a store (registry.block_entity), a model as text and a picture for it
mod.hook("Item_staticCtor_after", function(call)
  registry.block("statue", "Statue", "stone", "stone", 1.0, 1.0, 0)
  registry.block_entity("statue", 0)
  local picture = surface.create(64, 32)
  picture:write(0, 0, 64, 32, string.rep("\200\30\200\255", 64 * 32))
  registry.block_model("statue", [[
texture 64 32
part base 0 24 0 0 0 -4 -6 -4 8 6 8
part pillar 0 18 0 0 14 -2 -14 -2 4 14 4
part top 0 4 0 0 0 -4 -4 -4 8 4 8
]], picture)
end)
