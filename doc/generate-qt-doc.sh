#!/bin/sh

inputfile="index.docbook"
outputfile="index.html"
#qhpfile="qtikz.qhp"
qchfile="qtikz.qch"
qhcpfile="qtikz.qhcp"
qhcfile="qtikz.qhc"

usage()
{
	progname=`echo $0 | sed -e "s/[^\/]*\///g"`
	cat << EOF
Usage: $progname [OPTIONS]
Options:
  -c|--clean            remove ${qchfile} and ${qhcfile}
  --qhc                 generate ${qhcfile}; without this option,
                        only ${outputfile} is generated
EOF
}

cleanup()
{
	rm ${outputfile} ${qchfile} ${qhcfile}
}

generate_html()
{
	ksgmltoolsdir="`kde4-config --install data --expandvars`/ksgmltools2"
	dtddir="${ksgmltoolsdir}/customization/dtd"
	xslfile="${ksgmltoolsdir}/docbook/xsl/html/docbook.xsl"

	xsltproc --path ${dtddir} -o ${outputfile} ${xslfile} ${inputfile}
	sed -e "s/<head>/<head><meta http-equiv=\"Content-Style-Type\" content=\"text\/css\"><link href=\"index.css\" rel=\"stylesheet\" type=\"text\/css\">/" \
	    -e "s/KTikZ/QTikZ/g" \
	    -e "s/cmake/qmake/g" \
	    -e "s/KDE/Qt/g" \
	    -e "s/kdebase and kdelibs from [^,]*,/<span class=\"application\">Qt<\/span> 4.4/g" \
	    ${outputfile} > ${outputfile}_temp
	# Remove information about "Report Bug" and "Switch Application Language" items in the Help menu which do not exist in the Qt-only version
	sed -e ":a;s/<dt><span class=\"term\"><span class=\"guimenu\">Help<\/span>-&gt;<span class=\"guimenuitem\">Report Bug.*for this application\.<\/p><\/dd>//;/</N;//ba" \
	    ${outputfile}_temp > ${outputfile}
	# Remove duplicate "Qt 4.4" in the "Compilation and Installation" section
	# Remove instructions about "Show Statusbar", "Configure Shortcuts" and "Configure Toolbars" which do not exist in the Qt-only version
	sed -e "s/<span class=\"application\">Qt<\/span> 4.4 <span/<span/" \
	    -e "/term-commands-show-statusbar/,/toolbar\.<\/p><\/dd>/d" \
	    ${outputfile} > ${outputfile}_temp
	mv ${outputfile}_temp ${outputfile}
}

generate_qhc()
{
	generate_html

	#qhelpgenerator ${qhpfile} -o ${qchfile} # this is already done in the following step (see <docFiles> in ${qhcpfile})
	qcollectiongenerator ${qhcpfile} -o ${qhcfile}
}

while test x"$1" != x
do
	case $1 in
		-h|--help)
			usage
			exit 0;;
		-c|--clean)
			cleanup
			exit 0;;
		--qhc)
			generate_qhc
			exit 0;;
		*) shift;;
	esac
done

generate_html
