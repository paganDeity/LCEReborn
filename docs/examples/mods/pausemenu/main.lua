-- replace one of the game's own scenes with a menu page of the mod: the scene factory is NavigateToScene, so a hook that
-- cancels it and opens a page answers for the scene (5 is the pause menu, see EUIScene)
local replaced = 0
mod.hook("ConsoleUIController_NavigateToScene", function(call)
  if call.scene == 5 then
    replaced = replaced + 1
    local page = UIMenuPage.new("mypause")
    page:AddLabel(1, "My pause menu")
    page:AddButton(2, "Resume", "Back to the game.")
    ConsoleUIController.instance():NavigateToMenuPage(call.playerId, page)
    call.result = true
    call.cancel = true
  end
end)
mod.command("replaced", function() return "replaced " .. replaced end)
