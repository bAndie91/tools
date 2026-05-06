
INSTALL_WRAPPER =

TOOLS = mount.bindfs mount.posixovlfs mount.xattrovlfs mount.fusefile mount.abshelper \
  mount.restic mount.rclone

# TODO: discover usage of /sbin/fs.d and /sbin/fs directories.
TARGETS = $(foreach name,$(TOOLS),/sbin/$(name))


.PHONY: default
default:
	$(error try "make install-all INSTALL_WRAPPER=sudo')

.PHONY: install-all
install-all: $(TARGETS)

$(TARGETS): /sbin/%: %
	$(INSTALL_WRAPPER) install --compare $< $@
	@echo remove $@ >> uninstall.sh
