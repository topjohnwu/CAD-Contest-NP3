#####################################################
# LaTeX Project Makefile
# Create by LeoMao
# Use latexmk, xelatex, asymptote
######################################################
MAINFILE = template
JOBNAME = EDA_final_report

SRCPATH = ./
FIGPATH = ./images

TEX = latexmk
TEXFLAG = -xelatex -synctex=1 -interaction=nonstopmode -use-make

REALTIMEVIEW = -pvc
VIEWER = evince

ASY = asy
ASYFLAG = -f pdf

TEXFILES = $(shell find $(SRCPATH) -type f -name '*.tex')
ASYFILES = $(shell find $(FIGPATH) -type f -name '*.asy')

.PHONY: $(JOBNAME).pdf clean

all: $(JOBNAME).pdf

$(JOBNAME).pdf: $(MAINFILE).tex $(TEXFILES) $(ASYFILES)
	$(TEX) $(TEXFLAG) $(REALTIMEVIEW) -jobname=$(JOBNAME) $(MAINFILE).tex

$(FIGPATH)%.asy:
	$(ASY) $(ASYFLAG) $@

clean:
	latexmk -CA $(JOBNAME).fdb_latexmk
	rm -f *.synctex.gz
	rm -f *.aux
