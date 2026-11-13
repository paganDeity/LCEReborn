-- Higher-level helpers written in Lua over the generated calls. Nothing here is C++: every function
-- is a few lines over the game's own methods, which is how the friendly layer stays cheap to keep.
local std = { command = {}, audio = {}, lang = {}, input = {}, data = {}, config = {}, net = {}, world = {}, entity = {}, redstone = {}, ui = {}, draw = {}, gen = {} }

-- the name of the block at a position, such as "minecraft:stone"
function std.world.block_at(level, x, y, z)
  local text = level:getBlockState(x, y, z):getBlock():toString()
  return text:match("{(.*)}") or text
end

-- true while a position is inside the world and its chunk is loaded
local function loaded(level, x, y, z)
  return level:isInWorldBounds(x, y, z) and level:hasChunkAt(x, y, z, false)
end

-- set the sky (layer 15) or block (layer 0) light at a position to 0..15; false when it cannot be set
function std.world.set_light(level, layer, x, y, z, value)
  if (layer ~= 15 and layer ~= 0) or value < 0 or value > 15 or not loaded(level, x, y, z) then return false end
  level:setBrightness(layer, x, y, z, value, true)
  return true
end

-- true when the block at a position is solid and passes redstone power through (a wire can climb over it)
function std.redstone.conducts(level, x, y, z)
  return loaded(level, x, y, z) and level:getBlockState(x, y, z):isSolidBlockingCubeAndNotSignalSource()
end

local function is_wire(level, x, y, z)
  return loaded(level, x, y, z) and level:getBlockState(x, y, z):getBlock():toString():find("minecraft:redstone_wire", 1, true) ~= nil
end

-- the power (0 to 15) of the redstone wire at a position, or -1 when there is no wire
function std.redstone.wire_power(level, x, y, z)
  return is_wire(level, x, y, z) and level:getBlockData(x, y, z) or -1
end

-- set the power of the wire at a position without telling its neighbours; false when it is not a wire or power is out of range
function std.redstone.set_wire_power(level, x, y, z, power)
  if power < 0 or power > 15 or not is_wire(level, x, y, z) then return false end
  level:setData(x, y, z, power, 2, false)
  return true
end

-- what the sources around a position give, wires not counted
function std.redstone.signal_at(level, x, y, z)
  return loaded(level, x, y, z) and Blocks.get_redstone_wire():externalSignal(level, x, y, z) or 0
end

-- tell the neighbours of a position that a wire changed
function std.redstone.update_wire_neighbors(level, x, y, z)
  if loaded(level, x, y, z) then level:updateNeighborsAt(x, y, z, Blocks.get_redstone_wire(), false) end
end

-- a mod's configuration, one `key = value` line each in config.txt in the mod's data folder: a value is true, false, a number or a "quoted string".
-- load(defaults) gives the defaults a table of the same keys with what the file says over them, and writes the file with the defaults when there is none
function std.config.load(defaults, path)
  path = path or "config.txt"
  local config = {}
  for key, value in pairs(defaults) do config[key] = value end
  local text = file.read(path)
  if not text then
    std.config.save(config, path)
    return config
  end
  for line in text:gmatch("[^\r\n]+") do
    local key, raw = line:match("^%s*([%w_%.%-]+)%s*=%s*(.-)%s*$")
    if key and defaults[key] ~= nil then
      if raw == "true" then config[key] = true
      elseif raw == "false" then config[key] = false
      elseif tonumber(raw) then config[key] = tonumber(raw)
      else
        local quoted = raw:match('^"(.*)"$')
        if quoted then config[key] = quoted end
      end
    end
  end
  return config
end

