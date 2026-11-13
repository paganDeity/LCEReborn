-- a sound that loops until stopped, panned left and at half volume, on a channel of its own (the sound is decoded and fed to an audio stream)
local std = require("lcestd:std")
local playing

mod.command("loopon", function()
  playing = std.audio.play("random.click", { loop = true, pan = -0.5, volume = 0.5 })
  return playing and "looping" or "that sound is not in the game"
end)

mod.command("loopoff", function()
  if playing then playing.stop() playing = nil end
  return "stopped"
end)
