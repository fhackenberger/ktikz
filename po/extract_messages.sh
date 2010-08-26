#!/bin/sh
#BASEDIR="../" # root of translatable sources
BASEDIRS="../app ../common ../part" # roots of translatable sources
PARTDIR="../part"
PROJECT="ktikz" # project name
BUGADDR="http://www.hackenberger.at/" # MSGID-Bugs
WDIR=`pwd` # working dir

TIKZCOMMANDSLIST=lists/tikzcommands.list

add_list()
{
	LIST=$1

	if [ -z "$LIST" ]
	then
		echo ">>ERR<< add_list() - missing parameter LIST - exiting"
		return
	fi

	if [ -f "$LIST" ]
	then
		cat $LIST | while read ROW
		do
			echo "tr2i18n(\"${ROW}\")" >> ${WDIR}/rc.cpp
		done
	else
		echo ">>ERR<< add_list() - file $LIST does not exist."
	fi
}

echo "Preparing rc files"
#cd ${BASEDIR}
# we use simple sorting to make sure the lines do not jump around too much from system to system
#find . -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > ${WDIR}/rcfiles.list
# dirty hack: let Qt translate the .ui files instead of KDE
#find ${BASEDIRS} -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > ${WDIR}/rcfiles.list
(find ${BASEDIRS} -name '*.rc' -o -name '*.kcfg'; find ${PARTDIR} -name '*.ui') | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/rc.cpp
# additional string for KAboutData
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/rc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/rc.cpp

cd ${WDIR}

# Add tikz commands descriptions
#add_list "$TIKZCOMMANDSLIST"

echo "Done preparing rc files"


echo "Extracting messages"
#cd ${BASEDIR}
# see above on sorting
#find . -name '*.cpp' -o -name '*.h' -o -name '*.c' | sort > ${WDIR}/infiles.list
find ${BASEDIRS} -name '*.cpp' -o -name '*.h' -o -name '*.c' | sort > ${WDIR}/infiles.list
echo "rc.cpp" >> ${WDIR}/infiles.list
cd ${WDIR}
DBASEDIRS=`echo $BASEDIRS | sed -e "s/\([^\ ]*\)/-D \\1/g"`
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
	-kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
	--msgid-bugs-address="${BUGADDR}" \
	--files-from=infiles.list ${DBASEDIRS} -D ${WDIR} -o ${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }
echo "Done extracting messages"


echo "Merging translations"
catalogs=`find . -name '*.po'`
for cat in $catalogs; do
	echo $cat
	msgmerge -o $cat.new $cat ${PROJECT}.pot
	mv $cat.new $cat
done
echo "Done merging translations"


echo "Cleaning up"
cd ${WDIR}
rm rcfiles.list
rm infiles.list
rm rc.cpp
echo "Done"
