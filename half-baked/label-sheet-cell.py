#!/usr/bin/env python3

import argparse
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import A4
from reportlab.lib.units import mm


def mm_to_pt(mm_val):
    return mm_val * mm

def render_text_in_cell(text, col, row, cell_width_mm, cell_height_mm, h_margin_mm, v_margin_mm, font_size):
    page_width_pt, page_height_pt = A4
    cell_width_pt = mm_to_pt(cell_width_mm)
    cell_height_pt = mm_to_pt(cell_height_mm)
    h_margin_pt = mm_to_pt(h_margin_mm)
    v_margin_pt = mm_to_pt(v_margin_mm)
    padding_pt = 4  # small padding inside the cell

    c = canvas.Canvas("/dev/stdout", pagesize=A4)
    c.setFont("Helvetica", font_size)
	
    # Calculate origin of the cell (from bottom-left)
    x = (col - 1) * (cell_width_pt + h_margin_pt) + padding_pt
    y = page_height_pt - (row * (cell_height_pt + v_margin_pt)) + padding_pt
	
    usable_width = cell_width_pt - 2 * padding_pt
    usable_height = cell_height_pt - 2 * padding_pt
	
    # Build line-by-line filling of the cell
    line_height = font_size * 1.2
	
    # Build a string long enough to overflow a line, repeat
    repeated_text = text
    while c.stringWidth(repeated_text + " " + text, "Helvetica", font_size) < usable_width:
        repeated_text += " " + text
	
    # Fill lines within the cell height
    max_lines = int(usable_height // line_height)
    for i in range(max_lines):
        c.drawString(x, y + i * line_height, repeated_text)
	
    c.save()


def parse_integer_pair(s):
	try:
		return [float(n) for n in s.split(',')]
	except:
		raise argparse.ArgumentTypeError("Must be in format like \"1,2\"")

parser = argparse.ArgumentParser(description="Generate a PDF with repeated text inside a grid cell. Useful for printable sticky-labels sheet.", add_help=True, allow_abbrev=True)
parser.add_argument("text", help="Text to repeat on the label")
parser.add_argument("--cell", type=parse_integer_pair, required=True, help="Column,row coordinates (1-based)")
parser.add_argument("--cell-size", type=parse_integer_pair, required=True, help="Cell width,height in mm")
parser.add_argument("--margin", type=float, default=2, help="Spacing between cells in mm")
parser.add_argument("--margin-horizontal", type=float, help="Override horizontal cell spacing")
parser.add_argument("--margin-vertical", type=float, help="Override vertical cell spacing")
parser.add_argument("--font-size", type=int, default=12, help="Font size in points")
opt = parser.parse_args()

if opt.margin_horizontal is None: opt.margin_horizontal = opt.margin
if opt.margin_vertical is None: opt.margin_vertical = opt.margin

render_text_in_cell(opt.text, opt.cell[0], opt.cell[1], opt.cell_size[0], opt.cell_size[1], opt.margin_horizontal, opt.margin_vertical, opt.font_size)
