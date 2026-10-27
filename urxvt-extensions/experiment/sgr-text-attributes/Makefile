
INSTALL_WRAPPER =
PREFIX = /usr
ETCPREFIX = /etc


.PHONY: default
default:
	$(error try `make install INSTALL_WRAPPER=sudo PREFIX=/usr/local ETCPREFIX=/usr/local/etc´)

.PHONY: install
install: install-terminfo $(PREFIX)/bin/urxvt-sgr-compat $(PREFIX)/bin/urxvt-sgr-proxy desktop-entry alternatives

.PHONY: install-terminfo
install-terminfo: urxvt-sgr.ti
	$(INSTALL_WRAPPER) tic -x -o $(ETCPREFIX)/terminfo -s $<

$(PREFIX)/bin/urxvt-sgr-compat $(PREFIX)/bin/urxvt-sgr-proxy: $(PREFIX)/bin/%: %
	$(INSTALL_WRAPPER) install $< $@

.PHONY: desktop-entry
desktop-entry: $(PREFIX)/share/applications/urxvt-sgr-compat.desktop
	$(INSTALL_WRAPPER) update-desktop-database

$(PREFIX)/share/applications/urxvt-sgr-compat.desktop: urxvt-sgr-compat.desktop
	$(INSTALL_WRAPPER) cp --no-preserve=ownership $< $@

.PHONY: alternatives
alternatives:
	$(INSTALL_WRAPPER) update-alternatives \
		--install /usr/bin/x-terminal-emulator x-terminal-emulator $(PREFIX)/bin/urxvt-sgr-compat 20 \
		--slave /usr/share/man/man1/x-terminal-emulator.1.gz x-terminal-emulator.1.gz /usr/share/man/man1/urxvt.1.gz
