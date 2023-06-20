
TOOLS_TARGET_DIR = /usr/tool

TOOLS = anacron-peruser hidedir nf_condition sshwho swapoff-progress syncifstate

install-all:
	set -e ;\
	for tool in $(TOOLS);\
	do\
		install --compare $$tool $(TOOLS_TARGET_DIR)/ ;\
	done
