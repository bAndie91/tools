
MANPAGE_FILES = $(wildcard man*/*.*.xz)

SYSTEM_MANPAGES_PATH = /usr/share/man

SYSTEM_MANPAGES_FILES = $(addprefix $(SYSTEM_MANPAGES_PATH)/,$(MANPAGE_FILES))


install-manpages: $(SYSTEM_MANPAGES_FILES)
	/etc/cron.daily/man-db

$(SYSTEM_MANPAGES_FILES): $(SYSTEM_MANPAGES_PATH)/%: %
	install "$<" /usr/share/man/$<
	@echo remove /usr/share/man/$< >> uninstall.sh
