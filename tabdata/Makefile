
BIN_DIR = /usr/tool
LIB_DIR = /usr/lib/tool/perl5/tabdata

TOOLS = \
  mrkv2td \
  td2html \
  td2mrkv \
  td-add-headers \
  td-alter \
  td-disabm-headers \
  td-filter \
  td-group \
  td-keepheader \
  td-pivot \
  td-select \
  td-trans \
  td-trans-fixcol \
  td-trans-ls \
  td-trans-mount \


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
