

ifndef PYTHONLIBDIR
PYTHONPREFIX = $(shell python2.7-config --prefix)

ifeq ($(PYTHONPREFIX),)
$(error Do you have python2.7-config? If no, you may define PYTHONLIBDIR directly, eg: 'make install PYTHONLIBDIR=/usr/local/lib/python2.7')
endif

PYTHONLIBDIR = $(PYTHONPREFIX)/lib/python2.7
endif


.PHONY: default
default:
	@echo some targets to choose form: install
	@false

$(PYTHONLIBDIR)/imagemetadata.py: imagemetadata.py
	install --compare "$<" "$@"

.PHONY: install
install: $(PYTHONLIBDIR)/imagemetadata.py
