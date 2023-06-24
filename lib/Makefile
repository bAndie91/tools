
/usr/lib/tool:
	mkdir -p $@
	@echo remove $@ >> uninstall.sh

/usr/lib/tool/functions.bc: /usr/lib/tool /etc/profile.d/bc-functions.sh
	install functions.bc $@
	@echo remove $@ >> uninstall.sh

/etc/profile.d/bc-functions.sh:
	install bc-functions.sh $@
	@echo remove $@ >> uninstall.sh

install-all: /usr/lib/tool/functions.bc
