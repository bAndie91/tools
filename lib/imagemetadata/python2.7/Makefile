
PYTHONPREFIX = $(shell python2.7-config --prefix)
PYTHONLIBDIR = $(PYTHONPREFIX)/lib/python2.7

default:
	@echo some targets to choose form: install
	@false

$(PYTHONLIBDIR)/imagemetadata.py: imagemetadata.py
	[ -d "$(PYTHONPREFIX)" ]
	install --compare "$<" "$@"

install: $(PYTHONLIBDIR)/imagemetadata.py
