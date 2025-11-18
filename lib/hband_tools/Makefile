
# maybe set INSTALL_WRAPPER to 'sudo'
INSTALL_WRAPPER =

PYTHON_VERSION = 2.7

ifndef INSTALL_PREFIX
PYTHONPREFIX = $(shell python$(PYTHON_VERSION)-config --prefix)

ifeq ($(PYTHONPREFIX),)
$(error Do you have python$(PYTHON_VERSION)-config? If no, you may define INSTALL_PREFIX directly, eg: 'make install INSTALL_PREFIX=/usr/local/lib/python$(PYTHON_VERSION)')
endif

INSTALL_PREFIX = $(PYTHONPREFIX)/lib/python$(PYTHON_VERSION)/hband_tools
endif


.PHONY: default
default:
	@echo some targets to choose form: install
	@false


SOURCE_PREFIX = python$(PYTHON_VERSION)
SOURCE_FILES = $(wildcard $(SOURCE_PREFIX)/*.py)
INSTALL_SUFFIXES = $(patsubst $(SOURCE_PREFIX)/%,%,$(SOURCE_FILES))
INSTALL_TARGETS = $(addprefix $(INSTALL_PREFIX)/,$(INSTALL_SUFFIXES))


$(INSTALL_TARGETS): $(INSTALL_PREFIX)/%: $(SOURCE_PREFIX)/% | $(INSTALL_PREFIX)
	$(INSTALL_WRAPPER) install $< "$@"
	@echo remove $@ >> uninstall.sh

$(INSTALL_PREFIX):
	$(INSTALL_WRAPPER) mkdir -p $@
	@echo remove $@ >> uninstall.sh

.PHONY: install
install: $(INSTALL_TARGETS)
