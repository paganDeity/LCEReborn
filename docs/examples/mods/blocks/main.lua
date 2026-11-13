local std = require("lcestd:std")

mod.hook("Item_staticCtor_after", function(call)
  registry.item("ruby", "Ruby", "diamond", 64)

  registry.block("ruby_ore", "Ruby Ore", "iron_ore", "stone", 3.0, 5.0, 0)
  registry.block_drop("ruby_ore", "blocks:ruby", 1, 3)
  registry.creative("blocks:ruby_ore", 0)
  registry.creative("blocks:ruby", 3)

  registry.block("lamp", "Lamp", "stone", "stone", 1.0, 1.0, 0)
  registry.block_property("lamp", "lit", 2)
  registry.block_interactive("lamp")
  registry.block_face("lamp", "all", "glowstone", "lit=1")
  registry.creative("blocks:lamp", 1)

  registry.block("signpost", "Signpost", "planks_oak", "wood", 1.0, 1.0, 0)
  registry.block_property("signpost", "facing", 4)
  registry.block_shape("signpost", "6,0,6,10,16,10;6,10,0,10,14,6", "")
  registry.block_rotates("signpost")
  registry.creative("blocks:signpost", 1)

  registry.block("crate", "Crate", "planks_oak", "wood", 1.0, 1.0, 0)
  registry.block_entity("crate", 27)
  registry.block_interactive("crate")
  registry.fuel("blocks:crate", 300)
  registry.fuel("blocks:signpost", 300)
  registry.creative("blocks:crate", 1)

  registry.block("counter", "Counter", "gold_block", "metal", 1.0, 1.0, 0)
  registry.block_entity("counter", 0)
  registry.block_interactive("counter")
  registry.creative("blocks:counter", 7)

  registry.shaped("blocks:lamp", 1, "#/R", "#=minecraft:glass,R=blocks:ruby", "mechanisms", "minecraft:redstone_lamp")
  registry.smelting("blocks:ruby_ore", "blocks:ruby", 1.0)
end)

mod.hook("Block_getLightEmission", function(call)
  if call.self == "blocks:lamp" and call.state == 1 then
    call.result = 15
    call.cancel = true
  end
end)

-- a mod block that has registry.block_interactive reports its use through the Block_use hook of the game class
mod.hook("Block_use", function(call)
  local level = call.level
  local x, y, z = call.x, call.y, call.z
  local name = std.world.block_at(level, x, y, z)
  if name == "blocks:crate" then
    std.entity.open_container(call.player, x, y, z)
  elseif name == "blocks:lamp" then
    std.world.set_state(level, x, y, z, "lit", 1 - std.world.get_state(level, x, y, z, "lit"))
  elseif name == "blocks:counter" then
    std.entity.tell(call.player, "count: " .. (std.world.data_get(level, x, y, z, "n") or "0"))
  end
end)

mod.hook("ScriptedBlockEntity_tick", function(call)
  local self = call.self
  local level = self:get_mLevel()
  if level:get_mIsClientSide() then return end
  local x, y, z = self:get_mPos_x(), self:get_mPos_y(), self:get_mPos_z()
  if std.world.block_at(level, x, y, z) ~= "blocks:counter" then return end
  local n = tonumber(std.world.data_get(level, x, y, z, "n") or "0")
  std.world.data_set(level, x, y, z, "n", tostring(n + 1))
end)

-- the capabilities the world records in level.dat: once a chunk holding a mod block is saved, its palette is one (2)
mod.command("worldcaps", function()
  return "capabilities " .. MinecraftServer.instance():getLevel(0):getLevelData():getNbtCapabilities()
end)
