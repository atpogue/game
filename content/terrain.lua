local function make_sprite(x, y, color)
  local tilesize = 16.0
  return {
    atlas = 'assets/kenney-1bitpack.png',
    source = { x * tilesize, y * tilesize, tilesize, tilesize},
    color = color
  }
end

terrain 'grass-1' {
  sprite = make_sprite(5, 0, 0x3BD872FF)
}

terrain 'grass-2' {
  sprite = make_sprite(6, 0, 0x43FF64FF)
}

terrain 'grass-3' {
  sprite = make_sprite(7, 0, 0x32DC96FF)
}

terrain 'grass-tall' {
  sprite = make_sprite(0, 2, 0x0C9B64FF)
}

terrain 'dirt' {
  sprite = make_sprite(2, 0, 0x79464bff)
}

terrain 'rocks' {
  sprite = make_sprite(2, 0, 0xcec5b7ff)
}

-- purple: 0x3B0264ff
-- olive: 0x799b64ff
-- purple 2: 0x794664ff
-- peachy: 0xce464bff
-- navy grey: 0x02464bff

