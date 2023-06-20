
TARGET_DIR = /usr/tools

TOOLS = anacron-peruser hidedir nf_condition sshwho swapoff-progress syncifstate

install-all:
	set -e ;\
	for tool in $(TOOLS);\
	do\
		install --compare $$tool $(TARGET_DIR)/ ;\
	done
