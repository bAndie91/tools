
TARGET_DIR = /usr/tool

TOOLS = grandparentexe lrurs mksock remove syncfs utmp usbreset \
  chmod-cheap setgroups kill-rogue xidle logbtmp lastcsv

SPECIAL_INSTALL_TOOLS = grandparentexe netbulk

NORMAL_TARGET_FILES = $(foreach tool,$(filter-out $(SPECIAL_INSTALL_TOOLS),$(TOOLS)),$(TARGET_DIR)/$(tool))

SPECIAL_TARGET_FILES = $(foreach tool,$(SPECIAL_INSTALL_TOOLS),$(TARGET_DIR)/$(tool))


all: $(TOOLS)

install-all: $(NORMAL_TARGET_FILES) $(SPECIAL_TARGET_FILES)

define install-tool-recipe
	install $< $@
	@echo remove $@ >> uninstall.sh
endef

$(NORMAL_TARGET_FILES): $(TARGET_DIR)/%: %
	$(install-tool-recipe)

netbulk:
	$(CC) -DONLY_ALLOWED_GID=$(shell getent group netbulk | cut -d: -f3) setgroups.c -o $@

xidle: CFLAGS += -lX11 -lXss

$(TARGET_DIR)/grandparentexe: grandparentexe
	$(install-tool-recipe)
	chmod g+s $@

$(TARGET_DIR)/netbulk: netbulk
	setcap cap_setgid+ep $(TARGET_DIR)/netbulk

$(TARGET_DIR)/kill-rogue: /usr/share/man/man1/kill-rogue.1.gz

/usr/share/man/man1/kill-rogue.1.gz:
	cat ../doc/kill-rogue.1 | gzip > $@
	@echo remove $@ >> uninstall.sh


logbtmp: logbtmp.d/logbtmp
	install $< $@

logbtmp.d/logbtmp:
	$(MAKE) -C $(dir $@) $(notdir $@)

lastcsv: lastcsv.d/lastcsv
	install $< $@

lastcsv.d/lastcsv:
	$(MAKE) -C $(dir $@) $(notdir $@)
