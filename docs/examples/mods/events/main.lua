-- the game's own events, as a mod sees them: a block broken, an item used on a block, a mob hurt and killed, a chat line. Each is a woven
-- method of the class that does the work, so the counts move only when the game really does it
local count = { destroy = 0, useon = 0, hurt = 0, die = 0, chat = 0 }

local function counter(name)
  return function() count[name] = count[name] + 1 end
end

mod.hook("ServerPlayerGameMode_destroyBlock", counter("destroy"))
mod.hook("ServerPlayerGameMode_useItemOn", counter("useon"))
mod.hook("LivingEntity_hurt", counter("hurt"))
mod.hook("LivingEntity_die", counter("die"))
mod.hook("ServerGamePacketListenerImpl_handleChat", counter("chat"))
mod.hook("ServerGamePacketListenerImpl_handleChat_2", counter("chat"))

mod.command("eventcount", function()
  return string.format("destroy %d useon %d hurt %d die %d chat %d", count.destroy, count.useon, count.hurt, count.die, count.chat)
end)
