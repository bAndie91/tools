
TARGET_DIR = /usr/tool
LIBDIR = /usr/lib/tool
OLDINCLUDEDIR = /usr/include/tool

TOOLS = grandparentexe relpath mksock remove syncfs utmp usbreset \
  chmod-cheap setgroups kill-rogue xidle logbtmp lastcsv wall \
  loadenv setpgrp xdpys cdstatus getready runas

SPECIAL_COMPILE_TOOLS = logbtmp lastcsv chmod-cheap wall
SPECIAL_INSTALL_TOOLS = grandparentexe netbulk lastbcsv

NORMAL_COMPILE_TOOLS = $(filter-out $(SPECIAL_COMPILE_TOOLS),$(TOOLS))

NORMAL_TARGET_FILES = $(foreach tool,$(filter-out $(SPECIAL_INSTALL_TOOLS),$(TOOLS)),$(TARGET_DIR)/$(tool))
SPECIAL_TARGET_FILES = $(foreach tool,$(SPECIAL_INSTALL_TOOLS),$(TARGET_DIR)/$(tool))
POD_FILES = $(filter $(wildcard *.pod),$(foreach tool,$(TOOLS),$(tool).pod))



.PHONY: default
default:
	$(error may be interested in: all install-all install-manpages)
.PHONY: all
all: $(TOOLS) netbulk lastbcsv
.PHONY: install-all
install-all: $(NORMAL_TARGET_FILES) $(SPECIAL_TARGET_FILES) install-manpages /sbin/suid-reboot


define install-tool-recipe
	install $< $@
	@echo remove $@ >> uninstall.sh
endef

define make-subdir-tool-recipe:
	$(MAKE) -C $(dir $@) $(notdir $@)
endef



$(NORMAL_COMPILE_TOOLS): %: %.c
	$(CC) $< $(CFLAGS) -o $@


$(NORMAL_TARGET_FILES): $(TARGET_DIR)/%: %
	$(install-tool-recipe)

$(TARGET_DIR)/grandparentexe: grandparentexe
	$(install-tool-recipe)
	chmod g+s $@

$(TARGET_DIR)/netbulk: netbulk
	$(install-tool-recipe)
	setcap cap_setgid+ep $(TARGET_DIR)/netbulk

$(TARGET_DIR)/lastbcsv: $(TARGET_DIR)/lastcsv
	ln -sfT lastcsv $@
	@echo remove $@ >> uninstall.sh

$(TARGET_DIR)/kill-rogue: /usr/share/man/man1/kill-rogue.1.gz

/usr/share/man/man1/kill-rogue.1.gz:
	cat ../doc/kill-rogue.1 | gzip > $@
	@echo remove $@ >> uninstall.sh


netbulk: setgroups.c
	$(CC) $< $(CFLAGS) -DONLY_ALLOWED_GID=$(shell getent group netbulk | cut -d: -f3) -o $@

suid-reboot: suid-command.c
	gcc $< $(CFLAGS) -o $@

lastbcsv: lastcsv
	ln -sfT lastcsv $@

xidle xdpys: override CFLAGS += -lX11 -lXss

loadenv: override CFLAGS += -I$(OLDINCLUDEDIR) -L$(LIBDIR) -larray -Wl,-rpath=$(LIBDIR)



logbtmp: logbtmp.d/logbtmp
	install $< $@
logbtmp.d/logbtmp:
	$(make-subdir-tool-recipe)

lastcsv: lastcsv.d/lastcsv
	install $< $@
lastcsv.d/lastcsv:
	$(make-subdir-tool-recipe)

chmod-cheap: coreutils/chmod-cheap
	install $< $@
coreutils/chmod-cheap:
	$(make-subdir-tool-recipe)

wall: wall.d/wall
	install $< $@
wall.d/wall:
	$(make-subdir-tool-recipe)






MANPAGE_SECTION = 1
COMPRESSOR = xz
COMPRESSOR_EXT = xz
MANPAGE_FILE_SUFFIX = .$(MANPAGE_SECTION).$(COMPRESSOR_EXT)
SYSTEM_MANPAGES_PATH = /usr/share/man/man$(MANPAGE_SECTION)
SPECIAL_MANPAGES = relpath
MANPAGE_FILES = $(foreach podfile,$(filter-out $(SPECIAL_MANPAGES),$(POD_FILES)),$(SYSTEM_MANPAGES_PATH)/$(basename $(podfile))$(MANPAGE_FILE_SUFFIX))

$(MANPAGE_FILES): $(SYSTEM_MANPAGES_PATH)/%$(MANPAGE_FILE_SUFFIX): %.pod
	pod2man --name="$(basename $<)" --section $(MANPAGE_SECTION) --utf8 "$<" | $(COMPRESSOR) > "$@~" && mv "$@~" "$@"
	@echo remove $@ >> uninstall.sh

$(SYSTEM_MANPAGES_PATH)/lastbcsv$(MANPAGE_FILE_SUFFIX): lastcsv.pod
	ln -sfn lastcsv "$@"
	@echo remove $@ >> uninstall.sh

$(SYSTEM_MANPAGES_PATH)/relpath.1tool.$(COMPRESSOR_EXT): relpath.pod
	pod2man --name="$(basename $<)" --section $(MANPAGE_SECTION) --utf8 "$<" | $(COMPRESSOR) > "$@~" && mv "$@~" "$@"
	@echo remove $@ >> uninstall.sh

install-manpages: $(MANPAGE_FILES) $(SYSTEM_MANPAGES_PATH)/lastbcsv$(MANPAGE_FILE_SUFFIX) $(SYSTEM_MANPAGES_PATH)/relpath.1tool.$(COMPRESSOR_EXT)
	/etc/cron.daily/man-db

.PHONY: install-manpages




/sbin/suid-reboot: suid-reboot
	install -o root -g root -m 4444 $< $@
	setfacl -m u:reboot:r-x $@
