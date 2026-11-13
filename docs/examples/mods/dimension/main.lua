-- a dimension of a mod's own: declared while the registries are open and entered with a command run by the player
mod.hook("Item_staticCtor_after", function()
  registry.dimension("hollow", 5)
end)
mod.command("enter", function(sender, args)
  local player = MinecraftServer.instance():getPlayers():getPlayerByName(sender)
  if not player then return "no player " .. sender end
  player:changeDimension(tonumber(args) or 5)
  return "entered"
end, "<id>", 0, "number")
