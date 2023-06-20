
install:
	@set -e ;\
	for file in *.*.* ;\
	do\
		volume=`echo "$$file" | sed -e 's/.*\.\([^.]\+\)\.[^.]\+$$/\\1/'` ;\
		install --compare "$$file" /usr/share/man/man$$volume/ ;\
	done
