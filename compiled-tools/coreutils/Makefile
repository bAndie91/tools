
SOURCEDIR = coreutils

CHMOD_DEPS = $(SOURCEDIR)/lib/configmake.h $(SOURCEDIR)/src/version.h $(SOURCEDIR)/src/chmod.o $(SOURCEDIR)/lib/unused-parameter.h

SUPPORTED_COMMIT = 7945e09



chmod-cheap: $(SOURCEDIR)/src/chmod
	install $< $@


define make_coreutils_target
	$(MAKE) -C $(SOURCEDIR) $(subst $(SOURCEDIR)/,,$@)
endef

$(SOURCEDIR)/src/chmod: $(CHMOD_DEPS)
	$(make_coreutils_target)

$(CHMOD_DEPS): $(SOURCEDIR)/Makefile
	$(make_coreutils_target)

$(SOURCEDIR)/Makefile: $(SOURCEDIR)/configure
	cd $(SOURCEDIR) && ./configure

$(SOURCEDIR)/configure: $(SOURCEDIR)/.git/HEAD
	cd $(SOURCEDIR) && PATH=/usr/local/opt/gettext_0.19.6/bin:$$PATH ./bootstrap


$(SOURCEDIR)/src/chmod.o: $(SOURCEDIR)/src/chmod.c

$(SOURCEDIR)/src/chmod.c: $(SOURCEDIR)/.git/HEAD
	cd $(SOURCEDIR) && git checkout HEAD -- src/chmod.c
	patch -d $(SOURCEDIR) -p1 -i ../chmod-cheap.patch -f



$(SOURCEDIR)/.git/HEAD: | $(SOURCEDIR)
	cd $(SOURCEDIR) && git checkout $(SUPPORTED_COMMIT)

$(SOURCEDIR):
	git clone git://git.sv.gnu.org/coreutils $(SOURCEDIR)
