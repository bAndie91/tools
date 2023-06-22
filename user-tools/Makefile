
TOOLS_TARGET_DIR = /usr/tool

TOOLS = 2opml adr2html asterisk-log-separator awk-cut base58 base64url \
  bencode2json chattr-cow chattr-nocow chromium_cookie_decrypt.py \
  chshebang convert_chromium_cookies_to_netscape.sh corner_time cut.awk \
  debdiff descpids dfbar digasn diu dlnew dump_php_session.php eat \
  errorlevel error_reporting.php fcomplete fc-search-codepoint fdupes-hardlink ff fgat \
  find-by-date findnewestfile findoldestfile foreach \
  geoip git-checkout-sparse gitdiff grepdatetime Head header.sed \
  indent2tree inisort is_gzip jobsel json2bencode json2msgpack LevelDB \
  lines list_screens lpset ls2html lsata lsenv mime_extract mkdeb moz_bookmarks \
  msgpack2json mysql-fix-orphan-privileges.php noacute nocomment nthash.py \
  ocspverify organizebydate palemoon-current-url parsel partial \
  paths2indent pipekill PMbwmon PMdirindex PMhexdiff PMnslist \
  PMpwgen PMrecdiff psgetchildpids pyzor-files qrwifi regulate \
  renamemanual repeat rfc rsacrypt rsysrq screenconsole \
  screen-notify screenreattach slay slayall sockio ssh-agent-finder \
  straceall strip-ansi-seq swap symlinks2dot tabularize Tail takeown \
  taslis tests text2img-dataurl touchx ttinput uchmod unicodestyle \
  upsidedown url_decode url_encode url_encode_bf url-parts vidir-sanitize \
  vifiles waitpid whisper-retention-info wikibot \
  xdg-autostart xml2json msg perl-repl rsync-semichroot locale-validator \
  reportcmdstatus multithrottler \
  cdexec daemonctl dataurl2bin fixlogfiledatetime fixRFC822filemtime fmtkv \
  g_filename_to_uri kt lnto

TOOLS_TARGET_FILES := $(addprefix $(TOOLS_TARGET_DIR)/,$(TOOLS))



$(TOOLS_TARGET_DIR)/msg: /etc/bash_completion.d/msg
$(TOOLS_TARGET_DIR)/perl-repl: /usr/libexec/perlshell
$(TOOLS_TARGET_DIR)/rsync-semichroot: /usr/share/doc/tool/rsync-semichroot.txt
$(TOOLS_TARGET_DIR)/multithrottler: /usr/lib/multithrottler/Throttler.pm

/etc/bash_completion.d/msg:
	install ../bash/bash_completion.d/msg $@
	@echo remove $@ >> uninstall.sh

/usr/libexec/perlshell:
	install ../libexec/perlshell $@
	@echo remove $@ >> uninstall.sh

/usr/share/doc/tool/rsync-semichroot.txt:
	install ../doc/rsync-semichroot.txt $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool/bash-utils: | /usr/lib/tool
	install ../bash/bash-utils $@
	@echo remove $@ >> uninstall.sh

/usr/lib/multithrottler/Throttler.pm: | /usr/lib/multithrottler
	install ../lib/multithrottler/Throttler.pm $@
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



MANPAGES_DIR = ../doc/man
MANPAGE_SECTION = 1
MANPAGE_FILE_SUFFIX = .$(MANPAGE_SECTION).xz
MANPAGE_FILES := $(foreach toolname,$(TOOLS),$(MANPAGES_DIR)/man$(MANPAGE_SECTION)/$(toolname)$(MANPAGE_FILE_SUFFIX))

manpages: $(MANPAGE_FILES)

$(MANPAGES_DIR)/man*/*: | $(MANPAGES_DIR)/man$(MANPAGE_SECTION)/

$(MANPAGES_DIR)/man*/:
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

$(MANPAGE_FILES): $(MANPAGES_DIR)/man$(MANPAGE_SECTION)/%$(MANPAGE_FILE_SUFFIX): %
	@if podchecker "$<"; then \
		outfile="$(MANPAGES_DIR)/man$(MANPAGE_SECTION)/$<$(MANPAGE_FILE_SUFFIX)" ;\
		pod2man --name="$<" --section $(MANPAGE_SECTION) --utf8 "$<" | xz > "$$outfile.tmp" &&\
		mv -f "$$outfile.tmp" "$$outfile" ;\
	fi

install-manpages: manpages
	$(MAKE) -C ../doc/man
