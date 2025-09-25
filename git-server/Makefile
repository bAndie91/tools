
EXEC_PREFIX = /usr/local/bin
ETC_PREFIX = /etc


default:
	@echo may be interested in: install
	false

install: $(EXEC_PREFIX)/git-receive-pack $(EXEC_PREFIX)/git-repo-mgmt $(ETC_PREFIX)/issue.git
.PHONY: install

$(EXEC_PREFIX)/git-receive-pack $(EXEC_PREFIX)/git-repo-mgmt: $(EXEC_PREFIX)/%: %
	install $< $@

$(ETC_PREFIX)/issue.git: $(ETC_PREFIX)/%: %
	cp -f $< $@


define uninstall
	@[ ! -e $(1) ] || remove $(1)
endef

uninstall:
	$(call uninstall,$(EXEC_PREFIX)/git-receive-pack)
	$(call uninstall,$(EXEC_PREFIX)/git-repo-mgmt)
	$(call uninstall,$(ETC_PREFIX)/issue.git)
.PHONY: uninstall
