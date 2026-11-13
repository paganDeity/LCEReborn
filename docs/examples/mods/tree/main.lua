-- a command with branches: /shop buy <item> <count>, /shop list, /shop help
local std = require("lcestd:std")

std.command.tree("shop", {
  buy = { arguments = "item number", run = function(sender, args)
    local item, count = args:match("^(%S+)%s+(%d+)$")
    if not item then return "usage: /shop buy <item> <count>" end
    return sender .. " buys " .. count .. " " .. item
  end },
  list = { run = function() return "nothing for sale yet" end },
  help = { run = function() return "buy <item> <count>, list" end },
})
