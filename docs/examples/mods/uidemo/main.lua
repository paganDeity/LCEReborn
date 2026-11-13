local ui = require("widgets:widgets")

local menu = ui.menu("uidemo_settings", {title = "Demo settings"})
local summary = menu:label("Nothing changed yet")
menu:toggle("Sound", true, function(on) summary.text = "Sound " .. (on and "on" or "off") end)
menu:slider("Volume", 0, 10, 5, function(value) summary.text = "Volume " .. value end)
menu:input("Name", "Steve", function(text) summary.text = "Name is " .. text end, 16)
menu:button("Close", function(self) self:close() end)

mod.command("uidemo", function(sender, args)
  return menu:open(0) and "opened" or "refused"
end)
