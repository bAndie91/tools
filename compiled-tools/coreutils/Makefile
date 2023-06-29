
SOURCEDIR = coreutils
CHMOD_OBJ_DEP_FILES = \
  lib/configmake.h src/version.h lib/unused-parameter.h lib/sys/stat.h lib/string.h lib/time.h lib/fcntl.h \
  lib/stdio.h lib/getopt.h lib/inttypes.h lib/stddef.h lib/stdlib.h lib/unistd.h lib/wchar.h
CHMOD_OBJ_DEPS = $(foreach file,$(CHMOD_OBJ_DEP_FILES),$(SOURCEDIR)/$(file))


SUPPORTED_COMMIT = 7945e09e7b1164cc463d6f6c2bb717211276df74

# if your sysstem defaults are too old
BOOTSTRAP_EXTRA_PATH=/usr/local/opt/gettext_0.19.6/bin


chmod-cheap: $(SOURCEDIR)/src/chmod
	install $< $@


define make_coreutils_target
	$(MAKE) -C $(SOURCEDIR) $(subst $(SOURCEDIR)/,,$@)
endef

define checkout_supported_commit
	cd $(SOURCEDIR) && if [ $$(git show -s --format=%H) != $(SUPPORTED_COMMIT) ]; then git checkout $(SUPPORTED_COMMIT); fi
endef


$(SOURCEDIR)/src/chmod: $(SOURCEDIR)/src/chmod.o
	$(make_coreutils_target)

$(SOURCEDIR)/src/chmod.o: $(SOURCEDIR)/src/chmod.c $(CHMOD_OBJ_DEPS)
	$(make_coreutils_target)

$(SOURCEDIR)/src/chmod.c: $(SOURCEDIR)/.git/HEAD
	$(checkout_supported_commit)
	cd $(SOURCEDIR) && git checkout HEAD -- src/chmod.c
	patch -d $(SOURCEDIR) -p1 -i ../chmod-cheap.patch -f


$(CHMOD_OBJ_DEPS): $(SOURCEDIR)/Makefile
	$(make_coreutils_target)

$(SOURCEDIR)/Makefile: $(SOURCEDIR)/configure
	cd $(SOURCEDIR) && ./configure

$(SOURCEDIR)/configure: $(SOURCEDIR)/bootstrap
	$(checkout_supported_commit)
	cd $(SOURCEDIR) && PATH=$(BOOTSTRAP_EXTRA_PATH):$$PATH ./bootstrap --skip-po

$(SOURCEDIR)/bootstrap: $(SOURCEDIR)/.git/HEAD
	$(checkout_supported_commit)
	touch $@


$(SOURCEDIR)/.git/HEAD: | $(SOURCEDIR)
	$(checkout_supported_commit)


$(SOURCEDIR):
	git clone git://git.sv.gnu.org/coreutils $(SOURCEDIR)


clean:
	cd $(SOURCEDIR) && git checkout HEAD -- .
	cd $(SOURCEDIR) && git clean -x -d -f

.PHONY: clean
