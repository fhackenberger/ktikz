#!/bin/sh

# This shell script compiles a LaTeX file to PDF using the latex, dvips,
# ps2pdf sequence and is supposed to be used in KTikZ only, as a replacement
# for pdflatex.
# Therefore the following assumptions are made in the code of this script:
# - the only options with a value are -output-directory and -interaction
# - the name of the tex file passed to this script always ends on .tex
# - the name of the tex file is the only thing which is not an option or
#   the value of one of the above mentioned options

# Usage:
# - open KTikZ, go to Settings -> Configure KTikZ..., in the "General" tab
#   change the value of PDFLaTeX command to /path/to/ktikzlatex2ps2pdf.sh
# - in the "Template" box change the value to /path/to/ktikzlatex2ps2pdf_template.pgs

options=""
texfile=""

while test x"$1" != x
do
	case $1 in
		-output-directory|-interaction) # add their values to the list of options
			options="$options $1"
			shift
			options="$options $1"
			shift;;
		-*)
			options="$options $1"
			shift;;
		*)
			texfile=$1
			shift;;
	esac
done

texfilebasename=`echo $texfile | sed -e "s/\.tex$//"`

latex $options $texfilebasename.tex
# return when latex fails (otherwise ps2pdf below runs without errors)
latexreturn=$?
if test $latexreturn -ne 0
then
	exit 1
fi

export DVIPSHEADERS=$DVIPSHEADERS:$TEXINPUTS # if a dvips header file is located in the same directory as the template file, then this will make dvips find it
dvips -Ppdf -G0 -o $texfilebasename.ps $texfilebasename.dvi

ps2pdf14 $texfilebasename.ps $texfilebasename.pdf
