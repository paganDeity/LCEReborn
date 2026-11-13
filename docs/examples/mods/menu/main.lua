-- a menu of the game's console scene: a page is filled with rows and handed to the scene, and the scene's own
-- handlers (UIScene_CustomMenu_handle*) are what the mod listens to
local last = "nothing yet"

local function page_of(call)
  return call.self:GetPage():GetName()
end

mod.hook("UIScene_CustomMenu_handlePress_after", function(call)
  if page_of(call) == "demo" then last = "pressed " .. call.childId end
end)

mod.hook("UIScene_CustomMenu_handleSliderElementMove_after", function(call)
  if page_of(call) == "demo" then last = "volume " .. call.value end
end)

mod.command("menu", function()
  local page = UIMenuPage.new("demo")
  page:AddLabel(1, "Mod menu")
  page:AddButton(2, "Say hello", "Sends a greeting when pressed.")
  page:AddCheckbox(3, "Enabled", true, "A checkbox of the mod.")
  page:AddSlider(4, "Volume", 0, 10, 5, "A slider of the mod.")
  return ConsoleUIController.instance():NavigateToMenuPage(0, page) and "opened" or "refused"
end)
mod.command("menulast", function() return last end)
