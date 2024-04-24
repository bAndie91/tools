
default:
	false

install-tablet-rotation-watcher: /etc/daemons/tablet-rotation-watcher
	TODO
.PHONY: install-tablet-rotation-watcher

/etc/daemons/tablet-rotation-watcher: tablet-rotation-watcher
	install --compare $< $@

uninstall-tablet-rotation-watcher:
	[ -e /etc/daemons/tablet-rotation-watcher ] || rm /etc/daemons/tablet-rotation-watcher
.PHONY: uninstall-tablet-rotation-watcher
