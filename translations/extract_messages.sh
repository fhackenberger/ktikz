#!/bin/sh
BASEDIRS="../app ../common ../part" # roots of translatable sources
PARTDIR="../part"
PROJECT="ktikz" # project name
BUGADDR="http://www.hackenberger.at/" # MSGID-Bugs
WDIR=`pwd` # working dir

exists () {
	type "$1" >/dev/null 2>/dev/null
}

if ! exists extractrc
then
	echo "Error: the script 'extractrc' is not found in your \$PATH."
	echo "Install the kdesdk-scripts (or under Ubuntu the pkg-kde-tools) package. Alternatively, you can get this script at http://websvn.kde.org/trunk/KDE/kdesdk/scripts/extractrc"
	exit 1
fi

echo "Preparing rc files"
# we use simple sorting to make sure the lines do not jump around too much from system to system
# dirty hack: let Qt translate the .ui files instead of KDE (see also app/CMakeLists.txt)
#find ${BASEDIRS} -name '*.rc' -o -name '*.ui' -o -name '*.kcfg' | sort > ${WDIR}/rcfiles.list
(find ${BASEDIRS} -name '*.rc' -o -name '*.kcfg'; find ${PARTDIR} -name '*.ui') | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/rc.cpp
# additional string for KAboutData
echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/rc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/rc.cpp
echo "Done preparing rc files"

echo "Extracting messages"
# see above on sorting
find ${BASEDIRS} -name '*.cpp' -o -name '*.h' -o -name '*.c' | sort > ${WDIR}/infiles.list
echo "rc.cpp" >> ${WDIR}/infiles.list
DBASEDIRS=`echo $BASEDIRS | sed -e "s/\([^\ ]*\)/-D \\1/g"`
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
	-kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
	--msgid-bugs-address="${BUGADDR}" \
	--files-from=${WDIR}/infiles.list ${DBASEDIRS} -D ${WDIR} -o ${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }
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
rm ${WDIR}/rcfiles.list
rm ${WDIR}/infiles.list
rm ${WDIR}/rc.cpp
echo "Done"
