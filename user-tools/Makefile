
REPO_ROOT ?= ..

TOOLS_TARGET_DIR = /usr/tool

TOOLS ?=   2opml adr2html asterisk-log-separator awk-cut base58 base64url \
  bencode2json cdexec chattr-cow chattr-nocow chromium_cookie_decrypt.py \
  chshebang convert_chromium_cookies_to_netscape.sh corner_time \
  cut.awk daemonctl dataurl2bin debdiff descpids dfbar digasn diu dlnew \
  dump_php_session.php eat errorlevel error_reporting.php fcomplete \
  fc-search-codepoint fdupes-hardlink ff fgat find-by-date findnewestfile \
  findoldestfile fixlogfiledatetime fixRFC822filemtime fmtkv foreach \
  geoip g_filename_to_uri git-checkout-sparse gitdiff grepdatetime Head \
  header.sed indent2tree inisort is_gzip jobsel json2bencode json2msgpack \
  kt LevelDB lines list_screens lnto locale-validator lpset ls2html lsata \
  lsenv mime_extract mkdeb moz_bookmarks msg msgpack2json multithrottler \
  mysql-fix-orphan-privileges.php noacute nocomment nthash.py ocspverify \
  organizebydate palemoon-current-url parsel partial paths2indent \
  perl-repl pipekill PMbwmon PMdirindex PMhexdiff PMnslist PMpwgen \
  PMrecdiff psgetchildpids pyzor-files qrwifi regulate renamemanual repeat \
  reportcmdstatus rfc rsacrypt rsync-semichroot rsysrq screenconsole \
  screen-notify screenreattach slay slayall sockio ssh-agent-finder \
  straceall strip-ansi-seq swap symlinks2dot tabularize Tail takeown taslis \
  tests text2img-dataurl touchx ttinput uchmod unicodestyle upsidedown \
  url_decode url_encode url_encode_bf url-parts vidir-sanitize vifiles \
  waitpid whisper-retention-info wikibot xdg-autostart xml2json \
  levenshtein-distance jaro-metric jaro-winkler-metric loggerexec \
  logrotate-counters logto pdfflop pngmetatext set-sys-path subst_sudo_user \
  terminaltitle timestamper set-xcursor-lock-and-run mkmagnetlink htmlentities

TOOLS_TARGET_FILES = $(addprefix $(TOOLS_TARGET_DIR)/,$(TOOLS))



$(TOOLS_TARGET_DIR)/msg: /etc/bash_completion.d/msg
$(TOOLS_TARGET_DIR)/perl-repl: /usr/libexec/perlshell
$(TOOLS_TARGET_DIR)/rsync-semichroot: /usr/share/doc/tool/rsync-semichroot.txt
$(TOOLS_TARGET_DIR)/multithrottler: /usr/lib/multithrottler/Throttler.pm

/etc/bash_completion.d/msg:
	install $(REPO_ROOT)/bash/bash_completion.d/msg $@
	@echo remove $@ >> uninstall.sh

/usr/libexec/perlshell:
	install $(REPO_ROOT)/libexec/perlshell $@
	@echo remove $@ >> uninstall.sh

/usr/share/doc/tool/rsync-semichroot.txt:
	install $(REPO_ROOT)/doc/rsync-semichroot.txt $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool/bash-utils: | /usr/lib/tool
	install $(REPO_ROOT)/bash/bash-utils $@
	@echo remove $@ >> uninstall.sh

/usr/lib/multithrottler/Throttler.pm: | /usr/lib/multithrottler
	install $(REPO_ROOT)/lib/multithrottler/Throttler.pm $@
	@echo remove $@ >> uninstall.sh

/usr/lib/multithrottler:
	mkdir -p $@
	@echo remove $@ >> uninstall.sh


$(TOOLS_TARGET_DIR):
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

/usr/share/doc/tool:
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool:
	mkdir -p $@
	@echo remove $@ >> uninstall.sh



install-all: \
  /usr/lib/tool/bash-utils \
  install-manpages \
  install-tools


install-tools: $(TOOLS_TARGET_DIR) $(TOOLS_TARGET_FILES)

$(TOOLS_TARGET_FILES): | $(TOOLS_TARGET_DIR)

$(TOOLS_TARGET_FILES): $(TOOLS_TARGET_DIR)/%: %
	install $(notdir $@) $(TOOLS_TARGET_DIR)/
	@echo remove $(TOOLS_TARGET_DIR)/$(notdir $@) >> uninstall.sh


list-tools:
	@echo $(TOOLS) | tr " " "\n" | sort



MANPAGES_DIR = $(REPO_ROOT)/doc/man
MANPAGE_SECTION ?= 1
MANPAGE_SECTION_EXT ?= $(MANPAGE_SECTION)
MANPAGE_FILE_SUFFIX = .$(MANPAGE_SECTION_EXT).xz
MANPAGES_SUBDIR = $(MANPAGES_DIR)/man$(MANPAGE_SECTION)
MANPAGE_FILES = $(foreach toolname,$(TOOLS),$(MANPAGES_SUBDIR)/$(toolname)$(MANPAGE_FILE_SUFFIX))

manpages: $(MANPAGE_FILES)

$(MANPAGE_FILES): | $(MANPAGES_SUBDIR)

$(MANPAGES_SUBDIR):
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

$(MANPAGE_FILES): $(MANPAGES_SUBDIR)/%$(MANPAGE_FILE_SUFFIX): %
	@if podchecker "$<"; then \
		pod2man --name="$<" --section $(MANPAGE_SECTION_EXT) --utf8 "$<" | xz > "$@.tmp" &&\
		mv -f "$@.tmp" "$@" ;\
	fi

install-manpages: manpages
	$(MAKE) -C $(REPO_ROOT)/doc/man
