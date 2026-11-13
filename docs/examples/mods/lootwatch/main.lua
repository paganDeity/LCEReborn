local std = require("lcestd:std")
-- counts the loot hooks as they fire: a chest filled from its loot table, a mob's loot-table drops and a fishing retrieve
local fills, drops, catches = 0, 0, 0
local cancelFills = false
mod.hook("LootTable_fill", function(call)
  fills = fills + 1
  if cancelFills then call.cancel = true end
end)
local cancelDrops = false
mod.hook("Mob_dropAllDeathLoot", function(call)
  drops = drops + 1
  if cancelDrops then call.cancel = true end
end)
mod.command("lootcancel", function(sender, args)
  cancelDrops = args == "on"
  cancelFills = cancelDrops
  return "loot " .. (cancelDrops and "cancelled" or "allowed")
end, "<on|off>", 0, "on|off")
mod.hook("FishingHook_retrieve", function(call)
  catches = catches + 1
  if cancelDrops then call.cancel = true end
end)
mod.command("lootreset", function()
  fills, drops, catches = 0, 0, 0
  return "counts reset"
end)
mod.command("lootcount", function()
  return "fills " .. fills .. " drops " .. drops .. " catches " .. catches
end)
-- the items in the chest at 262 141 244; reading a chest that has a loot table fills it, so read it only once it was opened
mod.command("chestitems", function()
  local level = MinecraftServer.instance():getLevel(0)
  local n = 0
  for slot = 0, 26 do n = n + std.world.get_item_count(level, 262, 141, 244, slot) end
  return "items " .. n
end)
-- whether the player who runs this (as @p) has a bobber out; a reply to an executed command goes to the player, so lastbobber reads it back
local lastBobber = "never"
mod.command("bobber", function(sender)
  local player = MinecraftServer.instance():getPlayers():getPlayerByName(sender)
  lastBobber = player and (player:get_mFishing() and "set" or "none") or "no player"
  return ""
end)
mod.command("lastbobber", function() return "bobber " .. lastBobber end)
