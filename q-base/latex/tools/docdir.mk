
include .deps
include .meta


DVIS=$(foreach d,$(DOCS),$d.dvi)
DSTDVIS=$(foreach d,$(DVIS),$(DOCROOT)/dvi/$d)

PDFS=$(foreach d,$(DOCS),$d.pdf)
DSTPDFS=$(foreach d,$(PDFS),$(DOCROOT)/pdf/$d)

ALLGOALS=.deps dvi clean pdf .cvsignore

dummy::
	@echo ===============================================================
	@echo You should explicitely set one of goals: $(ALLGOALS)
	@echo as $(MAKE) argument
	@echo ===============================================================
	exit 1

$(ALLGOALS)::
	for d in $(SUBDIRS); do $(MAKE) -C $$d -f $(DOCROOT)/tools/docdir.mk $@; done

dvi:: $(DSTDVIS)

pdf:: $(DSTPDFS)

clean::
	rm -rf *~ TEMP.* $(foreach e,dvi pdf aux log toc idx ind lof lot, *.$e)

.deps ::
	@if test -z $(DOCROOT); then echo no DOCROOT set; exit 1; fi
	$(DOCROOT)/tools/builddeps $(DOCS) > $@

.cvsignore ::
	@if test -z $(DOCROOT); then echo no DOCROOT set; exit 1; fi
	cp $(DOCROOT)/tools/cvsignore $@

.meta :
	@echo ===============================================================
	@echo You should manually create file with name \'.meta\' and fill it
	@echo with documents and subdirs information.
	@echo See $(DOCROOT)/tools/latexdir.mk
	@echo for samples.
	@echo ===============================================================
	exit 1

%.dvi : %.tex
	@if test -z $(DOCROOT); then echo no DOCROOT set; exit 1; fi
	$(DOCROOT)/tools/latex2dvi $<

%.pdf : %.tex
	@if test -z $(DOCROOT); then echo no DOCROOT set; exit 1; fi
	$(DOCROOT)/tools/latex2pdf $<

$(DOCROOT)/dvi/% : %
	cp $< $@

$(DOCROOT)/pdf/% : %
	cp $< $@

%.pdf : %.tex
	@if test -z $(DOCROOT); then echo no DOCROOT set; exit 1; fi
	$(DOCROOT)/tools/latex2pdf $<
