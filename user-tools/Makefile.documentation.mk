
ifndef TOOLS
$(error TOOLS is not set in Makefile)
endif

ifndef REPO_ROOT
$(error REPO_ROOT is not set in Makefile)
endif

MANPAGE_SECTION ?= 1
MANPAGE_SECTION_EXT ?= $(MANPAGE_SECTION)

MANPAGES_DIR = $(REPO_ROOT)/doc/man
MANPAGE_FILE_SUFFIX = .$(MANPAGE_SECTION_EXT).xz
MANPAGES_SUBDIR = $(MANPAGES_DIR)/man$(MANPAGE_SECTION)
MANPAGE_FILES = $(foreach toolname,$(TOOLS),$(MANPAGES_SUBDIR)/$(toolname)$(MANPAGE_FILE_SUFFIX))

manpages: $(MANPAGE_FILES)
.PHONY: manpages

$(MANPAGE_FILES): | $(MANPAGES_SUBDIR)

$(MANPAGES_SUBDIR):
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

$(MANPAGE_FILES): $(MANPAGES_SUBDIR)/%$(MANPAGE_FILE_SUFFIX): %
	@if podchecker "$<"; then \
		pod2man --name="$<" --section $(MANPAGE_SECTION_EXT) --utf8 "$<" | xz > "$@.tmp" &&\
		mv -f "$@.tmp" "$@" ;\
	else \
		touch "$@" ;\
	fi

install-manpages: manpages
	$(MAKE) -C $(REPO_ROOT)/doc/man install-manpages
.PHONY: install-manpages


descriptions.txt: $(TOOLS)
	perl -ne 'if(/^=head1 NAME/){ <>; print while $$_=<>  and !/^\s*$$/; }' -- $(TOOLS) | uniq > $@~
	mv -f $@~ $@
