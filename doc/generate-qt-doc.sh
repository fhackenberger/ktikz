#!/bin/sh

ksgmltoolsdir="`kde4-config --install data --expandvars`/ksgmltools2"
dtddir="${ksgmltoolsdir}/customization/dtd"
xslfile="${ksgmltoolsdir}/docbook/xsl/html/docbook.xsl"
inputfile="index.docbook"
outputfile="index.html"
#qhpfile="qtikz.qhp"
qchfile="qtikz.qch"
qhcpfile="qtikz.qhcp"
qhcfile="qtikz.qhc"

cleanup()
{
	rm ${outputfile} ${qchfile} ${qhcfile}
}

generate()
{
	xsltproc --path ${dtddir} -o ${outputfile} ${xslfile} ${inputfile}
	sed -e "s/<head>/<head><meta http-equiv=\"Content-Style-Type\" content=\"text\/css\"><link href=\"index.css\" rel=\"stylesheet\" type=\"text\/css\">/" \
	    -e "s/KTikZ/QTikZ/g" \
	    -e "s/cmake/qmake/g" \
	    -e "s/KDE/Qt/g" \
	    -e "s/kdebase and kdelibs from [^,]*,/Qt 4.4/g" \
	    ${outputfile} > ${outputfile}_temp
	mv ${outputfile}_temp ${outputfile}

	#qhelpgenerator ${qhpfile} -o ${qchfile} # this is already done in the following step (see <docFiles> in ${qhcpfile})
	qcollectiongenerator ${qhcpfile} -o ${qhcfile}
}

while test x"$1" != x
do
	case $1 in
		-c|--clean)
			cleanup
			exit 0;;
		*) shift;;
	esac
done

generate
