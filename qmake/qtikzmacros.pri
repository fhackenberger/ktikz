# Variables

unix:!macx {
	PKG_CONFIG = $$pkgConfigExecutable()
    POPPLERINCLUDES = $$system($$PKG_CONFIG --cflags poppler-qt5)
    POPPLERLIBS = $$system($$PKG_CONFIG --libs poppler-qt5)
	QMAKE_CXXFLAGS += $$POPPLERINCLUDES
	QMAKE_LFLAGS += $$POPPLERLIBS
}

# Functions

defineTest(QtVersionGreaterThan) {
	isEqual(QT_MAJOR_VERSION, $$1)|greaterThan(QT_MAJOR_VERSION, $$1) {
		isEqual(QT_MINOR_VERSION, $$2)|greaterThan(QT_MINOR_VERSION, $$2) {
			isEqual(QT_PATCH_VERSION, $$3)|greaterThan(QT_PATCH_VERSION, $$3) {
				return(true)
			}
		}
	}
	return(false)
}

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

defineReplace(replaceArgs) {
	template = $$member(ARGS, 0)
	args = $$replace(ARGS, $${template}, "")
	itemList =

	for(arg, args) {
		item = $$replace(template, "%1", $$arg)
		itemList += $$item
	}
	return($$itemList)
}

defineReplace(tsFilesInDir) {
	dir = $$1
	languages = $$2
	tsfiles =

	for(lang, languages) {
		tsName = $${dir}/$${lang}/qtikz_$${lang}.ts
		exists($$tsName) {
			tsfiles += $$tsName
		}
	}
	return($$tsfiles)
}

defineReplace(qmFiles) {
	dir = $$1
	names = $$2
	qmfiles =

	for(name, names) {
		baseName = $$replace(name, "\\.ts", "")
		baseName = $$replace(baseName, ".*\\/", "")
		qmName = $${dir}/$${baseName}.qm
		qmfiles += $$qmName
	}
	return($$qmfiles)
}

defineReplace(manTemplateFilesInDir) {
	dir = $$member(ARGS, 0)
	languages = $$replace(ARGS, $${dir}, "")
	manfiles =

	for(lang, languages) {
		manName = $${dir}/$${lang}/qtikz.1.template
		exists($$manName) {
			manfiles += $$manName
		}
	}
	return($$manfiles)
}

defineReplace(manFiles) {
	names = $$ARGS
	manfiles =

	for(name, names) {
		baseName = $$replace(name, "\\.template", "")
		baseName = $$replace(baseName, ".*\\/", "")
		manName = $${OUT_PWD}/$${LOCALEDIR}$${baseName}
		manfiles += $$manName
	}
	return($$qmfiles)
}

defineReplace(addPrefix) {
	prefix = $$member(ARGS, 0)
	names = $$replace(ARGS, $${prefix}, "")
	files =

	for(name, names) {
		newName = $${prefix}$${name}
		files += $$newName
	}
	return($$files)
}

defineReplace(addSuffix) {
	suffix = $$member(ARGS, 0)
	names = $$replace(ARGS, $${suffix}, "")
	files =

	for(name, names) {
		newName = $${name}$${suffix}
		files += $$newName
	}
	return($$files)
}
