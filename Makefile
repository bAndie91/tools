
default:
	false

descriptions.md:
	./gen-descriptions > $@~
	mv $@~ $@
