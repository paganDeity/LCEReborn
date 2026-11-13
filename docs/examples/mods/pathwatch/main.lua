local std = require("lcestd:std")
-- the game's path searches as they begin: which mob searches, for what, and a switch that answers every search with no
-- path, so the mobs stand still
local searches, cancelled, last = 0, 0, "none"
local cancelAll = false

local function seen(call, target)
  searches = searches + 1
  last = (call.mob and call.mob:getEncodeId() or "nothing") .. " to " .. target
  if cancelAll then
    cancelled = cancelled + 1
    call.result = nil
    call.cancel = true
  end
end

mod.hook("PathFinder_findPath_2", function(call) seen(call, "a point") end)
mod.hook("PathFinder_findPath_3", function(call) seen(call, "a block") end)
mod.hook("PathFinder_findPath_4", function(call) seen(call, call.entity and call.entity:getEncodeId() or "nothing") end)

mod.command("pathcount", function()
  return "searches " .. searches .. " cancelled " .. cancelled .. " last " .. last
end)
mod.command("pathcancel", function(sender, args)
  cancelAll = args == "on"
  searches, cancelled, last = 0, 0, "none"
  return "paths " .. (cancelAll and "cancelled" or "allowed")
end, "<on|off>", 0, "on|off")
-- how far the cows moved from where pathcancel last put them, in whole blocks
local start = {}
mod.command("cowsmoved", function(sender, args)
  local level = MinecraftServer.instance():getLevel(0)
  local moved = 0
  for _, cow in ipairs(std.world.entities(level)) do
    local id = cow:getId()
    if TargetSelector.TypeName(cow) == "cow" then
      local x, z = cow:get_mX(), cow:get_mZ()
      if args == "mark" then
        start[id] = {x, z}
      elseif start[id] then
        moved = math.max(moved, math.floor(math.abs(x - start[id][1]) + math.abs(z - start[id][2])))
      end
    end
  end
  return args == "mark" and "marked" or ("moved " .. moved)
end)
