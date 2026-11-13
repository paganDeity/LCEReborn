-- a key of the mod's own on the PC client: std.input.bind runs the function each time the key goes down, unless a screen has the keyboard
local std = require("lcestd:std")
local presses = 0

std.input.bind("G", function()
  presses = presses + 1
  game.log("G pressed " .. presses .. " times")
end)

mod.command("presses", function() return tostring(presses) end)
