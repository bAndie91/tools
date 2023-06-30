
TARGET_DIR = /usr/tool

TOOLS = grandparentexe relpath mksock remove syncfs utmp usbreset \
  chmod-cheap setgroups kill-rogue xidle logbtmp lastcsv wall

SPECIAL_INSTALL_TOOLS = grandparentexe netbulk

NORMAL_TARGET_FILES = $(foreach tool,$(filter-out $(SPECIAL_INSTALL_TOOLS),$(TOOLS)),$(TARGET_DIR)/$(tool))
SPECIAL_TARGET_FILES = $(foreach tool,$(SPECIAL_INSTALL_TOOLS),$(TARGET_DIR)/$(tool))



all: $(TOOLS) netbulk
install-all: $(NORMAL_TARGET_FILES) $(SPECIAL_TARGET_FILES)

.PHONY: all install-all

define install-tool-recipe
	install $< $@
	@echo remove $@ >> uninstall.sh
endef

define make-subdir-tool-recipe:
	$(MAKE) -C $(dir $@) $(notdir $@)
endef



$(NORMAL_TARGET_FILES): $(TARGET_DIR)/%: %
	$(install-tool-recipe)

$(TARGET_DIR)/grandparentexe: grandparentexe
	$(install-tool-recipe)
	chmod g+s $@

$(TARGET_DIR)/netbulk: netbulk
	$(install-tool-recipe)
	setcap cap_setgid+ep $(TARGET_DIR)/netbulk


$(TARGET_DIR)/kill-rogue: /usr/share/man/man1/kill-rogue.1.gz

/usr/share/man/man1/kill-rogue.1.gz:
	cat ../doc/kill-rogue.1 | gzip > $@
	@echo remove $@ >> uninstall.sh


netbulk:
	$(CC) -DONLY_ALLOWED_GID=$(shell getent group netbulk | cut -d: -f3) setgroups.c -o $@

xidle: CFLAGS += -lX11 -lXss



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
