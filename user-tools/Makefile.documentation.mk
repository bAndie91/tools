
ifndef TOOLS
$(error TOOLS is not set in Makefile)
endif

descriptions.txt: $(TOOLS)
	perl -ne 'if(/^=head1 NAME/){ <>; print while $$_=<>  and !/^\s*$$/; }' -- $(sort $(TOOLS)) | uniq > $@~
	mv -f $@~ $@



$(dir $(lastword $(MAKEFILE_LIST)))/Makefile.manpages.mk: tools-with-pod

tools-with-pod: SHELL = /bin/bash
tools-with-pod: $(TOOLS)
	declare -A tools;\
	for tool in $$(cat $@); do\
	  tools[$$tool]=1;\
	done;\
	for tool in $?; do\
	  if podchecker $$tool; then\
	    tools[$$tool]=1;\
	  else\
	    unset tools[$$tool];\
	  fi;\
	done;\
	echo $${!tools[@]} > $@


.PHONY: manpages install-manpages
manpages install-manpages: $(dir $(lastword $(MAKEFILE_LIST)))/Makefile.manpages.mk
	$(MAKE) -f $< $@
