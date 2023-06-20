
TOOLS_TARGET_DIR = /usr/tool

TOOLS = 2opml adr2html asterisk-log-separator awk-cut base58 base64url \
  bencode2json chattr-cow chattr-nocow chromium_cookie_decrypt.py \
  chshebang convert_chromium_cookies_to_netscape.sh corner_time cut.awk \
  debdiff descpids dfbar digasn digwiki diu dlnew dump_php_session.php eat \
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
  reportcmdstatus multithrottler

TOOLS_TARGET_FILES := $(addprefix $(TOOLS_TARGET_DIR)/,$(TOOLS))


install-msg: $(TOOLS_TARGET_DIR)/msg /etc/bash_completion.d/msg

/etc/bash_completion.d/msg:
	install ../bash/bash_completion.d/msg $@
	@echo remove $@ >> uninstall.sh

install-perl-repl: $(TOOLS_TARGET_DIR)/perl-repl /usr/libexec/perlshell

/usr/libexec/perlshell:
	install ../libexec/perlshell $@
	@echo remove $@ >> uninstall.sh

install-rsync-semichroot: $(TOOLS_TARGET_DIR)/rsync-semichroot /usr/share/doc/tool/rsync-semichroot.txt

/usr/share/doc/tool/rsync-semichroot.txt:
	install ../doc/rsync-semichroot.txt $@
	@echo remove $@ >> uninstall.sh

install-bash-functions: install-basedir-lib /usr/lib/tool/bash-utils

/usr/lib/tool/bash-utils:
	install ../bash/bash-utils $@
	@echo remove $@ >> uninstall.sh

install-multithrottler: $(TOOLS_TARGET_DIR)/multithrottler /usr/lib/multithrottler/Throttler.pm

/usr/lib/multithrottler/Throttler.pm:
	install ../lib/multithrottler/Throttler.pm $@
	@echo remove $@ >> uninstall.sh


install-basedirs: install-basedir-lib
	mkdir -p $(TOOLS_TARGET_DIR)
	@echo remove $(TOOLS_TARGET_DIR) >> uninstall.sh
	mkdir -p /usr/share/doc/tool
	@echo remove /usr/share/doc/tool >> uninstall.sh

install-basedir-lib:
	mkdir -p /usr/lib/tool
	@echo remove /usr/lib/tool >> uninstall.sh



install-all: install-basedirs install-msg install-perl-repl install-rsync-semichroot \
  install-multithrottler \
  install-bash-functions $(TOOLS_TARGET_FILES) install-manpages


$(TOOLS_TARGET_FILES): install-basedirs
	install $(notdir $@) $(TOOLS_TARGET_DIR)/
	@echo remove $(TOOLS_TARGET_DIR)/$(notdir $@) >> uninstall.sh


list-tools:
	@echo $(TOOLS) | tr " " "\n" | sort



MANPAGES_DIR = ../doc/man
MANPAGE_FILE_SUFFIX = .1.xz
MANPAGE_FILES := $(addprefix $(MANPAGES_DIR)/,$(addsuffix $(MANPAGE_FILE_SUFFIX),$(TOOLS)))

manpages: $(MANPAGE_FILES)

$(MANPAGE_FILES):
	@set -e ;\
	tool=$$(basename "$@" $(MANPAGE_FILE_SUFFIX)) ;\
	if podchecker "$$tool"; then \
		pod2man --utf8 "$$tool" | xz > "$(MANPAGES_DIR)/$$tool$(MANPAGE_FILE_SUFFIX).tmp" ;\
		mv "$(MANPAGES_DIR)/$$tool$(MANPAGE_FILE_SUFFIX).tmp" "$(MANPAGES_DIR)/$$tool$(MANPAGE_FILE_SUFFIX)" ;\
	fi

install-manpages: manpages
	$(MAKE) -C ../doc/man
