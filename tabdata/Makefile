
BIN_DIR = /usr/tool
LIB_DIR = /usr/lib/tool/perl5/tabdata

TOOLS = \
  csv2td \
  kvpairs2td \
  mrkv2td \
  td2html \
  td2kvpairs \
  td2mrkv \
  td-add-headers \
  td-alter \
  td-collapse \
  td-disamb-headers \
  td-env \
  td-expand \
  td-filter \
  td-format \
  td-gnuplot \
  td-keepheader \
  td-lpstat \
  td-ls \
  td-pivot \
  td-ps \
  td-rename \
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

.PHONY: list-tools
list-tools:
	@echo $(TOOLS) | tr " " "\n" | sort


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
	perl -MMath::Cartesian::Product -e 1

.PHONY: try-satisfy-dependencies
try-satisfy-dependencies:
	apt install libswitch-perl liblist-moreutils-perl libtext-csv-perl libmath-cartesian-product-perl


REPO_ROOT = ..

include ../user-tools/Makefile.documentation.mk


index.md: SHELL = bash
index.md: $(TOOLS)
	echo -ne "# Tabdata commands\n\n" > $@~
	for file in $(TOOLS); do echo -ne "- [$$file](#$$file)\n"; done >> $@~
	for file in $(TOOLS); do \
		echo -ne "\n" ;\
		echo -ne "# $$file\n\n" ;\
		pod2markdown --utf8 < "$$file" |\
			perl -pe 's/^#/##/; s/\b([a-z0-9.-]+)(\(\d)/[$$1](#$$1)$$2/g' ;\
	done >> $@~
	mv $@~ $@
