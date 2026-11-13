local ui = {}

local ROW = 20
local PAD = 8
local menus = {}
local hooked = false

local function focusable(widget)
  return widget.kind ~= "label"
end

local function layout(menu, screen_width, screen_height)
  local width = menu.width
  local height = PAD * 2 + 14
  for _, widget in ipairs(menu.widgets) do height = height + widget.rows * ROW end
  local x = (screen_width - width) // 2
  local y = (screen_height - height) // 2
  local top = y + PAD + 14
  for _, widget in ipairs(menu.widgets) do
    widget.x, widget.y, widget.w, widget.h = x + PAD, top, width - PAD * 2, widget.rows * ROW - 2
    top = top + widget.rows * ROW
  end
  menu.box = {x = x, y = y, w = width, h = height}
end

local function widget_at(menu, px, py)
  for index, widget in ipairs(menu.widgets) do
    if focusable(widget) and px >= widget.x and px < widget.x + widget.w and py >= widget.y and py < widget.y + widget.h then
      return index, widget
    end
  end
end

local function step_focus(menu, direction)
  local count = #menu.widgets
  local index = menu.focus
  for _ = 1, count do
    index = (index - 1 + direction) % count + 1
    if focusable(menu.widgets[index]) then menu.focus = index return end
  end
end

local function activate(menu, widget, px)
  if widget.kind == "button" then
    if widget.on_click then widget.on_click(menu) end
  elseif widget.kind == "toggle" then
    widget.value = not widget.value
    if widget.on_change then widget.on_change(widget.value, menu) end
  elseif widget.kind == "slider" then
    if px then
      local fraction = (px - widget.x) / widget.w
      fraction = math.max(0, math.min(1, fraction))
      widget.value = math.floor(widget.min + fraction * (widget.max - widget.min) + 0.5)
      if widget.on_change then widget.on_change(widget.value, menu) end
    end
  end
end

local function adjust(menu, widget, delta)
  if widget.kind == "slider" then
    widget.value = math.max(widget.min, math.min(widget.max, widget.value + delta))
    if widget.on_change then widget.on_change(widget.value, menu) end
  elseif widget.kind == "toggle" then
    activate(menu, widget)
  end
end

local WHITE = 0xFFFFFFFF

local function draw_widget(painter, widget, focused, hovered)
  local x, y, w, h = widget.x, widget.y, widget.w, widget.h
  local text_y = y + (h - 8) // 2
  if widget.kind == "label" then
    painter:Text(widget.text, x, text_y, 0xFF404040, false)
  elseif widget.kind == "button" then
    painter:Button(x, y, x + w, y + h, focused or hovered, true)
    painter:Centred(widget.text, x + w // 2, text_y, WHITE, true)
  elseif widget.kind == "toggle" then
    painter:Button(x, y, x + w, y + h, focused or hovered, true)
    painter:Text(widget.text, x + 6, text_y, WHITE, true)
    local half = 28
    painter:SwitchSegment(x + w - 2 * half - 4, y + 3, x + w - half - 4, y + h - 3, "Off", not widget.value, 0xFF8A2B2B, 0xFF4A1414)
    painter:SwitchSegment(x + w - half - 4, y + 3, x + w - 4, y + h - 3, "On", widget.value, 0xFF3C8527, 0xFF1D4D13)
  elseif widget.kind == "slider" then
    painter:Box(x, y, x + w, y + h, focused or hovered)
    painter:Text(widget.text .. ": " .. widget.value, x + 6, y + 3, WHITE, true)
    painter:Slider(x, x + w, y + h, widget.value - widget.min, widget.max - widget.min + 1, focused, 0xFF3C8527)
  elseif widget.kind == "input" then
    painter:Box(x, y, x + w, y + h, focused or hovered)
    painter:Text(widget.text .. ": " .. widget.value .. (focused and "_" or ""), x + 6, text_y, WHITE, true)
  end
end

local function install_hooks()
  if hooked then return end
  hooked = true
  mod.hook("ModScreen_Draw", function(call)
    local menu = menus[ModScreen.Name(call.pad)]
    if not menu then return end
    local gui = call.gui
    layout(menu, call.width, call.height)
    local box = menu.box
    local painter = gui:painter()
    painter:Fill(0, 0, call.width, call.height, 0x90000000)
    painter:Panel(box.x, box.y, box.x + box.w, box.y + box.h)
    painter:Text(menu.title, box.x + PAD, box.y + PAD - 2, 0xFF404040, false)
    for index, widget in ipairs(menu.widgets) do
      draw_widget(painter, widget, index == menu.focus, index == menu.hover)
    end
  end)
  mod.hook("ModScreen_Input", function(call)
    local menu = menus[ModScreen.Name(call.pad)]
    if not menu or not menu.box then return end
    local kind = call.kind
    if kind == "move" then
      menu.hover = widget_at(menu, call.x, call.y)
      call.cancel = true
    elseif kind == "press" and call.button == 1 then
      local index, widget = widget_at(menu, call.x, call.y)
      if index then
        menu.focus = index
        activate(menu, widget, call.x)
      end
      call.cancel = true
    elseif kind == "key" then
      local widget = menu.widgets[menu.focus]
      local key = call.text
      if key == "Down" or key == "Tab" then step_focus(menu, 1) call.cancel = true
      elseif key == "Up" then step_focus(menu, -1) call.cancel = true
      elseif key == "Left" and widget then adjust(menu, widget, -1) call.cancel = true
      elseif key == "Right" and widget then adjust(menu, widget, 1) call.cancel = true
      elseif key == "Enter" and widget then activate(menu, widget) call.cancel = true
      elseif key == "Backspace" and widget and widget.kind == "input" then
        widget.value = widget.value:sub(1, -2)
        if widget.on_change then widget.on_change(widget.value, menu) end
        call.cancel = true
      end
    elseif kind == "text" then
      local widget = menu.widgets[menu.focus]
      if widget and widget.kind == "input" and #widget.value < widget.max_length then
        widget.value = widget.value .. call.text
        if widget.on_change then widget.on_change(widget.value, menu) end
      end
      call.cancel = true
    end
  end)
end

local Menu = {}
Menu.__index = Menu

local function add(menu, widget)
  widget.rows = widget.rows or 1
  menu.widgets[#menu.widgets + 1] = widget
  if menu.focus == 0 and focusable(widget) then menu.focus = #menu.widgets end
  return widget
end

function Menu:label(text) return add(self, {kind = "label", text = text}) end
function Menu:button(text, on_click) return add(self, {kind = "button", text = text, on_click = on_click}) end
function Menu:toggle(text, value, on_change) return add(self, {kind = "toggle", text = text, value = value, on_change = on_change}) end
function Menu:slider(text, min, max, value, on_change)
  return add(self, {kind = "slider", text = text, min = min, max = max, value = value, on_change = on_change})
end
function Menu:input(text, value, on_change, max_length)
  return add(self, {kind = "input", text = text, value = value or "", on_change = on_change, max_length = max_length or 32})
end

function Menu:open(player)
  self.player = player
  return ModScreen.Open(player, self.name)
end

function Menu:close()
  if self.player then ModScreen.Close(self.player) end
end

function ui.menu(name, options)
  install_hooks()
  options = options or {}
  local menu = setmetatable({name = name, title = options.title or name, width = options.width or 220,
                             widgets = {}, focus = 0, hover = nil}, Menu)
  menus[name] = menu
  return menu
end

return ui
