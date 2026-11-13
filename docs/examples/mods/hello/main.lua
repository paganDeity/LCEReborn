local ticks = 0

-- every hook is a game function: this one runs as the server's tick begins
mod.hook("MinecraftServer_tick", function()
  ticks = ticks + 1
end)

-- cancel a chat message the server is about to handle
mod.hook("ServerGamePacketListenerImpl_handleChat_2", function(call)
  if call.packet:getMessage() == "badword" then
    call.cancel = true
  end
end)

mod.hook("PlayerList_add", function(call)
  game.say("Welcome, " .. call.player:getName() .. "!")
end)

mod.command("hello", function(sender, args)
  return "Hello " .. sender .. ", " .. #game.players() .. " player(s) online, " .. ticks .. " ticks, args: " .. args
end)

game.log("hello mod started")
