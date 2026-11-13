local std = require("lcestd:std")
-- a channel of a mod's own: the server sends bytes, the client mod reads them and answers, the server mod reads that
local out = "none"
local log = ""
-- the client side of the channel: runs as the game reads a custom payload
mod.hook("ClientPacketListener_handleCustomPayload", function(call)
  local p = call.packet
  if p:getIdentifier() == "network|hello" then
    log = log .. "client got " .. #p:getData() .. " bytes: " .. p:getData() .. ";"
    -- answer on the same channel
    call.self:send(ServerboundCustomPayloadPacket.new_2("network|reply", "pong"))
  end
end)
-- the server side
mod.hook("ServerGamePacketListenerImpl_handleCustomPayload", function(call)
  local p = call.packet
  if p:getIdentifier() == "network|reply" then log = log .. "server got " .. p:getData() .. ";" end
end)
mod.command("netsend", function()
  std.net.broadcast("network|hello", "ping\0\1\2")
  return "sent"
end)
mod.command("netlog", function() return log end)
