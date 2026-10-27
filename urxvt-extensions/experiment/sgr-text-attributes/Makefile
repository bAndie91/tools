
INSTALL_WRAPPER =
PREFIX = /usr
ETCPREFIX = /etc


.PHONY: default
default:
	$(error try `make install INSTALL_WRAPPER=sudo PREFIX=/usr/local ETCPREFIX=/usr/local/etc´)

.PHONY: install
install: install-terminfo $(PREFIX)/bin/urxvt-sgr-proxy

.PHONY: install-terminfo
install-terminfo: urxvt-sgr.ti
	$(INSTALL_WRAPPER) tic -x -o $(ETCPREFIX)/terminfo -s $<

$(PREFIX)/bin/urxvt-sgr-proxy: urxvt-sgr-proxy
	$(INSTALL_WRAPPER) install $< $@
