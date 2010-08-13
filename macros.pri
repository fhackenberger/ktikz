# Variables

unix:!macx {
	QMAKE_CXXFLAGS += `pkg-config --cflags poppler-qt4`
	QMAKE_LFLAGS += `pkg-config --libs poppler-qt4`
}

# Functions

defineReplace(formSources) {
	names = $$ARGS
	sourceNames =
	for(name, names) {
		baseName = $$replace(name, "\\.ui", "")
		sourceName = $${baseName}.cpp
		exists($$sourceName) {
			sourceNames += $$sourceName
		}
	}
	return($$sourceNames)
}

defineReplace(headerFiles) {
	names = $$ARGS
	headerNames =

	for(name, names) {
		baseName = $$replace(name, "\\.cpp", "")
		headerName = $${baseName}.h
		exists($$headerName) {
			headerNames += $$headerName
		}
	}
	return($$headerNames)
}

defineReplace(qmFiles) {
	names = $$ARGS
	qmfiles =

	for(name, names) {
		baseName = $$replace(name, "\\.ts", "")
		qmName = $${OUT_PWD}/$${LOCALEDIR}$${baseName}.qm
#		exists($$qmName) {
			qmfiles += $$qmName
#		}
	}
	return($$qmfiles)
}
