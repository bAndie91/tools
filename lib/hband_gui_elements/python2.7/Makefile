
# maybe set INSTALL_WRAPPER to 'sudo'
INSTALL_WRAPPER =


ifndef PYTHONLIBDIR
PYTHONPREFIX = $(shell python2.7-config --prefix)

ifeq ($(PYTHONPREFIX),)
$(error Do you have python2.7-config? If no, you may define PYTHONLIBDIR directly, eg: 'make install PYTHONLIBDIR=/usr/local/lib/python2.7')
endif

PYTHONLIBDIR = $(PYTHONPREFIX)/lib/python2.7/hband_gui_elements
endif


.PHONY: default
default:
	@echo some targets to choose form: install
	@false


SOURCE_FILES = $(wildcard *.py)
INSTALL_TARGETS = $(addprefix $(PYTHONLIBDIR)/,$(SOURCE_FILES))

$(INSTALL_TARGETS): $(PYTHONLIBDIR)/%: % | $(PYTHONLIBDIR)
	$(INSTALL_WRAPPER) install $< "$@"

$(PYTHONLIBDIR):
	$(INSTALL_WRAPPER) mkdir -p $@

.PHONY: install
install: $(INSTALL_TARGETS)
