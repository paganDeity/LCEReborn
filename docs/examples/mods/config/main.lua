-- a mod's settings kept in its data folder: defaults on the first run, the player's edits after
local std = require("lcestd:std")

local config = std.config.load({ greeting = "Hello", repeats = 2, loud = false })

mod.command("greet", function(sender)
  local text = config.greeting .. ", " .. sender
  if config.loud then text = text:upper() end
  return string.rep(text .. ". ", config.repeats)
end)

-- loading what was just saved gives the same table back
std.config.save({ greeting = "Hi there", repeats = 3, loud = true })
local again = std.config.load({ greeting = "", repeats = 0, loud = false })
assert(again.greeting == "Hi there" and again.repeats == 3 and again.loud == true, "the configuration round trips")
std.config.save(config)
