/***************************************************************************
 *   Copyright (C) 2007 by Glad Deschrijver                                *
 *   Glad.Deschrijver@UGent.be                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class QCheckBox;
class QDialogButtonBox;
class QLineEdit;
class QSpinBox;
class QTabWidget;
class QTextCharFormat;
class QWidget;
class ConfigAppearanceWidget;
class ColorButton;

class ConfigDialog : public QDialog
{
	Q_OBJECT

public:
	ConfigDialog(QWidget *parent = 0);
	void setTranslatedHighlightTypeNames(const QStringList &typeNames);
	void setHighlightTypeNames(const QStringList &typeNames);
	void setDefaultHighlightFormats(const QMap<QString, QTextCharFormat> &defaultFormatList);
	void readSettings();
	void writeSettings();

signals:
	void settingsChanged();

private:
	void setSettings();
	QWidget *generalPage();
	QWidget *typesettingPage();
	QWidget *appearancePage(QWidget *parent);
	void browseCommand(QLineEdit *lineEdit);
	void keyPressEvent(QKeyEvent *event);

	QTabWidget *m_centerWidget;
	QDialogButtonBox *m_buttonBox;

	QLineEdit *m_textFontEdit;
	QCheckBox *m_showMatchingCheck;
	QCheckBox *m_showWhiteSpacesCheck;
	ColorButton *m_matchingColorButton;

	QSpinBox *m_historyLengthSpinBox;
	QCheckBox *m_commandsInDockCheck;

	QLineEdit *m_latexEdit;
	QLineEdit *m_pdftopsEdit;
	QFont m_textFont;
	int m_numRecentFiles;
	bool m_commandsInDock;
	QString m_latexCommand;
	QString m_pdftopsCommand;

	QLineEdit *m_replaceEdit;
	QLineEdit *m_editorEdit;

	ConfigAppearanceWidget *m_appearance;

private slots:
	void setCommandsInDock(bool inDock);
	void selectFont();
	void browseCommand();
	void accept();
};

#endif
