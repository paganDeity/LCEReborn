-- Replaces the game's redstone wire logic: when a wire changes, the whole connected
-- network is solved once, instead of wire by wire with a chain of neighbour updates.

local std = require("lcestd:std")
local rs = std.redstone

local WIRE = "minecraft:redstone_wire"
local FLAT = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} }

local function key(x, y, z) return x .. "," .. y .. "," .. z end

-- wires a wire at x,y,z connects to: beside it, one up over a clear roof, one down past a clear side
local function links(level, x, y, z)
  local out = {}
  local roofOpen = not rs.conducts(level, x, y + 1, z)
  for _, d in ipairs(FLAT) do
    local nx, nz = x + d[1], z + d[2]
    if rs.wire_power(level, nx, y, nz) >= 0 then
      out[#out + 1] = { nx, y, nz }
    end
    if rs.conducts(level, nx, y, nz) then
      if roofOpen and rs.wire_power(level, nx, y + 1, nz) >= 0 then out[#out + 1] = { nx, y + 1, nz } end
    elseif rs.wire_power(level, nx, y - 1, nz) >= 0 then
      out[#out + 1] = { nx, y - 1, nz }
    end
  end
  return out
end

mod.hook("wire_update", function(call)
  local level = call.level
  local nodes, order, seen = {}, {}, {}
  local queue = { { call.x, call.y, call.z } }
  if rs.wire_power(level, call.x, call.y, call.z) < 0 then
    -- the wire is gone: start from the wires around it
    queue = {}
    for _, d in ipairs({ {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0} }) do
      for dy = -1, 1 do
        local nx, ny, nz = call.x + d[1], call.y + d[2] + dy, call.z + d[3]
        if rs.wire_power(level, nx, ny, nz) >= 0 then queue[#queue + 1] = { nx, ny, nz } end
      end
    end
  end

  local head = 1
  while head <= #queue do
    local p = queue[head]; head = head + 1
    local k = key(p[1], p[2], p[3])
    if not seen[k] then
      seen[k] = true
      local node = { x = p[1], y = p[2], z = p[3], old = rs.wire_power(level, p[1], p[2], p[3]), links = links(level, p[1], p[2], p[3]) }
      node.power = rs.signal_at(level, p[1], p[2], p[3])
      nodes[k] = node
      order[#order + 1] = node
      for _, l in ipairs(node.links) do queue[#queue + 1] = l end
    end
  end

  -- strongest first: a wire takes the best neighbour minus one, so one sweep per power level is enough
  local changed = true
  while changed do
    changed = false
    for _, n in ipairs(order) do
      for _, l in ipairs(n.links) do
        local m = nodes[key(l[1], l[2], l[3])]
        if m and m.power - 1 > n.power then n.power = m.power - 1; changed = true end
      end
    end
  end

  for _, n in ipairs(order) do
    if n.power ~= n.old then
      rs.set_wire_power(level, n.x, n.y, n.z, n.power)
      rs.update_wire_neighbors(level, n.x, n.y, n.z)
      for _, d in ipairs({ {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, {0, -1, 0} }) do
        rs.update_wire_neighbors(level, n.x + d[1], n.y + d[2], n.z + d[3])
      end
    end
  end
end)
