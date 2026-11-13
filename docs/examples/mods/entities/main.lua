-- listing the entities of a level, all of them or the ones in a box, with std.world.entities (the game gives their ids, level:getEntity makes a handle of each)
local std = require("lcestd:std")
local function count(list)
  local by = {}
  for _, e in ipairs(list) do
    local t = TargetSelector.TypeName(e)
    by[t] = (by[t] or 0) + 1
  end
  local names = {}
  for t, n in pairs(by) do names[#names + 1] = t .. "=" .. n end
  table.sort(names)
  return table.concat(names, ",")
end
mod.command("ents", function()
  local level = MinecraftServer.instance():getLevel(0)
  return "all: " .. count(std.world.entities(level))
end)
mod.command("entsnear", function()
  local level = MinecraftServer.instance():getLevel(0)
  return "near: " .. count(std.world.entities(level, 258, 138, 240, 266, 146, 248))
end)

-- a list result: the player's hand slots come as a table of item stacks; run as the player (execute as @p run handslots),
-- because a reply to an executed command goes to the player, and read back with lasthands
local lastHands = "never"
mod.command("handslots", function(sender)
  local player = MinecraftServer.instance():getPlayers():getPlayerByName(sender)
  if not player then lastHands = "no player" return "" end
  local slots = player:getHandSlots()
  local main = slots[1] and slots[1]:getCount() or 0
  lastHands = "slots " .. #slots .. " main " .. main
  return ""
end)
mod.command("lasthands", function() return "hands " .. lastHands end)
