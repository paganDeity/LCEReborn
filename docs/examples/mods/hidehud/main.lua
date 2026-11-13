-- the console personality's HUD and button hints are scenes of the game; the generated calls reach both
mod.hook("MinecraftServer_tick", function(call)
  local ui = ConsoleUIController.instance()
  for player = 0, 3 do
    local hud = ui:GetHUD(player)
    if hud then hud:setVisible(false) end
    ui:SetEnableTooltips(player, false)
    for i = 0, 15 do ui:ShowTooltip(player, i, false) end
  end
end)
