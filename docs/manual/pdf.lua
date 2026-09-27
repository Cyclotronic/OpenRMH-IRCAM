-- SPDX-License-Identifier: MIT
-- Copyright (c) 2026 Cyclotronic
--
-- Pandoc filter used when building the PDF. manual.md places images as <img src="..." width="PX"> so that GitHub
-- renders them at a sensible size; this turns each one into a pandoc image with the same size in inches (96 px/in),
-- capped at the text width of the page set in metadata.yaml.

local TEXT_WIDTH_IN = 10.1   -- A4 landscape (11.69 in) less 2 cm side margins

local function to_image(tag)
  local src = tag:match('src="([^"]+)"')
  if not src then return nil end
  local attr = {}
  local px = tonumber(tag:match('width="(%d+)"') or "")
  if px then
    attr.width = string.format("%.2fin", math.min(px / 96, TEXT_WIDTH_IN))
  end
  return pandoc.Image({}, src, "", attr)
end

function RawInline(el)
  if el.format == "html" and el.text:match("^<img") then
    return to_image(el.text)
  end
end

-- Tables: text columns left-aligned (they would otherwise inherit the figure's centring), and a first row that holds
-- images moved back into the body. Word tables without a heading row arrive with their first data row as the header,
-- which keeps the whole table from starting at the bottom of a page.
function Table(tbl)
  for _, spec in ipairs(tbl.colspecs) do
    if spec[1] == "AlignDefault" then spec[1] = "AlignLeft" end
  end
  local head_has_image = false
  for _, row in ipairs(tbl.head.rows) do
    for _, cell in ipairs(row.cells) do
      pandoc.Div(cell.contents):walk({ Image = function() head_has_image = true end })
    end
  end
  if head_has_image and #tbl.bodies > 0 then
    local rows = tbl.head.rows
    for i = #rows, 1, -1 do table.insert(tbl.bodies[1].body, 1, rows[i]) end
    tbl.head.rows = {}
  end
  return tbl
end

function RawBlock(el)
  if el.format == "html" and el.text:match("<img") then
    local inlines = {}
    for tag in el.text:gmatch("<img[^>]*>") do
      local img = to_image(tag)
      if img then
        if #inlines > 0 then table.insert(inlines, pandoc.Space()) end
        table.insert(inlines, img)
      end
    end
    if #inlines > 0 then return pandoc.Para(inlines) end
  end
end
