# urxvt SGR compatibility relay

This is an **unpatched-urxvt workaround**, not a Perl extension. The urxvt Perl API exposes screen-line/rendering hooks, but not a hook for raw incoming CSI/SGR sequences. A Perl extension therefore cannot reliably tell whether a cell was written under SGR 2, 8, or 9, or when the corresponding reset arrives.

`urxvt-sgr-proxy` runs the user's shell (or another command) on a nested PTY and filters the application's output before urxvt parses it. It uses only Python 3's standard library; `xrdb` is optional.

## What is emulated

* `CSI 2 m` (faint): blend the foreground 50% towards the current background. The blend is quantized to the nearest xterm 256-color entry by default, which suits ordinary 256-color urxvt builds.
* `CSI 8 m` (conceal): set the effective foreground to the current displayed background. When that target is the terminal's default color, the relay resolves it to the configured RGB value before emitting a concrete color, rather than using `39`/`49` (which mean the default color of the output channel and can reveal white text on a black background). The effect is recalculated when foreground/background colors or reverse-video change.
* `CSI 9 m` (crossed out): append Unicode U+0336 COMBINING LONG STROKE OVERLAY to printable characters while strike-through is active. This gives an actual strike-through visual in fonts that support the combining mark, without disturbing ordinary SGR underline state. It remains a text-level approximation, not a native per-cell rendition bit; complex emoji/grapheme clusters and fonts without this glyph can render imperfectly.

The relevant resets are handled: `22` resets faint (and is passed through to reset bold too), `28` reveals text, `29` stops adding strike overlays, and `0` resets all tracked state. The relay also handles `39`, `49`, `7`, `27`, ordinary ANSI colors and `38/48;5` or `38/48;2` color changes while one of the emulated attributes is active.

Faint uses the foreground/background values from X resources `URxvt.foreground`, `URxvt.background`, and `URxvt.color0` through `URxvt.color15` when `xrdb` is available. Override any of these with environment variables `URXVT_SGR_DEFAULT_FG`, `URXVT_SGR_DEFAULT_BG`, and `URXVT_SGR_COLOR0` ... `URXVT_SGR_COLOR15` (hex colors such as `#c0c0c0`). Set `URXVT_SGR_TRUECOLOR=1` to emit RGB overrides rather than nearest-256 palette colors, if your urxvt build handles direct-color SGR the way you want.

## Install / run

```sh
install -Dm755 urxvt-sgr-proxy ~/.local/bin/urxvt-sgr-proxy
urxvt -e ~/.local/bin/urxvt-sgr-proxy
```

With arguments, the relay runs that command instead of the login shell:

```sh
urxvt -e ~/.local/bin/urxvt-sgr-proxy -- bash -l
```

Use it as the command in your terminal launcher if you want all interactive output (including TUI applications) filtered. It has a real PTY behind the wrapper and relays keyboard input, terminal replies, window-size changes, and exit status. There is no need to change the urxvt executable. The extra PTY layer may still expose compatibility issues with unusual applications that inspect process/session topology or depend on terminal-specific ioctl behavior.

## Optional terminfo entry

The terminfo entry makes `dim`, `invis`, and the ncurses `smxx`/`rmxx` strikeout capabilities available to terminfo-aware applications. It inherits `rxvt-unicode-256color`; change that parent to `rxvt-unicode` if your installation has no 256-color entry (faint blending will then be less accurate).

```sh
 tic -x -o ~/.terminfo urxvt-sgr.ti
```

The relay sets `TERM=urxvt-sgr` only if `infocmp -x urxvt-sgr` finds the installed entry; otherwise it preserves the inherited `TERM`. You can override the entry name with `URXVT_SGR_TERM`.

Terminfo alone does **not** add rendering support to urxvt; it only tells applications which sequences to send. The relay is what translates those sequences into styles urxvt can display.

An optional old-style termcap fragment is also provided in `urxvt-sgr.termcap`. It advertises the traditional `mh` (dim) and `mk` (invisible) capabilities. There is no portable two-character termcap capability for strikeout; `smxx`/`rmxx` are ncurses terminfo extensions, so cross-out advertisement is only available through the terminfo entry.

## Sanity check

Run this inside the wrapped terminal:

```sh
printf 'normal | faint: \033[2mfaint\033[22m | concealed: \033[8mhidden\033[28m | crossed: \033[9mstruck\033[29m\n'
```

Expected: “faint” is less intense, “hidden” blends into its background, and “struck” has a stroke through each printable character. For resets, try the sequences independently as well as in combined SGR lists. Testing against your exact urxvt build and X resources is recommended before making it the default launcher.
