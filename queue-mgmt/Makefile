
TARGET_DIR = /usr/tool

FILES = qadd qadd-common qls qrm qrun

TARGET_FILES = $(foreach filename,$(FILES),$(TARGET_DIR)/$(filename))


install-all: $(TARGET_FILES)


$(TARGET_FILES): $(TARGET_DIR)/%: %
	install $(notdir $@) $(TARGET_DIR)/
	@echo remove $@ >> uninstall.sh
