
PYTHONPREFIX = $(shell python2.7-config --prefix)
PYTHONLIBDIR = $(PYTHONPREFIX)/lib/python2.7

default:
	@echo some targets to choose form: install
	@false

$(PYTHONLIBDIR)/imagerotation.py: imagerotation.py
	[ -d "$(PYTHONPREFIX)" ]
	install --compare "$<" "$@"

install: $(PYTHONLIBDIR)/imagerotation.py
