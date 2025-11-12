
# specify INSTALL_WRAPPER=sudo at the make command if you need
INSTALL_WRAPPER =

TOOLS_TARGET_DIR = /usr/tool

include $(dir $(lastword $(MAKEFILE_LIST)))/Makefile.lists.mk

SYMLINKS = $(shell find $(addprefix ./,$(TOOLS)) -maxdepth 0 -type l -printf "%f\n")
SYMLINK_TOOLS = $(filter $(SYMLINKS),$(TOOLS))
NORMAL_TOOLS = $(filter-out $(SYMLINK_TOOLS),$(TOOLS))
TOOLS_TARGET_FILES = $(addprefix $(TOOLS_TARGET_DIR)/,$(NORMAL_TOOLS))
TOOLS_TARGET_SYMLINKS = $(addprefix $(TOOLS_TARGET_DIR)/,$(SYMLINK_TOOLS))


define record_uninstall
	echo remove $@ >> uninstall.sh
endef


default:
	@echo You may be interested in: install-all, install-tools, list-tools, manpages
	@false
.PHONY: default


$(TOOLS_TARGET_DIR)/msg: /usr/share/bash-completion/completions/msg
$(TOOLS_TARGET_DIR)/perl-repl: /usr/libexec/perlshell
$(TOOLS_TARGET_DIR)/rsync-semichroot: /usr/share/doc/tool/rsync-semichroot.txt
$(TOOLS_TARGET_DIR)/multithrottler: /usr/lib/multithrottler/Throttler.pm
$(TOOLS_TARGET_DIR)/git-mirrors: /usr/share/bash-completion/completions/git-mirrors
$(TOOLS_TARGET_DIR)/git-push-remotes: /usr/share/bash-completion/completions/git-push-remotes

COMPLETION_BASH_SCRIPTS = $(foreach path,$(wildcard ../bash/bash_completion.d/*),$(notdir $(path)))

$(addprefix /usr/share/bash-completion/completions/,$(COMPLETION_BASH_SCRIPTS)): /usr/share/bash-completion/completions/%: ../bash/bash_completion.d/%
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

/usr/libexec/perlshell: ../libexec/perlshell
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

/usr/share/doc/tool/rsync-semichroot.txt: ../doc/rsync-semichroot.txt | /usr/share/doc/tool
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

/usr/lib/tool/bash-utils: ../bash/bash-utils | /usr/lib/tool
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

/usr/lib/multithrottler/Throttler.pm: ../lib/multithrottler/Throttler.pm | /usr/lib/multithrottler
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

/usr/lib/multithrottler:
	[ -d $@ ] || mkdir $@
	@$(record_uninstall)


$(TOOLS_TARGET_DIR):
	[ -d $@ ] || mkdir $@
	@$(record_uninstall)

/usr/share/doc/tool:
	[ -d $@ ] || mkdir $@
	@$(record_uninstall)

/usr/lib/tool:
	[ -d $@ ] || mkdir $@
	@$(record_uninstall)



DEPCHECK_TARGETS = $(addprefix DEPCHECK/,$(NORMAL_TOOLS))
depcheck-all: $(DEPCHECK_TARGETS)
.PHONY: $(DEPCHECK_TARGETS)


DEPCHECK/lines:
	perl -MARGV::readonly -e 1
DEPCHECK/parsel:
	python3 -c 'import parsel; import cssselect'
DEPCHECK/xml2json:
	python -c 'import xmltodict'
DEPCHECK/palemoon-current-urls:
	which jq
DEPCHECK/dbus-call:
	perl -MNet::DBus -e 1
	perl -MXML::Hash::LX -e 1
	perl -MJSON -e 1

.PHONY: try-satisfy-dependencies
try-satisfy-dependencies:
	-cpan ARGV::readonly
	-$(INSTALL_WRAPPER) apt install python3-parsel
	-$(INSTALL_WRAPPER) apt install python3-cssselect
	-$(INSTALL_WRAPPER) apt install python3-xmltodict
	-$(INSTALL_WRAPPER) apt install jq
	-$(INSTALL_WRAPPER) apt install libnet-dbus-perl || cpan Net::DBus
	-$(INSTALL_WRAPPER) apt install libxml-libxml-perl libxml-hash-lx-perl || cpan XML::Hash::LX
	-$(INSTALL_WRAPPER) apt install libjson-perl || cpan JSON
	-$(INSTALL_WRAPPER) apt install liblog-log4perl-perl || cpan Log::Log4perl
	-$(INSTALL_WRAPPER) apt install libyaml-perl || cpan YAML
	-$(INSTALL_WRAPPER) apt install libversion-util-perl || cpan Version::Util
	-$(INSTALL_WRAPPER) apt install libdatetime-format-strptime-perl || cpan DateTime::Format::Strptime


.PHONY: install-all
install-all: \
  git-utimes \
  /usr/lib/tool/bash-utils \
  install-tools \
  install-manpages


# change files mtime back to their last git commit date,
# so merely switching branches does not make 'make' believe that the requirement files are newer.
.PHONY: git-utimes
git-utimes:
	git utimes . || true

.PHONY: install-tools
install-tools: $(TOOLS_TARGET_FILES) $(TOOLS_TARGET_SYMLINKS)

$(TOOLS_TARGET_FILES) $(TOOLS_TARGET_SYMLINKS): | $(TOOLS_TARGET_DIR)

$(TOOLS_TARGET_FILES): $(TOOLS_TARGET_DIR)/%: %
	$(MAKE) DEPCHECK/$<
	$(INSTALL_WRAPPER) install $< $@
	@$(record_uninstall)

install-symlinks: $(TOOLS_TARGET_SYMLINKS)
.PHONY: install-symlinks

$(TOOLS_TARGET_SYMLINKS): $(TOOLS_TARGET_DIR)/%: %
	$(INSTALL_WRAPPER) ln -snfv $(shell readlink $<) $@
	@$(record_uninstall)


list-tools:
	@echo $(TOOLS) | tr " " "\n" | sort
.PHONY: list-tools


include $(dir $(lastword $(MAKEFILE_LIST)))/Makefile.documentation.mk
