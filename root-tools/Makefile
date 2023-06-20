
TOOLS_TARGET_DIR = /usr/tool

TOOLS = anacron-peruser hidedir nf_condition sshwho swapoff-progress syncifstate

TOOLS_TARGET_FILES := $(addprefix $(TOOLS_TARGET_DIR)/,$(TOOLS))


install-all: install-sendmail-cron $(TOOLS_TARGET_FILES)

$(TOOLS_TARGET_FILES):
	install $(notdir $@) $(TOOLS_TARGET_DIR)/
	@echo remove $@ >> uninstall.sh


install-sendmail-cron: /usr/sbin/sendmail.cron
	update-alternatives --install /usr/sbin/sendmail sendmail /usr/sbin/sendmail.cron 800
	@echo update-alternatives --remove sendmail /usr/sbin/sendmail.cron >> uninstall.sh

/usr/sbin/sendmail.cron:
	install sendmail.cron /usr/sbin/
	@echo remove $@ >> uninstall.sh
