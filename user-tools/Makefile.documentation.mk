
ifndef TOOLS
$(error TOOLS is not set in Makefile)
endif

MANPAGE_SECTION ?= 1
MANPAGE_SECTION_EXT ?= $(MANPAGE_SECTION)

MANPAGES_DIR = gen-doc
MANPAGES_COMPRESSOR = xz
MANPAGE_FILE_SUFFIX = .$(MANPAGE_SECTION_EXT).$(MANPAGES_COMPRESSOR)
MANPAGES_SUBDIR = $(MANPAGES_DIR)/man$(MANPAGE_SECTION)
MANPAGE_FILENAMES = $(foreach toolname,$(TOOLS),$(toolname)$(MANPAGE_FILE_SUFFIX))
MANPAGE_FILES = $(foreach basename,$(MANPAGE_FILENAMES),$(MANPAGES_SUBDIR)/$(basename))

INSTALL_MANPAGES_PATH = /usr/share/man/man$(MANPAGE_SECTION)
INSTALL_MANPAGES_FILES = $(addprefix $(INSTALL_MANPAGES_PATH)/,$(MANPAGE_FILENAMES))


.PHONY: manpages
manpages: $(MANPAGE_FILES)

$(MANPAGE_FILES): | $(MANPAGES_SUBDIR)

$(MANPAGES_SUBDIR):
	mkdir -p $@

$(MANPAGE_FILES): $(MANPAGES_SUBDIR)/%$(MANPAGE_FILE_SUFFIX): %
	@if podchecker "$<"; then \
		pod2man --name="$<" --section $(MANPAGE_SECTION_EXT) --utf8 "$<" | $(MANPAGES_COMPRESSOR) > "$@~" &&\
		mv -f "$@~" "$@" ;\
	else \
		true > "$@" ;\
	fi


.PHONY: install-manpages
install-manpages: $(INSTALL_MANPAGES_FILES) .stamp.update-man-db

.stamp.update-man-db: $(INSTALL_MANPAGES_FILES)
	if [ "$?" ]; then $(INSTALL_WRAPPER) /etc/cron.daily/man-db; touch $@; fi


$(INSTALL_MANPAGES_FILES): $(INSTALL_MANPAGES_PATH)/%: $(MANPAGES_SUBDIR)/%
	$(INSTALL_WRAPPER) install "$<" "$@"
	@echo remove $@ >> uninstall.sh



descriptions.txt: $(TOOLS)
	perl -ne 'if(/^=head1 NAME/){ <>; print while $$_=<>  and !/^\s*$$/; }' -- $(sort $(TOOLS)) | uniq > $@~
	mv -f $@~ $@
