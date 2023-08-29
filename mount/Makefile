
TOOLS = mount.bindfs mount.posixovlfs mount.xattrovlfs mount.fusefile

TARGETS = $(foreach name,$(TOOLS),/sbin/$(name))

install-all: $(TARGETS)

.PHONY: install-all

$(TARGETS): /sbin/%: %
	install --compare $< $@
	@echo remove $@ >> uninstall.sh
