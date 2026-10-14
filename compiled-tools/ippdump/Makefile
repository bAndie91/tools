
PREFIX = /usr/local

.PHONY: default
default:
	$(error try `make install INSTALL_WRAPPER=sudo´)

ippdump: ippdump.go
	go mod init ippdump
	go get github.com/OpenPrinting/goipp
	go build -o $@ $<

.PHONY: install
install: $(PREFIX)/bin/ippdump

$(PREFIX)/bin/ippdump: ippdump
	$(INSTALL_WRAPPER) install $< $@

.PHONY: uninstall
uninstall:
	[ ! -e $(PREFIX)/bin/ippdump ] || rm $(PREFIX)/bin/ippdump
