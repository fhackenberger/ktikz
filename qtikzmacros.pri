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
	dir = $$member(ARGS, 0)
	languages = $$replace(ARGS, $${dir}, "")
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
	names = $$ARGS
	qmfiles =

	for(name, names) {
		baseName = $$replace(name, "\\.ts", "")
		baseName = $$replace(baseName, ".*\\/", "")
		qmName = $${OUT_PWD}/$${LOCALEDIR}$${baseName}.qm
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
