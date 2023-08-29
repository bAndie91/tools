
MANPAGE_FILES = $(foreach file,$(wildcard man*/*.*.xz),$(shell test -s $(file) && echo $(file)))

SYSTEM_MANPAGES_PATH = /usr/share/man

SYSTEM_MANPAGES_FILES = $(addprefix $(SYSTEM_MANPAGES_PATH)/,$(MANPAGE_FILES))


default:
	@echo suggestions: install-manpages
	@false

list-manpages:
	@echo $(MANPAGE_FILES) | tr " " "\n" | sort

install-manpages: $(SYSTEM_MANPAGES_FILES)
	/etc/cron.daily/man-db

$(SYSTEM_MANPAGES_FILES): $(SYSTEM_MANPAGES_PATH)/%: %
	install "$<" /usr/share/man/$<
	@echo remove /usr/share/man/$< >> uninstall.sh
