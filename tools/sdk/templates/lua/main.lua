-- {{name}}: a starting point. Every call and hook is a method of a game class, listed in docs/modding-api.md (generated from the game's headers);
-- `lcestd` is the friendly layer in Lua (std.world, std.entity, std.net, std.config, std.data, std.input, std.lang).
local std = require("lcestd:std")

-- the mod's settings, kept in its data folder: the defaults on the first run, the player's edits after
local config = std.config.load({ greeting = "Hello" })

-- a hook is a game method: this runs as the server's tick begins (add _after for the end of it, and call.cancel = true to skip the game's work)
local ticks = 0
mod.hook("MinecraftServer_tick", function()
  ticks = ticks + 1
end)

-- a command of the game's own: listed by /help with its usage, run only by an operator here (0 anyone, 2 operators, 4 console)
mod.command("{{id}}", function(sender, args)
  return config.greeting .. ", " .. sender .. " (" .. ticks .. " ticks)"
end, "[text]", 2)

game.log("{{name}} started")