-- write a configuration table (booleans, numbers and strings) as load reads it, keys in order
function std.config.save(config, path)
  local keys = {}
  for key in pairs(config) do keys[#keys + 1] = key end
  table.sort(keys)
  local lines = {}
  for _, key in ipairs(keys) do
    local value = config[key]
    lines[#lines + 1] = key .. " = " .. (type(value) == "string" and ('"' .. value .. '"') or tostring(value))
  end
  return file.write(path or "config.txt", table.concat(lines, "\n") .. "\n")
end

-- a command with branches: std.command.tree("shop", { buy = { arguments = "item number", usage = "<item> <count>", run = function(sender, args) ... end },
-- list = { run = function(sender, args) ... end } }, permission). The first word picks the branch, which gets the rest of the line; the command box offers the branch names
-- and then the kinds the branch declares
function std.command.tree(name, branches, permission)
  local names = {}
  for branch in pairs(branches) do names[#names + 1] = branch end
  table.sort(names)
  local usage = table.concat(names, "|")
  local signatures = {}
  for _, branch in ipairs(names) do
    signatures[#signatures + 1] = (branch .. " " .. (branches[branch].arguments or "")):gsub("%s+$", "")
  end
  mod.command(name, function(sender, args)
    local word, rest = args:match("^(%S+)%s*(.*)$")
    local branch = word and branches[word]
    if not branch then return "usage: /" .. name .. " " .. usage end
    return branch.run(sender, rest)
  end, usage, permission or 0, table.concat(signatures, ";"))
end

-- play a sound by name (a game sound such as "random.click", or one the mod registered) on a channel of its own, and get a handle to stop it:
-- options.loop repeats it until stopped, options.volume scales it (1 is as recorded) and options.pan places it from -1 (left) to 1 (right)
local loops = {}
local loop_hook = false

local function feed(loop)
  while loop.stream:queued() < #loop.data do
    if not loop.stream:queue(loop.data) then return end
  end
end

function std.audio.play(name, options)
  options = options or {}
  local wav = sound.decode(name)
  if #wav < 45 then return nil end
  local channels, rate = string.unpack("<I2I4", wav, 23)
  local data = wav:sub(45)
  local volume, pan = options.volume or 1, options.pan or 0
  if volume ~= 1 or pan ~= 0 or channels == 1 and pan ~= 0 then
    local left, right = volume * (1 - math.max(pan, 0)), volume * (1 + math.min(pan, 0))
    local samples, count = {}, #data // 2
    for index = 1, count, channels do
      local value = string.unpack("<i2", data, index * 2 - 1)
      samples[#samples + 1] = string.pack("<i2i2", math.max(-32768, math.min(32767, math.floor(value * left))), math.max(-32768, math.min(32767, math.floor(value * right))))
    end
    data, channels = table.concat(samples), 2
  end
  local stream = audio.open(rate, channels)
  if not stream then return nil end
  stream:queue(data)
  local handle = { stream = stream, data = data, loop = options.loop }
  function handle.stop()
    if handle.stream then handle.stream:close() handle.stream = nil end
  end
  if options.loop then
    loops[#loops + 1] = handle
    if not loop_hook then
      loop_hook = true
      mod.hook("Minecraft_tick_after", function()
        for index = #loops, 1, -1 do
          local loop = loops[index]
          if loop.stream then feed(loop) else table.remove(loops, index) end
        end
      end)
    end
  end
  return handle
end

-- load the translations a mod ships: for each language code, lang/<code>.txt in the mod holds `key = text` lines, and a title given to registry.item or
-- registry.block that is a key shows its text for players of that language (for example std.lang.load({"es-ES", "de-DE"}))
function std.lang.load(codes)
  for _, code in ipairs(codes) do
    local text = file.read_mod("lang/" .. code .. ".txt")
    if text then
      for line in text:gmatch("[^\r\n]+") do
        local key, value = line:match("^%s*(.-)%s*=%s*(.-)%s*$")
        if key and key ~= "" then registry.translate(code, key, value) end
      end
    end
  end
end

-- true when the key or mouse button with a name such as "G", "F6" or "Mouse4" went down in the game's last tick, and no screen or chat has the keyboard
function std.input.pressed(name)
  local keys = CInput.instance():GetKeyboardMouse()
  local input = PcKeyboardMouse.InputFromName(name)
  return input >= 0 and keys:InputPressed(input) and not ModScreen.IsOpen(0)
end

-- run a function each time a key goes down: std.input.bind("G", function() ... end); the binding lives as long as the mod
function std.input.bind(name, action)
  mod.hook("Minecraft_tick_after", function()
    if std.input.pressed(name) then action() end
  end)
end

-- a value a mod keeps on an entity, saved with it (as an entity tag "mod|key=value"); value is text, and nil removes it
function std.data.entity_set(entity, key, value)
  local prefix = "mod|" .. key .. "="
  for tag in entity:getTags():gmatch("[^\n]+") do
    if tag:sub(1, #prefix) == prefix then entity:removeTag(tag) end
  end
  if value ~= nil then return entity:addTag(prefix .. value) end
  return true
end

-- the value a mod kept on an entity under a key, or nil
function std.data.entity_get(entity, key)
  local prefix = "mod|" .. key .. "="
  for tag in entity:getTags():gmatch("[^\n]+") do
    if tag:sub(1, #prefix) == prefix then return tag:sub(#prefix + 1) end
  end
  return nil
end

-- a mod's settings for one world, kept in its data folder under the world's name, as std.config keeps the mod's own: defaults first, the file over them
local function world_path(level)
  return "worlds/" .. level:getLevelData():getLevelName():gsub("[^%w_%-]", "_") .. ".txt"
end

function std.data.world_load(level, defaults)
  return std.config.load(defaults, world_path(level))
end

function std.data.world_save(level, values)
  return std.config.save(values, world_path(level))
end

-- send bytes on a channel (a name such as "mymod|chat") to every player from the server; each client mod reads it by hooking
-- ClientPacketListener_handleCustomPayload and checking packet:getIdentifier()
function std.net.broadcast(channel, bytes)
  MinecraftServer.instance():getPlayers():broadcastAll(ClientboundCustomPayloadPacket.new_2(channel, bytes))
end

-- send bytes on a channel from a client to the server, whose mods read it by hooking ServerGamePacketListenerImpl_handleCustomPayload
function std.net.send_to_server(channel, bytes, player_index)
  Minecraft.GetInstance():getConnection(player_index or 0):send(ServerboundCustomPayloadPacket.new_2(channel, bytes))
end

-- the entities of a level as a list of handles: all of them, or the ones whose box touches the box from (x0, y0, z0) to (x1, y1, z1); the game gives their ids and level:getEntity makes a
-- handle of each, valid for this call, and an entity that went away in between is left out
function std.world.entities(level, x0, y0, z0, x1, y1, z1)
  local ids = x0 == nil and level:getAllEntities() or level:getEntities(nil, AABB.new_2(x0, y0, z0, x1, y1, z1))
  local found = {}
  for id in ids:gmatch("[^\n]+") do
    local entity = level:getEntity(tonumber(id))
    if entity then found[#found + 1] = entity end
  end
  return found
end

-- place the block with a name such as "minecraft:stone" (its default state) and tell the neighbours; false when the name is unknown or the chunk is not loaded
function std.world.set_block(level, x, y, z, name)
  local block = Block.byName(name)
  if not block or not level:hasChunkAt(x, y, z, false) then return false end
  level:setBlockAndUpdate(x, y, z, block:defaultBlockState())
  -- the game's own answer says whether the state changed, so what is there now is what tells
  return level:getBlockState(x, y, z):getBlock():toString() == block:toString()
end

-- open the inventory of the mod block at a position (see registry.block_entity) in the chest window for a player; false when the entity is not a player, the
-- block has no inventory or the player already has a window open
function std.entity.open_container(player, x, y, z)
  local ok, opened = pcall(function()
    local store = player:get_mLevel():getBlockEntity(x, y, z)
    if not store or store:getContainerSize() == 0 then return false end
    return player:openContainer(store) ~= 0
  end)
  return ok and opened
end

-- the name of a player, or the type name of any other entity such as "minecraft:pig"
function std.entity.name(entity)
  if std.entity.is_player(entity) then return entity:getName() end
  return TargetSelector.TypeName(entity)
end

-- true when the entity is a player
function std.entity.is_player(entity)
  return entity:instanceof(EntityType.get_PLAYER())
end

-- send a chat line to a player; nothing happens for any other entity
function std.entity.tell(entity, text)
  ChatCommands.Reply(entity, text)
end

-- give a mob a goal of its own; the goal runs as the ScriptedGoal_* hooks answer, told its name. priority is the game's (lower runs first), flags
-- are 1 movement, 2 looking and 4 jumping. targets true adds it to the list that picks whom the mob attacks
function std.entity.add_goal(mob, name, priority, flags, targets)
  if name == "" or #name > 64 then return false end
  local selector = targets and mob:getTargetSelector() or mob:getGoalSelector()
  selector:addGoal(priority, ScriptedGoal.new(mob, name, flags), true)
  return true
end

-- remove the goals a mod added under a name, from both lists; true when one was removed
function std.entity.remove_goal(mob, name)
  local from_goals = ScriptedGoal.Remove(mob:getGoalSelector(), name)
  local from_targets = ScriptedGoal.Remove(mob:getTargetSelector(), name)
  return from_goals or from_targets
end

-- walk a mob to a position at a speed (1.0 is its normal walk); false when it has no path
function std.entity.move_to(mob, x, y, z, speed)
  return mob:getNavigation():moveTo(x, y, z, speed)
end

-- turn a mob's head toward a position
function std.entity.look_at(mob, x, y, z)
  mob:getLookControl():setLookAt_2(x, y, z, 30.0, 30.0)
end

-- take every goal and target goal from a mob, so a mod gives it the whole of its behaviour
function std.entity.clear_goals(mob)
  mob:getGoalSelector():removeAllGoals()
  mob:getTargetSelector():removeAllGoals()
end

-- ask for a Block_tick of the block at a position after 1 to 1200 game ticks; false when the chunk is not loaded or the delay is out of range
function std.world.schedule_tick(level, x, y, z, delay)
  if delay < 1 or delay > 1200 or not level:hasChunkAt(x, y, z, false) then return false end
  level:forceAddBlockTick(x, y, z, level:getBlockState(x, y, z):getBlock(), delay, 0)
  return true
end

-- play a sound at a position for everyone near (within 16 blocks), by a name such as "minecraft:mob.villager.idle" or one a mod registered;
-- false when there is no such sound
function std.world.play_sound(level, x, y, z, name, volume, pitch)
  local sound = SoundEvents.byName((name:lower():gsub("^minecraft:", "")))
  if not sound then return false end
  level:playSound_2(nil, x, y, z, sound, 0, volume, pitch, 16.0)
  return true
end

-- the text a mod block stored under a key at a position (see registry.block_entity), or nil when nothing is stored or the block has no store
function std.world.data_get(level, x, y, z, key)
  local store = level:getBlockEntity(x, y, z)
  if not store or not store:Has(key) then return nil end
  return store:Get(key)
end

-- store text under a key for the mod block at a position; false when the block has no store or a limit (64 keys, 32 characters in a key,
-- 1024 bytes in a value) is reached
function std.world.data_set(level, x, y, z, key, value)
  local store = level:getBlockEntity(x, y, z)
  return store ~= nil and store:Set(key, value)
end

-- the value of a property of a mod block at a position (see registry.block_property), or -1 when the block has no such property
function std.world.get_state(level, x, y, z, property)
  if not level:hasChunkAt(x, y, z, false) then return -1 end
  local state = level:getBlockState(x, y, z)
  local block = state:getBlock()
  local ok, index = pcall(block.GetPropertyIndex, block, property)
  if not ok or index < 0 then return -1 end
  return block:GetValue(state, index)
end

-- set a property of a mod block at a position and tell the neighbours; false when the block has no such property or the value is out of range
function std.world.set_state(level, x, y, z, property, value)
  if not level:hasChunkAt(x, y, z, false) then return false end
  local state = level:getBlockState(x, y, z)
  local block = state:getBlock()
  local ok, index = pcall(block.GetPropertyIndex, block, property)
  if not ok or index < 0 or value < 0 or value >= block:GetPropertyCount(index) then return false end
  level:setBlockAndUpdate(x, y, z, block:WithValue(state, index, value))
  return std.world.get_state(level, x, y, z, property) == value
end

-- the inventory of the mod block at a position (see registry.block_entity), or nil when the block has none
local function inventory(level, x, y, z)
  local store = level:getBlockEntity(x, y, z)
  if not store then return nil end
  local ok, size = pcall(store.getContainerSize, store)
  if not ok or size == 0 then return nil end
  return store, size
end

-- the name of the item in a slot of a mod block's inventory, or an empty string when the slot is empty or does not exist
function std.world.get_item(level, x, y, z, slot)
  local store, size = inventory(level, x, y, z)
  if not store or slot < 0 or slot >= size then return "" end
  local stack = store:getItem(slot)
  if stack:isEmpty() then return "" end
  return ModItemNames.NameOf(stack:getItem())
end

-- how many of the item are in that slot, 0 when it is empty
function std.world.get_item_count(level, x, y, z, slot)
  local store, size = inventory(level, x, y, z)
  if not store or slot < 0 or slot >= size then return 0 end
  local stack = store:getItem(slot)
  return stack:isEmpty() and 0 or stack:getCount()
end

-- put count (0 to 64) of an item, by name, into a slot; an empty name or a count of 0 empties it. False when there is no such slot or the item is unknown
function std.world.set_item(level, x, y, z, slot, name, count)
  local store, size = inventory(level, x, y, z)
  if not store or slot < 0 or slot >= size or count < 0 or count > 64 then return false end
  if name == "" or count == 0 then
    store:removeItemNoUpdate(slot)
    return true
  end
  local item = ModItemNames.Find(name)
  if not item then return false end
  store:setItem(slot, ItemInstance.new_6(item, count, 0))
  return true
end

-- the height of the surface above a column
function std.world.surface(level, x, z)
  return level:getHeightmap(x, z)
end

-- true while a position is in daylight and open to the sky
function std.world.sunlit(level, x, y, z)
  return level:isDay() and level:canSeeSky(x, y, z)
end

-- hide the console personality's heads-up display and button hints for every player; call it each tick
function std.ui.hide_hud()
  local ui = ConsoleUIController.instance()
  for player = 0, 3 do
    local hud = ui:GetHUD(player)
    if hud then hud:setVisible(false) end
    ui:SetEnableTooltips(player, false)
    for i = 0, 15 do ui:ShowTooltip(player, i, false) end
  end
end

-- draw one coloured quad facing the camera at a depth, in camera space (the renderer's own calls)
function std.draw.quad(builder, r, g, b, x0, y0, x1, y1, z)
  builder:color_3(r, g, b, 1.0)
  builder:vertex(x0, y0, z); builder:vertex(x1, y0, z); builder:vertex(x1, y1, z)
  builder:vertex(x0, y0, z); builder:vertex(x1, y1, z); builder:vertex(x0, y1, z)
end

-- run fn between the state a flat coloured draw needs and restoring it
function std.draw.flat(fn)
  GlStateManager.enableBlend()
  GlStateManager.blendFunc(4, 5)
  GlStateManager.disableTexture()
  GlStateManager.disableCull()
  GlStateManager.color4f(1, 1, 1, 1)
  local tess = Tesselator.getInstance()
  local builder = tess:getBuilder()
  builder:begin_2(0)
  fn(builder)
  tess:end_()
  GlStateManager.enableTexture()
  GlStateManager.enableCull()
end

-- Place a structure: layers[1] is the lowest, each layer a list of rows, each row a string whose characters are looked up in `legend`
-- (a space leaves the block as it is). Placed with its first character at x, y, z, rows running along z and characters along x.
function std.gen.structure(level, x, y, z, layers, legend)
  for dy, rows in ipairs(layers) do
    for dz, row in ipairs(rows) do
      for dx = 1, #row do
        local block = legend[row:sub(dx, dx)]
        if block then std.world.set_block(level, x + dx - 1, y + dy - 1, z + dz - 1, block) end
      end
    end
  end
end

-- make a biome like one of the game's plains and give it its climate, then return it (id 48 to 127 are free for mods)
function std.gen.plains_biome(id, name, temperature, downfall)
  local properties = BiomeProperties.new(name)
  properties:setTemperatureAndDownfall(temperature, downfall)
  return PlainsBiome.new(id, false, properties)
end

-- set the block of a position (0 to 15 across, from 0 up) in a chunk being generated, by name; false when the name is unknown
function std.gen.set_block(chunk, x, y, z, name)
  local block = Block.byName(name)
  if not block then return false end
  return chunk:setBlockAndData(x, y, z, Block.getId(block), 0, false, true)
end

-- set a block of the primer a world generator is filling, by name; false when the name is unknown
function std.gen.primer_set(primer, x, y, z, name)
  local block = Block.byName(name)
  if not block then return false end
  primer:setState(x, y, z, block:defaultBlockState())
  return true
end

-- the name of the block in a primer
function std.gen.primer_get(primer, x, y, z)
  local text = primer:getState(x, y, z):getBlock():toString()
  return text:match("{(.*)}") or text
end

-- make every column of a chunk the given biome
function std.gen.fill_biome(chunk, id)
  chunk:setBiomes(string.rep(string.char(id), 256))
end

return std
