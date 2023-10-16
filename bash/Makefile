
define mkdir-recipe
	mkdir -p $@
	@echo remove $@ >> uninstall.sh
endef

define install-dep-recipe
	install --compare $< $@
	@echo remove $@ >> uninstall.sh
endef


default:
	@echo may be interested in: bash-command-not-found-handle
	@false

.PHONY: default

bash-command-not-found-handle: /etc/profile.d/bash.d/command-not-found-handle.sh

.PHONY: bash-command-not-found-handle

/etc/profile.d/bash.d/command-not-found-handle.sh: command-not-found-handle.sh | /etc/profile.d/bash.d
	$(install-dep-recipe)

/etc/profile.d/bash.d:
	$(mkdir-recipe)

/usr/lib/tool/ansi-codes: ansi-codes | /usr/lib/tool
	$(install-dep-recipe)

/usr/lib/tool:
	$(mkdir-recipe)
