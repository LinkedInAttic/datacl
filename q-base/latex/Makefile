#

pdf clean .deps .cvsignore::
	@if test -z $(DOCROOT); then echo no DOCROOT set, try to use \'setenv\' utilite; exit 1; fi
	$(MAKE) -C src -f $(DOCROOT)/tools/docdir.mk $@

cleandst::
	rm -rf pdf/*.pdf

