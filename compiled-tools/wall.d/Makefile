
SOURCEDIR = util-linux


SUPPORTED_COMMIT = da00806c1916a8a67fd6f4ef4016de92bab340dd



wall: $(SOURCEDIR)/wall
	install $< $@


define make_uptream_target
	$(MAKE) -C $(SOURCEDIR) $(subst $(SOURCEDIR)/,,$@)
endef

define checkout_supported_commit
	cd $(SOURCEDIR) && if [ $$(git show -s --format=%H) != $(SUPPORTED_COMMIT) ]; then git checkout $(SUPPORTED_COMMIT); fi
endef


$(SOURCEDIR)/wall: $(SOURCEDIR)/term-utils/wall.c
	$(make_uptream_target)

$(SOURCEDIR)/term-utils/wall.c: $(SOURCEDIR)/Makefile
	$(checkout_supported_commit)
	cd $(SOURCEDIR) && git checkout HEAD -- term-utils/wall.c
	patch -d $(SOURCEDIR) -p2 -i ../wall.c.patch -f


$(SOURCEDIR)/Makefile: $(SOURCEDIR)/configure
	cd $(SOURCEDIR) && ./configure --disable-all-programs --enable-wall --without-python

$(SOURCEDIR)/configure: $(SOURCEDIR)/autogen.sh
	$(checkout_supported_commit)
	cd $(SOURCEDIR) && ./autogen.sh
	touch $@

$(SOURCEDIR)/autogen.sh: $(SOURCEDIR)/.git/HEAD
	$(checkout_supported_commit)
	touch $@


$(SOURCEDIR)/.git/HEAD: | $(SOURCEDIR)
	$(checkout_supported_commit)


$(SOURCEDIR):
	git clone https://salsa.debian.org/debian/util-linux.git $(SOURCEDIR)


clean:
	cd $(SOURCEDIR) && git checkout HEAD -- .
	cd $(SOURCEDIR) && git clean -x -d -f
	touch $(SOURCEDIR)/autogen.sh

.PHONY: clean
