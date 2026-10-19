
PREFIX = /usr/local
INSTALL_WRAPPER =

.PHONY: default
default:
	$(error try `make ippdump´ and `make install INSTALL_WRAPPER=sudo´)

ippdump: ippdump.go
	go get
	go build -mod=readonly -o $@ $<

.PHONY: install
install: $(PREFIX)/bin/ippdump

$(PREFIX)/bin/ippdump: ippdump
	$(INSTALL_WRAPPER) install $< $@

.PHONY: uninstall
uninstall:
	[ ! -e $(PREFIX)/bin/ippdump ] || rm $(PREFIX)/bin/ippdump
