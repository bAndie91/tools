
TOOLS_TARGET_DIR = /usr/tool

TOOLS = anacron-peruser hidedir nf_condition sshwho swapoff-progress syncifstate

TOOLS_TARGET_FILES := $(addprefix $(TOOLS_TARGET_DIR)/,$(TOOLS))


install-all: $(TOOLS_TARGET_FILES) /usr/sbin/sendmail.cron /usr/sbin/sendmail.atd install-sendmail-smtp

$(TOOLS_TARGET_FILES):
	install $(notdir $@) $(TOOLS_TARGET_DIR)/
	@echo remove $@ >> uninstall.sh


/usr/sbin/sendmail.smtp: sendmail.smtp
	install $< $(dir $@)
	@echo remove $@ >> uninstall.sh

install-sendmail-smtp: /usr/sbin/sendmail.smtp
	update-alternatives --install /usr/sbin/sendmail sendmail /usr/sbin/sendmail.smtp 800
	@echo update-alternatives --remove sendmail /usr/sbin/sendmail.smtp >> uninstall.sh

.PHONY: install-sendmail-smtp

/usr/sbin/sendmail.cron: sendmail.cron
	install $< $(dir $@)
	@echo remove $@ >> uninstall.sh

/usr/sbin/sendmail.atd: sendmail.atd
	install $< $(dir $@)
	@echo remove $@ >> uninstall.sh
