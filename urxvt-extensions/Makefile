
default:
	@echo may be interested in: install-all-for-user
	false

PLUGINS = inbandfiletransfer-osc linefeed printscrollback queryinfo-osc selection-popup urgency-on-bell wheelzoom
TARGET_PREFIX = $(HOME)/.urxvt/ext
TARGET_FILES = $(foreach name,$(PLUGINS),$(TARGET_PREFIX)/$(name))

install-all-for-user: $(TARGET_FILES)
.PHONY: install-all-for-user

$(TARGET_FILES): $(TARGET_PREFIX)/%: perl/%
	install perl/$(notdir $@) $@
