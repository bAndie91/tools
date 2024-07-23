
BIN_DIR = /usr/tool
LIB_DIR = /usr/lib/tool/perl5/tabdata

TOOLS = \
  csv2td \
  mrkv2td \
  td2html \
  td2mrkv \
  td-add-headers \
  td-alter \
  td-breakup \
  td-disamb-headers \
  td-filter \
  td-group \
  td-keepheader \
  td-ls \
  td-pivot \
  td-select \
  td-sort \
  td-trans \
  td-trans-fixcol \
  td-trans-group \
  td-trans-gshadow \
  td-trans-ls \
  td-trans-mount \
  td-trans-passwd \
  td-trans-shadow \
  vcf2td \


TARGET_TOOLS = $(foreach filename,$(TOOLS),$(BIN_DIR)/$(filename))


install-all: depcheck $(TARGET_TOOLS) $(LIB_DIR)/common.pl install-manpages


$(TARGET_TOOLS): $(BIN_DIR)/%: %
	install $(notdir $@) $(BIN_DIR)/
	@echo remove $@ >> uninstall.sh

$(LIB_DIR)/common.pl: common.pl $(LIB_DIR)
	install $(notdir $@) $(LIB_DIR)/
	@echo remove $@ >> uninstall.sh

$(LIB_DIR):
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

.PHONY: depcheck
depcheck:
	$(info Checking dependencies...)
	perl -MSwitch -e 1
	perl -MList::MoreUtils -e 1
	perl -MPod::Usage -e 1
	perl -MMIME::QuotedPrint -e 1
	perl -MEncode -e 1
	perl -MText::CSV -e 1

.PHONY: try-satisfy-dependencies
try-satisfy-dependencies:
	apt install libswitch-perl liblist-moreutils-perl libtext-csv-perl


REPO_ROOT = ..

include ../user-tools/Makefile.documentation.mk
