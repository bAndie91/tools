
.PHONY: default
default:
	false

descriptions.md: $(filter-out Makefile README.md descriptions.txt uninstall.sh, $(wildcard admin-tools/* compiled-tools/*.pod crawler-bin/* root-tools/* user-tools/* xwin-tools/*))
	./gen-descriptions $^ > $@~
	mv $@~ $@
