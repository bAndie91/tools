
TOOLS_TARGET_DIR = /usr/tools

TOOLS = 2opml adr2html asterisk-log-separator awk-cut base58 base64url \
  bencode2json chattr-cow chattr-nocow chromium_cookie_decrypt.py \
  chshebang convert_chromium_cookies_to_netscape.sh corner_time cut.awk \
  debdiff descpids dfbar digasn digwiki diu dlnew dump_php_session.php eat \
  errorlevel error_reporting.php fcomplete fc-search-codepoint ff fgat \
  find-by-date findnewestfile findoldestfile geoip git-checkout-sparse \
  gitdiff grepdatetime Head header.sed \
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
  xdg-autostart xml2json

install-msg:
	install --compare msg $(TOOLS_TARGET_DIR)/
	install --compare ../bash/bash_completion.d/msg /etc/bash_completion.d/

install-perl-repl:
	install --compare perl-repl $(TOOLS_TARGET_DIR)/
	install --compare ../libexec/perlshell /usr/libexec/

install-rsync-semichroot:
	install --compare rsync-semichroot $(TOOLS_TARGET_DIR)/
	install --compare ../doc/rsync-semichroot.txt /usr/share/doc/tools/rsync-semichroot.txt

install-basedirs: install-basedir-lib
	mkdir /usr/tools
	mkdir /usr/share/doc/tools

install-basedir-lib:
	mkdir /usr/lib/tools

install-bash-functions: install-basedir-lib
	install --compare ../bash/bash-utils /usr/lib/tools/

install-all: install-basedirs install-msg install-perl-repl install-rsync-semichroot install-bash-functions
	set -e ;\
	for tool in $(TOOLS);\
	do\
		install --compare $$tool $(TOOLS_TARGET_DIR)/ ;\
	done
