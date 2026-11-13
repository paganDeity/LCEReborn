-- the game's own world stages are all skipped (they are methods of its renderers, hooked by name); the mod draws a frame of its own with the game's renderer calls
for _, hook in ipairs({ "LevelRenderer_renderSky", "LevelRenderer_render", "LevelRenderer_renderEntities", "LevelRenderer_renderDestroyAnimation",
                       "ParticleEngine_render", "ParticleEngine_renderLit", "GameRenderer_renderSnowAndRain", "GameRenderer_prepareAndRenderClouds" }) do
  mod.hook(hook, function(call) call.cancel = true end)
end
local logged = false
local function quad(b, r, g, bl, x0, y0, x1, y1, z)
  b:color_3(r, g, bl, 1.0)
  b:vertex(x0, y0, z); b:vertex(x1, y0, z); b:vertex(x1, y1, z)
  b:vertex(x0, y0, z); b:vertex(x1, y1, z); b:vertex(x0, y1, z)
end
mod.hook("GameRenderer_renderWeather", function(call)
  GlStateManager.enableBlend()
  GlStateManager.blendFunc(4, 5)
  GlStateManager.disableTexture()
  GlStateManager.disableCull()
  GlStateManager.color4f(1, 1, 1, 1)
  local tess = Tesselator.getInstance()
  local b = tess:getBuilder()
  b:begin_2(0)
  quad(b, 1, 0.2, 0.2, -2, -1, 2, 2, 4)
  quad(b, 0.2, 1, 0.2, -1, -0.5, 1, 1, 3.5)
  tess:end_()
  GlStateManager.enableTexture()
  GlStateManager.enableCull()
  if not logged then logged = true; game.log("pipeline mod drew a frame") end
end)
