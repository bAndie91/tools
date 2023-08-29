
REPO_ROOT ?= ..

TOOLS_TARGET_DIR = /usr/tool

TOOLS ?=   2opml adr2html asterisk-log-separator awk-cut base58 base64url \
  bencode2json cdexec chattr-cow chattr-nocow chromium_cookie_decrypt.py \
  chshebang convert_chromium_cookies_to_netscape.sh corner_time \
  cut.awk daemonctl dataurl2bin debdiff descpids dfbar digasn diu dlnew \
  dump_php_session.php eat errorlevel error_reporting.php fcomplete \
  fc-search-codepoint fdupes-hardlink ff fgat find-by-date findnewestfile \
  findoldestfile fixlogfiledatetime fixRFC822filemtime fmtkv foreach \
  g_filename_to_uri git-checkout-sparse gitdiff grepdatetime Head \
  header.sed indent2tree inisort is_gzip jobsel json2bencode json2msgpack \
  kt LevelDB lines screens lnto locale-validator lpset ls2html lsata \
  lsenv mime_extract mkdeb moz_bookmarks msg msgpack2json multithrottler \
  mysql-fix-orphan-privileges.php noacute nocomment nthash.py ocspverify \
  organizebydate palemoon-current-url parsel partial paths2indent \
  perl-repl pipekill PMbwmon PMdirindex PMhexdiff PMnslist PMpwgen \
  PMrecdiff pyzor-files qrwifi pvalve renamemanual repeat \
  reportcmdstatus rsacrypt rsync-semichroot rsysrq screenconsole \
  screen-notify screenreattach slay slayall sockio ssh-agent-finder \
  straceall strip-ansi-seq swap symlinks2dot tabularize Tail takeown taslis \
  tests text2img-dataurl touchx ttinput uchmod unicodestyle upsidedown \
  url_decode url_encode url_encode_bf url-parts vidir-sanitize vifiles \
  waitpid whisper-retention-info wikibot xdg-autostart xml2json \
  levenshtein-distance jaro-metric jaro-winkler-metric loggerexec \
  logrotate-counters logto pdfflop pngmetatext set-sys-path subst_sudo_user \
  terminaltitle timestamper set-xcursor-lock-and-run mkmagnetlink htmlentities \
  pfx2pem rdfindreport symlinks-analyze rn filterexec \
  mail-extract-addresses mail-extract-raw-headers

TOOLS_TARGET_FILES = $(addprefix $(TOOLS_TARGET_DIR)/,$(TOOLS))



default:
	@echo You may be interested in: install-all, install-tools, list-tools, manpages
	@false
.PHONY: default


$(TOOLS_TARGET_DIR)/msg: /etc/bash_completion.d/msg
$(TOOLS_TARGET_DIR)/perl-repl: /usr/libexec/perlshell
$(TOOLS_TARGET_DIR)/rsync-semichroot: /usr/share/doc/tool/rsync-semichroot.txt
$(TOOLS_TARGET_DIR)/multithrottler: /usr/lib/multithrottler/Throttler.pm

/etc/bash_completion.d/msg: $(REPO_ROOT)/bash/bash_completion.d/msg
	install $(REPO_ROOT)/bash/bash_completion.d/msg $@
	@echo remove $@ >> uninstall.sh

/usr/libexec/perlshell: $(REPO_ROOT)/libexec/perlshell
	install $(REPO_ROOT)/libexec/perlshell $@
	@echo remove $@ >> uninstall.sh

/usr/share/doc/tool/rsync-semichroot.txt: $(REPO_ROOT)/doc/rsync-semichroot.txt
	install $(REPO_ROOT)/doc/rsync-semichroot.txt $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool/bash-utils: $(REPO_ROOT)/bash/bash-utils | /usr/lib/tool
	install $(REPO_ROOT)/bash/bash-utils $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool/ansi-codes: $(REPO_ROOT)/bash/ansi-codes | /usr/lib/tool
	install $(REPO_ROOT)/bash/ansi-codes $@
	@echo remove $@ >> uninstall.sh

/usr/lib/multithrottler/Throttler.pm: $(REPO_ROOT)/lib/multithrottler/Throttler.pm | /usr/lib/multithrottler
	install $(REPO_ROOT)/lib/multithrottler/Throttler.pm $@
	@echo remove $@ >> uninstall.sh

/usr/lib/multithrottler:
	[ -d $@ ] || mkdir $@
	@echo remove $@ >> uninstall.sh


$(TOOLS_TARGET_DIR):
	[ -d $@ ] || mkdir $@
	@echo remove $@ >> uninstall.sh

/usr/share/doc/tool:
	[ -d $@ ] || mkdir $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool:
	[ -d $@ ] || mkdir $@
	@echo remove $@ >> uninstall.sh



install-all: \
  /usr/lib/tool/bash-utils \
  /usr/lib/tool/ansi-codes \
  install-tools \
  install-manpages
.PHONY: install-all


install-tools: $(TOOLS_TARGET_FILES)
.PHONY: install-tools

$(TOOLS_TARGET_FILES): | $(TOOLS_TARGET_DIR)

$(TOOLS_TARGET_FILES): $(TOOLS_TARGET_DIR)/%: %
	install $(notdir $@) $(TOOLS_TARGET_DIR)/
	@echo remove $@ >> uninstall.sh


list-tools:
	@echo $(TOOLS) | tr " " "\n" | sort
.PHONY: list-tools


include Makefile.documentation.mk
