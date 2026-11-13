-- the game's own menu navigator (hit testing, keyboard movement and scrolling of the shader and debug menus), kept by the mod across calls until it frees it
local navigator

local function get()
  if not navigator then
    navigator = MenuNavigator.new()
    -- a target is a kind and an index with a rectangle; view -1 is a fixed one, layer 0 the base
    navigator:Add(1, 0, 10, 10, 50, 30, -1, 0, true)
    navigator:Add(1, 1, 10, 40, 50, 60, -1, 0, true)
  end
  return navigator
end

mod.command("navhit", function(sender, args)
  local x, y = args:match("(%d+) (%d+)")
  local nav = get()
  return "target " .. nav:TargetAt(tonumber(x or 0), tonumber(y or 0)) .. " of " .. nav:Count()
end)
