# GUI tools

## gExifComment

## Perspect – picture perspection correction utility

What does this program do?

- display the image file given in cli argument in a window
- let you select 4 coordinates with left mouse button
- you can move an already pinned point by clicking again so then the closes point will be moved over
- remove a pin point by right-clicking nearby
- middle-click redraws the helper quadrangle

### KEYS

- `F1` - perspective correction (distort), then open resulting image
- `Shift-F1` - perspective correction (distort) **and crop**, then open resulting image
- `F2` - crop around on bounding box, then open

- `F3` - rotate the image so the drawn line segment (pin down 2 points, not 3 or 4) to be horizontal
- `F4` - rotate the image so the drawn line segment (pin down 2 points, not 3 or 4) to be vertical
- `Shift-F3`, `Shift-F4` - just like `F3`/`F4` plus auto-crop

- `F5` - save marked coordinates to `~/.perspect.coords.log`
- `F6` - restore coordinates from file
- `Q` - toggle quadrangle marked by coordinates
- `R` - toggle quadrangle's (axis-aligned) bounding rectangle

- `Ctrl-S` - "Save As…"
- `Ctrl-O` - open by mimeopen-gui
- `ESC` - close

- `left click` - pin a coordinate
- `right click` - remove closest pinned coordinate
- `middle click` - redraw
- `alt+drag` - move window (if WM supports)

### HINTS

images are not scaled, but displayed at the original size, so
a window manager which supports Alt-dragging and window regions being out of the screen
is recommended to work on large images.

### REQUIREMENTS

- gtk-2, pygtk
- imagemagick, convert(1) is called internally
- imagemetadata python module

## gMenu

## gTail

## xCred

## xgCal

## xStopper
