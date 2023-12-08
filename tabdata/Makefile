
BIN_DIR = /usr/tool
LIB_DIR = /usr/lib/tool/perl5/tabdata

TOOLS = td-select td-filter td-trans td-trans-ls td2mrkv td-keepheader \
  td-trans-mount td-alter td-add-headers td-pivot td-group td2html mrkv2td

TARGET_TOOLS = $(foreach filename,$(TOOLS),$(BIN_DIR)/$(filename))


install-all: $(TARGET_TOOLS) $(LIB_DIR)/common.pl install-manpages


$(TARGET_TOOLS): $(BIN_DIR)/%: %
	install $(notdir $@) $(BIN_DIR)/
	@echo remove $@ >> uninstall.sh

$(LIB_DIR)/common.pl: common.pl $(LIB_DIR)
	install $(notdir $@) $(LIB_DIR)/
	@echo remove $@ >> uninstall.sh

$(LIB_DIR):
	mkdir -p $@
	@echo remove $@ >> uninstall.sh


REPO_ROOT = ..

include ../user-tools/Makefile.documentation.mk
