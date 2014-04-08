### Input

LANGUAGES = cs de es fr
TRANSLATIONS = $$tsFilesInDir($${PWD}, $${LANGUAGES})

### Output

!isEmpty(TRANSLATIONS) {
	updateqm.name = lrelease ${QMAKE_FILE_IN}
	updateqm.input = TRANSLATIONS
	updateqm.output = locale/${QMAKE_FILE_BASE}.qm
	updateqm.commands = $${LRELEASECOMMAND} -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
	updateqm.CONFIG = no_link target_predeps
	QMAKE_EXTRA_COMPILERS += updateqm

	translations.path = $${TRANSLATIONS_INSTALL_DIR}
	translations.files += $$qmFiles($${OUT_PWD}/locale, $${TRANSLATIONS})
	translations.CONFIG += no_check_exist
	INSTALLS += translations
}

### Generate ts files (only to be used when ../app/tikzcommands.xml has changed)

TIKZCOMMANDS_TR_H = tikzcommands_tr.h
XMLPATTERNS = xmlpatterns
LUPDATE = lupdate
SED = sed

contains(QT_VERSION, ^4\\.[0-6]\\..*) {
	ts.commands = @echo ERROR: This Qt version is too old for the ts target. Need Qt 4.7+.
} else {
	system($${SED} -n q $${_PRO_FILE_}) { # if sed is available
		ts.commands += \
			$${XMLPATTERNS} -output $${TIKZCOMMANDS_TR_H}_tmp $${PWD}/../app/extract-tikzcommands.xq && \
			$${SED} -e "s/amp\\;//g" $${TIKZCOMMANDS_TR_H}_tmp > $${TIKZCOMMANDS_TR_H} && \
			$${LUPDATE} $${PWD}/../app $${PWD}/../common $${PWD}/../common/utils $${OUT_PWD}/$${TIKZCOMMANDS_TR_H} -ts $${TRANSLATIONS} && \
			$${QMAKE_DEL_FILE} $${OUT_PWD}/$${TIKZCOMMANDS_TR_H}_tmp && \
			$${QMAKE_DEL_FILE} $${OUT_PWD}/$${TIKZCOMMANDS_TR_H}
	} else {
		ts.commands = @echo ERROR: $${SED} is not found on your system.  Please install it if you want to use the 'make ts' target.
	}
}
QMAKE_EXTRA_TARGETS += ts
