
INSTALL_WRAPPER =

BIN_DIR = /usr/tool
REPO_ROOT = ../..

TOOLS = \
  gtabdata-avahi \
  gtabdata-dig \
  gtabdata-docker-ps \
  gtabdata-lpjobs \
  gtabdata-lpstat \
  gtabdata-netstat \

TARGET_TOOLS = $(foreach filename,$(TOOLS),$(BIN_DIR)/$(filename))


.PHONY: install-all
install-all: $(TARGET_TOOLS)

$(TARGET_TOOLS): $(BIN_DIR)/%: %
	$(INSTALL_WRAPPER) install $(notdir $@) $(BIN_DIR)/
	@echo remove $@ >> uninstall.sh


