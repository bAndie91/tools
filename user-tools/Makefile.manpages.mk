
POD_TOOLS = $(file < tools-with-pod)

MANPAGE_SECTION ?= 1
MANPAGE_SECTION_EXT ?= $(MANPAGE_SECTION)
MANPAGES_COMPRESSOR ?= xz

MANPAGES_DIR = gen-doc
MANPAGES_SUBDIR = $(MANPAGES_DIR)/man$(MANPAGE_SECTION)
MANPAGE_FILE_SUFFIX = .$(MANPAGE_SECTION_EXT).$(MANPAGES_COMPRESSOR)
MANPAGE_FILENAMES = $(foreach toolname,$(POD_TOOLS),$(toolname)$(MANPAGE_FILE_SUFFIX))
MANPAGE_FILEPATHS = $(foreach filename,$(MANPAGE_FILENAMES),$(MANPAGES_SUBDIR)/$(filename))

INSTALL_MANPAGES_PATH = /usr/share/man/man$(MANPAGE_SECTION)
INSTALL_MANPAGES_FILES = $(addprefix $(INSTALL_MANPAGES_PATH)/,$(MANPAGE_FILENAMES))


.PHONY: manpages
manpages: $(MANPAGE_FILEPATHS)

$(MANPAGE_FILEPATHS): | $(MANPAGES_SUBDIR)

$(MANPAGES_SUBDIR):
	mkdir -p $@


$(MANPAGE_FILEPATHS): $(MANPAGES_SUBDIR)/%$(MANPAGE_FILE_SUFFIX): %
	pod2man --name="$<" --section $(MANPAGE_SECTION_EXT) --utf8 "$<" | $(MANPAGES_COMPRESSOR) > "$@~"
	mv -f "$@~" "$@"



.PHONY: install-manpages
install-manpages: $(INSTALL_MANPAGES_FILES) .stamp.update-man-db

.stamp.update-man-db: $(INSTALL_MANPAGES_FILES)
	if [ "$?" ]; then $(INSTALL_WRAPPER) /etc/cron.daily/man-db; touch $@; fi


$(INSTALL_MANPAGES_FILES): $(INSTALL_MANPAGES_PATH)/%: $(MANPAGES_SUBDIR)/%
	$(INSTALL_WRAPPER) install "$<" "$@"
	@echo remove $@ >> uninstall.sh
