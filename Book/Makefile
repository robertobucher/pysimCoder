BOOK = BookPythonForControl
all: $(BOOK).pdf

$(BOOK).pdf: $(BOOK).ps $(BOOK).bbl
	ps2pdf $(BOOK).ps

$(BOOK).ps: $(BOOK).dvi
	dvips $(BOOK).dvi

$(BOOK).dvi: $(BOOK).tex PyControl.tex
	latex $(BOOK).tex
	latex $(BOOK).tex

$(BOOK).bbl: $(BOOK).aux robi.bib
	bibtex $(BOOK).aux

$(BOOK).aux: $(BOOK).dvi

