TIKZCOMMANDS_TR_H = tikzcommands_tr.h
XMLPATTERNS = xmlpatterns
LUPDATE = lupdate
SED = sed

contains(QT_VERSION, ^4\\.[0-6]\\..*) {
	ts.commands = @echo ERROR: This Qt version is too old for the ts target. Need Qt 4.7+.
} else {
	system($${SED} -n q $${_PRO_FILE_}) {
		ts.commands += \
			$${XMLPATTERNS} -output $${TIKZCOMMANDS_TR_H}_tmp $${_PRO_FILE_PWD_}/extract-tikzcommands.xq && \
			$${SED} -e "s/amp\\;//g" $${TIKZCOMMANDS_TR_H}_tmp > $${TIKZCOMMANDS_TR_H} && \
			(cd $${_PRO_FILE_PWD_} && $${LUPDATE} . ../common ../common/utils $${OUT_PWD}/$${TIKZCOMMANDS_TR_H} -ts $${TRANSLATIONS} && \
			$${QMAKE_DEL_FILE} $${OUT_PWD}/$${TIKZCOMMANDS_TR_H}_tmp && \
			$${QMAKE_DEL_FILE} $${OUT_PWD}/$${TIKZCOMMANDS_TR_H})
	} else {
		ts.commands = @echo ERROR: $${SED} is not found on your system.  Please install it if you want to use the 'make ts' target.
	}
}
QMAKE_EXTRA_TARGETS += ts
