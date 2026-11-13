-- Blocks made from the game's own classes: they behave as the vanilla door, trapdoor, button,
-- pressure plate, lever and fence do, and Block_* hooks change any method of them.

local function solid(r, g, b)
  local picture = surface.create(16, 16)
  picture:write(0, 0, 16, 16, string.rep(string.char(r, g, b, 255), 256))
  return picture
end

mod.hook("Item_staticCtor_after", function(call)
  registry.block_of("ruby_door", "door", "Ruby Door", "metal", 5.0, 5.0, 0)
  registry.block_of("ruby_trapdoor", "trapdoor", "Ruby Trapdoor", "metal", 3.0, 5.0, 0)
  registry.block_of("ruby_button", "button", "Ruby Button", "stone", 0.5, 0.5, 0)
  registry.block_of("ruby_plate", "pressure_plate", "Glowing Plate", "stone", 0.5, 0.5, 0)
  registry.block_of("ruby_lever", "lever", "Ruby Lever", "wood", 0.5, 0.5, 0)
  registry.block_of("ruby_fence", "fence", "Ruby Fence", "wood", 2.0, 5.0, 0)
  -- the pictures are the ones the game's door class reads (upper and lower), so there is no layout to work out
  registry.block_texture("ruby_door", solid(200, 30, 60), "upper", "")
  registry.block_texture("ruby_door", solid(120, 10, 40), "lower", "")
  -- derived from a block of the mod, it wears the same pictures: a door again, softer and wooden
  registry.block_of("oak_ruby_door", "ruby_door", "Oak Ruby Door", "wood", 3.0, 3.0, 0)
  for _, name in ipairs({ "ruby_door", "oak_ruby_door", "ruby_trapdoor", "ruby_button", "ruby_plate", "ruby_lever", "ruby_fence" }) do
    registry.creative("kinds:" .. name, 1)
  end
end)

-- the plate glows: a method of the game's class answered by the mod
mod.hook("Block_getLightEmission", function(call)
  if call.self == "kinds:ruby_plate" then
    call.result = 15
    call.cancel = true
  end
end)
