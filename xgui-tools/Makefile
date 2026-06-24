
TOOLS = gexifcomment gtail xgcal xstopper perspect xcred gmenu \
  image-rotate video-rotate

include ../user-tools/Makefile

$(TOOLS_TARGET_DIR)/xgcal: /usr/share/xgcal/timezones.png

DEPCHECK/xgcal:
	type sunclock
	type gcal
	type solartime

/usr/share/xgcal/timezones.png: timezones.png | /usr/share/xgcal
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

/usr/share/xgcal:
	$(INSTALL_WRAPPER) mkdir -p $@
	@$(record_uninstall)


THUNAR_CUSTOM_ACTION_COMMANDS = image-rotate video-rotate
THUNAR_CUSTOM_ACTION_COMMAND_TARGETS = $(foreach name,$(THUNAR_CUSTOM_ACTION_COMMANDS),$(TOOLS_TARGET_DIR)/$(name))

THUNAR_CUSTOM_ACTION_FILES = $(wildcard thunar-custom-actions/*.xml)
THUNAR_UCA_CONFIG_FILE = $(HOME)/.config/Thunar/uca.xml
.PHONY: install-thunar-custom-actions
install-thunar-custom-actions: $(THUNAR_CUSTOM_ACTION_COMMAND_TARGETS)
	$(MAKE) $(THUNAR_UCA_CONFIG_FILE)

$(THUNAR_UCA_CONFIG_FILE): $(THUNAR_CUSTOM_ACTION_FILES)
$(THUNAR_CUSTOM_ACTION_FILES):
	xmlinsert $(THUNAR_UCA_CONFIG_FILE) /actions unique-id < $@

