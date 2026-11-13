-- drawing in the world with the frame's canvas: flat triangles, textured triangles and a picture as a parallelogram, at world coordinates
local picture

-- one corner of a triangle: x y z, u v, red green blue alpha, as the little-endian floats the canvas reads
local function corner(x, y, z, u, v, r, g, b)
  return string.pack("<fffffffff", x, y, z, u, v, r, g, b, 1.0)
end

mod.hook("GameRenderer_renderWeather", function(call)
  local canvas = call.world
  if not picture then
    picture = surface.create(2, 2)
    picture:write(0, 0, 2, 2, string.rep("\0\255\0\255", 4))
  end
  canvas:colored_triangles(corner(257, 121, 243, 0, 0, 1, 0, 0) .. corner(259, 121, 243, 0, 0, 1, 0, 0) .. corner(259, 123, 243, 0, 0, 1, 0, 0))
  canvas:triangles(picture, corner(260, 121, 243, 0, 0, 1, 1, 1) .. corner(262, 121, 243, 1, 0, 1, 1, 1) .. corner(262, 123, 243, 1, 1, 1, 1, 1))
  canvas:image(picture, 263, 121, 243, 2, 0, 0, 0, 2, 0, 15)
end)
