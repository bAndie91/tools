
TOOLS = mount.bindfs mount.posixovlfs mount.xattrovlfs mount.fusefile \
  mount.restic

TARGETS = $(foreach name,$(TOOLS),/sbin/$(name))


default:
	@echo may be interested in: install-all
	false

install-all: $(TARGETS)
.PHONY: install-all

$(TARGETS): /sbin/%: %
	install --compare $< $@
	@echo remove $@ >> uninstall.sh
