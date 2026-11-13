-- a title that is a key: lang/<code>.txt holds `key = text` lines and the player sees the text for their language, or the key where there is none
local std = require("lcestd:std")
std.lang.load({ "en-US", "es-ES" })

mod.hook("Item_staticCtor_after", function(call)
  registry.item("ruby", "item.ruby", "diamond", 64)
end)
