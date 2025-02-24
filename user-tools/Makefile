
REPO_ROOT ?= ..

TOOLS_TARGET_DIR = /usr/tool

include $(dir $(lastword $(MAKEFILE_LIST)))/Makefile.lists.mk

SYMLINKS = $(shell find $(addprefix ./,$(TOOLS)) -maxdepth 0 -type l -printf "%f\n")
SYMLINK_TOOLS = $(filter $(SYMLINKS),$(TOOLS))
NORMAL_TOOLS = $(filter-out $(SYMLINK_TOOLS),$(TOOLS))
TOOLS_TARGET_FILES = $(addprefix $(TOOLS_TARGET_DIR)/,$(NORMAL_TOOLS))
TOOLS_TARGET_SYMLINKS = $(addprefix $(TOOLS_TARGET_DIR)/,$(SYMLINK_TOOLS))



default:
	@echo You may be interested in: install-all, install-tools, list-tools, manpages
	@false
.PHONY: default


$(TOOLS_TARGET_DIR)/msg: /usr/share/bash-completion/completions/msg
$(TOOLS_TARGET_DIR)/perl-repl: /usr/libexec/perlshell
$(TOOLS_TARGET_DIR)/rsync-semichroot: /usr/share/doc/tool/rsync-semichroot.txt
$(TOOLS_TARGET_DIR)/multithrottler: /usr/lib/multithrottler/Throttler.pm
$(TOOLS_TARGET_DIR)/git-mirrors: /usr/share/bash-completion/completions/git-mirrors
$(TOOLS_TARGET_DIR)/git-push-sync-branches: /usr/share/bash-completion/completions/git-push-sync-branches

COMPLETION_BASH_SCRIPTS = $(foreach path,$(wildcard $(REPO_ROOT)/bash/bash_completion.d/*),$(notdir $(path)))

$(addprefix /usr/share/bash-completion/completions/,$(COMPLETION_BASH_SCRIPTS)): /usr/share/bash-completion/completions/%: $(REPO_ROOT)/bash/bash_completion.d/%
	install $< $@
	@echo remove $@ >> uninstall.sh

/usr/libexec/perlshell: $(REPO_ROOT)/libexec/perlshell
	install $< $@
	@echo remove $@ >> uninstall.sh

/usr/share/doc/tool/rsync-semichroot.txt: $(REPO_ROOT)/doc/rsync-semichroot.txt | /usr/share/doc/tool
	install $< $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool/bash-utils: $(REPO_ROOT)/bash/bash-utils | /usr/lib/tool
	install $< $@
	@echo remove $@ >> uninstall.sh

/usr/lib/multithrottler/Throttler.pm: $(REPO_ROOT)/lib/multithrottler/Throttler.pm | /usr/lib/multithrottler
	install $< $@
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



DEPCHECK_TARGETS = $(addprefix DEPCHECK/,$(NORMAL_TOOLS))
depcheck-all: $(DEPCHECK_TARGETS)
.PHONY: $(DEPCHECK_TARGETS)

DEPCHECK/lines:
	perl -MARGV::readonly -e 1
DEPCHECK/parsel:
	python3 -c 'import parsel; import cssselect'
DEPCHECK/xml2json:
	python -c 'import xmltodict'
DEPCHECK/palemoon-current-url:
	which jq
DEPCHECK/dbus-call:
	perl -MNet::DBus -e 1
	perl -MXML::Hash::LX -e 1
	perl -MJSON -e 1

.PHONY: try-satisfy-dependencies
try-satisfy-dependencies:
	-cpan ARGV::readonly
	-apt install python3-parsel
	-apt install python3-cssselect
	-apt install python3-xmltodict
	-apt install jq
	-apt install libnet-dbus-perl || cpan Net::DBus
	-apt install libxml-libxml-perl libxml-hash-lx-perl || cpan XML::Hash::LX
	-apt install libjson-perl || cpan JSON
	-apt install liblog-log4perl-perl || cpan Log::Log4perl
	-apt install libyaml-perl || cpan YAML


install-all: \
  /usr/lib/tool/bash-utils \
  install-tools \
  install-manpages
.PHONY: install-all


install-tools: depcheck-all $(TOOLS_TARGET_FILES) $(TOOLS_TARGET_SYMLINKS)
.PHONY: install-tools

$(TOOLS_TARGET_FILES) $(TOOLS_TARGET_SYMLINKS): | $(TOOLS_TARGET_DIR)

$(TOOLS_TARGET_FILES): $(TOOLS_TARGET_DIR)/%: % DEPCHECK/%
	install --compare $(notdir $@) $(TOOLS_TARGET_DIR)/
	@echo remove $@ >> uninstall.sh

install-symlinks: $(TOOLS_TARGET_SYMLINKS)
.PHONY: install-symlinks

$(TOOLS_TARGET_SYMLINKS): $(TOOLS_TARGET_DIR)/%: %
	ln -snfv $(shell readlink $<) $@
	@echo remove $@ >> uninstall.sh


list-tools:
	@echo $(TOOLS) | tr " " "\n" | sort
.PHONY: list-tools


include $(dir $(lastword $(MAKEFILE_LIST)))/Makefile.documentation.mk
