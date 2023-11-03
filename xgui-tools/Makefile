
PREFIX = /usr/tool

default:
	@echo may be interested in: install-all
	false

TOOLS = gexifcomment gtail xgcal perspect

TARGET_FILES = $(foreach tool,$(TOOLS),$(PREFIX)/$(tool))
UNINSTALL_TARGETS = $(foreach tool,$(TOOLS),UNINSTALL/$(tool))
SHELL = bash -e

define rid
  @[ ! -e $(1) ] || rm -v $(1)
endef


install-all: $(TARGET_FILES)
.PHONY: install-all

uninstall-all: $(UNINSTALL_TARGETS)
.PHONY: uninstall-all

$(UNINSTALL_TARGETS):
	$(call rid,$(PREFIX)/$(notdir $@))
.PHONY: $(UNINSTALL_TOOLS)

UNINSTALL/xgcal:
	$(call rid,/usr/share/xgcal/timezones.png)
	$(call rid,/usr/share/xgcal)
	$(call rid,$(PREFIX)/xgcal)

$(PREFIX)/xgcal: DEPENDENCY/sunclock /usr/share/xgcal/timezones.png

DEPENDENCY/sunclock:
	type sunclock
.PHONY: DEPENDENCY/sunclock

/usr/share/xgcal/timezones.png: timezones.png | /usr/share/xgcal
	install $< $@

/usr/share/xgcal:
	mkdir -p $@

$(TARGET_FILES): $(PREFIX)/%: %
	install $< $@
