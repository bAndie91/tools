
TOOLS = gexifcomment gtail xgcal xstopper perspect xcred

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
