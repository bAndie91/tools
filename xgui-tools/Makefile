
PREFIX = /usr/tool

default:
	@echo may be interested in: install-all
	false

TOOLS = gexifcomment gtail xgcal

TARGET_FILES = $(foreach tool,$(TOOLS),$(PREFIX)/$(tool))
UNINSTALL_TARGETS = $(foreach tool,$(TOOLS),UNINSTALL/$(tool))
SHELL = bash -e



install-all: $(TARGET_FILES)
.PHONY: install-all

uninstall-all: $(UNINSTALL_TARGETS)
.PHONY: uninstall-all

$(UNINSTALL_TARGETS):
	[ ! -e $(PREFIX)/$(notdir $@) ] || rm $(PREFIX)/$(notdir $@)
.PHONY: $(UNINSTALL_TOOLS)



$(PREFIX)/xgcal: DEPENDENCY/sunclock /usr/share/xgcal/timezones.png

DEPENDENCY/sunclock:
	type sunclock
.PHONY: DEPENDENCY/sunclock

/usr/share/xgcal/timezones.png: timezones.png
	install $< $@

$(TARGET_FILES): $(PREFIX)/%: %
	install $< $@
