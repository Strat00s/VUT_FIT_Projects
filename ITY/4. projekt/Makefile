SHELL = /usr/bin/env bash
NAME = proj4
LOGIN = xbasty00

all:
	pdflatex $(NAME).tex
	biber $(NAME).bcf
	pdflatex $(NAME).tex

zip:
	zip $(LOGIN)-fit.zip *.tex *.bib Makefile img/*

clean:
	rm -f $(NAME).{aux,dvi,log,pdf,ps,out,bbl,bcf,blg,run.xml} $(LOGIN)-fit.zip
