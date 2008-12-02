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

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFontDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWhatsThis>

#include "colorbutton.h"
#include "configappearancewidget.h"
#include "configdialog.h"
#include "lineedit.h"

ConfigDialog::ConfigDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Configure TikZ editor"));

	m_centerWidget = new QTabWidget;
	m_centerWidget->addTab(generalPage(), tr("&General"));
	m_centerWidget->addTab(typesettingPage(), tr("&Typesetting"));
	m_centerWidget->addTab(appearancePage(this), tr("&Highlighting"));

	m_buttonBox = new QDialogButtonBox;
	QAction *whatsThisAction = QWhatsThis::createAction(this);
	whatsThisAction->setIcon(QIcon(":/images/help-contextual.png"));
	QToolButton *whatsThisButton = new QToolButton(this);
	whatsThisButton->setDefaultAction(whatsThisAction);
	whatsThisButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
	m_buttonBox->addButton(whatsThisButton, QDialogButtonBox::HelpRole);
	m_buttonBox->addButton(QDialogButtonBox::Ok);
	m_buttonBox->addButton(QDialogButtonBox::Cancel);
	connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_centerWidget);
	mainLayout->addWidget(m_buttonBox);
	setLayout(mainLayout);
}

void ConfigDialog::setTranslatedHighlightTypeNames(const QStringList &typeNames)
{
	for (int i = 0; i < typeNames.size(); ++i)
		m_appearance->addItem(typeNames.at(i));
}

void ConfigDialog::setHighlightTypeNames(const QStringList &typeNames)
{
	m_appearance->setTypeNames(typeNames);
}

void ConfigDialog::setDefaultHighlightFormats(const QMap<QString, QTextCharFormat> &defaultFormatList)
{
	m_appearance->setDefaultTextCharFormats(defaultFormatList);
}

void ConfigDialog::readSettings()
{
	m_appearance->readSettings("Highlighting");

	QSettings settings;
	m_numRecentFiles = settings.value("RecentFilesNumber", 10).toInt();
	m_commandsInDock = settings.value("CommandsInDock", false).toBool();
	m_latexCommand = settings.value("LatexCommand", "pdflatex").toString();
	m_pdftopsCommand = settings.value("PdftopsCommand", "pdftops").toString();
	m_editorEdit->setText(settings.value("TemplateEditor", "kwrite").toString());
	m_replaceEdit->setText(settings.value("TemplateReplaceText", "<>").toString());

	settings.beginGroup("MainWindow");
	m_toolBarStyleCombo->setCurrentIndex(settings.value("ToolBarStyle", 0).toInt());
	settings.endGroup();

	settings.beginGroup("Editor");
	const QString textFontString = settings.value("Font", qApp->font().toString()).toString();
	m_textFont.fromString(textFontString);
	m_showMatchingCheck->setChecked(settings.value("ShowMatchingBrackets", true).toBool());
	m_showWhiteSpacesCheck->setChecked(settings.value("ShowWhiteSpaces", false).toBool());
	m_matchingColorButton->setColor(settings.value("ColorMatchingBrackets", Qt::yellow).value<QColor>());
	settings.endGroup();

	setSettings();
}

void ConfigDialog::setSettings()
{
	m_textFontEdit->setText(m_textFont.family() + " " + QString::number(m_textFont.pointSize()));
	m_textFontEdit->setFont(m_textFont);
	m_historyLengthSpinBox->setValue(m_numRecentFiles);
	m_commandsInDockCheck->setChecked(m_commandsInDock);
	m_latexEdit->setText(m_latexCommand);
	m_pdftopsEdit->setText(m_pdftopsCommand);

	m_appearance->setSettings();
}

void ConfigDialog::writeSettings()
{
	m_numRecentFiles = m_historyLengthSpinBox->value();
	m_latexCommand = m_latexEdit->text();
	m_pdftopsCommand = m_pdftopsEdit->text();

	QSettings settings;
	settings.setValue("RecentFilesNumber", m_numRecentFiles);
	settings.setValue("CommandsInDock", m_commandsInDock);
	settings.setValue("LatexCommand", m_latexCommand);
	settings.setValue("PdftopsCommand", m_pdftopsCommand);
	settings.setValue("TemplateEditor", m_editorEdit->text());
	settings.setValue("TemplateReplaceText", m_replaceEdit->text());

	settings.beginGroup("MainWindow");
	settings.setValue("ToolBarStyle", m_toolBarStyleCombo->currentIndex());
	settings.endGroup();

	settings.beginGroup("Editor");
	settings.setValue("Font", m_textFont.toString());
	settings.setValue("ShowMatchingBrackets", m_showMatchingCheck->isChecked());
	settings.setValue("ShowWhiteSpaces", m_showWhiteSpacesCheck->isChecked());
	settings.setValue("ColorMatchingBrackets", m_matchingColorButton->getColor());
	settings.endGroup();

	m_appearance->writeSettings("Highlighting");
}

QWidget *ConfigDialog::generalPage()
{
	QWidget *page = new QWidget;

	// Editor
	const QString textFontWhatsThis = "<p>" + tr("Select the font of the main text.") + "</p>";
	m_textFontEdit = new QLineEdit;
	m_textFontEdit->setReadOnly(true);
	m_textFontEdit->setWhatsThis(textFontWhatsThis);
	QPushButton *textFontButton = new QPushButton(tr("Select"));
	textFontButton->setToolTip(tr("Select font"));
	textFontButton->setWhatsThis("<p>" + tr("Select the font of the main text.") + "</p>");
	connect(textFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));
	QLabel *textFontLabel = new QLabel(tr("Text &font:"));
	textFontLabel->setWhatsThis(textFontWhatsThis);
	textFontLabel->setBuddy(textFontButton);
	QWidget *textFontWidget = new QWidget;
	QHBoxLayout *textFontLayout = new QHBoxLayout;
	textFontLayout->addWidget(textFontLabel);
	textFontLayout->addWidget(m_textFontEdit);
	textFontLayout->addWidget(textFontButton);
	textFontLayout->setMargin(0);
	textFontWidget->setLayout(textFontLayout);

	m_showMatchingCheck = new QCheckBox(tr("Show matching &brackets"));
	m_showMatchingCheck->setWhatsThis("<p>" + tr("If the cursor is on "
	    "a bracket ({[]}), then the corresponding opening/closing "
	    "bracket will be highlighted.") + "</p>");
	m_showWhiteSpacesCheck = new QCheckBox(tr("Show &white spaces"));
	m_showWhiteSpacesCheck->setWhatsThis("<p>" + tr("Show white spaces "
	    "(spaces, tabs) in the text by replacing them with special "
	    "symbols.  These symbols will not be saved on disk.") + "</p>");
	const QString matchingColorWhatsThis = "<p>" + tr("Select the color "
	    "in which the highlighted brackets will be shown.") + "</p>";
	QLabel *matchingColorLabel = new QLabel(tr("Brackets matching &color:"));
	matchingColorLabel->setWhatsThis(matchingColorWhatsThis);
	m_matchingColorButton = new ColorButton();
	m_matchingColorButton->setMinimumSize(24, 24);
	m_matchingColorButton->setMaximumSize(24, 24);
	m_matchingColorButton->setWhatsThis(matchingColorWhatsThis);
	matchingColorLabel->setBuddy(m_matchingColorButton);
	QWidget *showMatchingWidget = new QWidget;
	showMatchingWidget->setEnabled(m_showMatchingCheck->isChecked());
	QHBoxLayout *showMatchingLayout = new QHBoxLayout(showMatchingWidget);
	showMatchingLayout->addSpacing(20);
	showMatchingLayout->addWidget(matchingColorLabel);
	showMatchingLayout->addWidget(m_matchingColorButton);
	showMatchingLayout->addStretch();
	showMatchingLayout->setMargin(0);
	connect(m_showMatchingCheck, SIGNAL(toggled(bool)), showMatchingWidget, SLOT(setEnabled(bool)));

	QVBoxLayout *editorLayout = new QVBoxLayout;
	editorLayout->addWidget(textFontWidget);
	editorLayout->addWidget(m_showWhiteSpacesCheck);
	editorLayout->addWidget(m_showMatchingCheck);
	editorLayout->addWidget(showMatchingWidget);
	QGroupBox *editorBox = new QGroupBox(tr("Editor"));
	editorBox->setLayout(editorLayout);

	// Interface
	QLabel *historyLengthLabel = new QLabel(tr("&Number of entries in the \"Open Recent\" menu:"));
	m_historyLengthSpinBox = new QSpinBox();
	const QString historyLengthWhatsThis = "<p>" + tr("Select the maximum number "
	    "of entries that appear in the \"Open Recent\" menu.  When "
	    "the number of recently opened files exceeds this limit, "
	    "the oldest entries will be lost.") + "</p>";
	historyLengthLabel->setWhatsThis(historyLengthWhatsThis);
	historyLengthLabel->setBuddy(m_historyLengthSpinBox);
	m_historyLengthSpinBox->setWhatsThis(historyLengthWhatsThis);
	QWidget *historyLengthBox = new QWidget;
	QHBoxLayout *historyLengthLayout = new QHBoxLayout;
	historyLengthLayout->addWidget(historyLengthLabel);
	historyLengthLayout->addWidget(m_historyLengthSpinBox);
	historyLengthLayout->setMargin(0);
	historyLengthBox->setLayout(historyLengthLayout);

	m_commandsInDockCheck = new QCheckBox(tr("Show TikZ "
	    "commands in &dock instead of menu"));
	m_commandsInDockCheck->setWhatsThis("<p>" + tr("If this option is "
	    "checked, a dock widget is available with a list of TikZ "
	    "commands which you can insert in your code by clicking on "
	    "them.  If this option is not checked, the TikZ commands will "
	    "be available in a menu instead.") + "</p>");
	connect(m_commandsInDockCheck, SIGNAL(toggled(bool)), this, SLOT(setCommandsInDock(bool)));
	QLabel *commandsInDockLabel = new QLabel(tr("(you have to restart "
	    "this program for this change to take effect)"));
	QVBoxLayout *commandsInDockLayout = new QVBoxLayout;
	commandsInDockLayout->addWidget(m_commandsInDockCheck);
	commandsInDockLayout->addWidget(commandsInDockLabel);
	commandsInDockLayout->setMargin(0);
	commandsInDockLayout->setSpacing(0);
	QWidget *commandsInDockWidget = new QWidget;
	commandsInDockWidget->setLayout(commandsInDockLayout);

	QLabel *toolBarStyleLabel = new QLabel(tr("&Toolbar style:"));
	m_toolBarStyleCombo = new QComboBox();
	const QString toolBarStyleWhatsThis = "<p>" + tr("Select the style in "
	    "which the toolbar will be displayed.") + "</p>";
	toolBarStyleLabel->setWhatsThis(toolBarStyleWhatsThis);
	toolBarStyleLabel->setBuddy(m_toolBarStyleCombo);
	m_toolBarStyleCombo->setWhatsThis(toolBarStyleWhatsThis);
	m_toolBarStyleCombo->addItem(tr("Icons only"));
	m_toolBarStyleCombo->addItem(tr("Text only"));
	m_toolBarStyleCombo->addItem(tr("Text alongside icons"));
	m_toolBarStyleCombo->addItem(tr("Text under icons"));
	QWidget *toolBarStyleBox = new QWidget;
	QHBoxLayout *toolBarStyleLayout = new QHBoxLayout;
	toolBarStyleLayout->addWidget(toolBarStyleLabel);
	toolBarStyleLayout->addWidget(m_toolBarStyleCombo);
	toolBarStyleLayout->addStretch();
	toolBarStyleLayout->setMargin(0);
	toolBarStyleBox->setLayout(toolBarStyleLayout);

	QVBoxLayout *interfaceLayout = new QVBoxLayout;
	interfaceLayout->addWidget(historyLengthBox);
	interfaceLayout->addWidget(commandsInDockWidget);
	interfaceLayout->addWidget(toolBarStyleBox);
	QGroupBox *interfaceBox = new QGroupBox(tr("Interface"));
	interfaceBox->setLayout(interfaceLayout);

	// Put everything together
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(editorBox);
	layout->addWidget(interfaceBox);
	layout->addStretch();
	page->setLayout(layout);

	return page;
}

QWidget *ConfigDialog::typesettingPage()
{
	QWidget *page = new QWidget;

	// Commands
	const QString latexWhatsThis = "<p>" + tr("Enter the path to the LaTeX executable here.") + "</p>";
	QLabel *latexLabel = new QLabel(tr("&LaTeX command:"));
	m_latexEdit = new LineEdit;
	m_latexEdit->setWhatsThis(latexWhatsThis);
	latexLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	latexLabel->setWhatsThis(latexWhatsThis);
	latexLabel->setBuddy(m_latexEdit);
	QToolButton *latexButton = new QToolButton;
	latexButton->setIcon(QIcon(":/images/document-open.png"));
	latexButton->setToolTip(tr("Browse command"));
	latexButton->setWhatsThis("<p>" + tr("Browse to the LaTeX executable.") + "</p>");
	latexButton->setObjectName("latexButton");
	connect(latexButton, SIGNAL(clicked()), this, SLOT(browseCommand()));

	const QString pdftopsWhatsThis = "<p>" + tr("Enter the path to the pdftops executable here.") + "</p>";
	QLabel *pdftopsLabel = new QLabel(tr("&Pdftops command:"));
	m_pdftopsEdit = new LineEdit;
	m_pdftopsEdit->setWhatsThis(pdftopsWhatsThis);
	pdftopsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	pdftopsLabel->setWhatsThis(pdftopsWhatsThis);
	pdftopsLabel->setBuddy(m_pdftopsEdit);
	QToolButton *pdftopsButton = new QToolButton;
	pdftopsButton->setIcon(QIcon(":/images/document-open.png"));
	pdftopsButton->setToolTip(tr("Browse command"));
	pdftopsButton->setWhatsThis("<p>" + tr("Browse to the pdftops executable.") + "</p>");
	pdftopsButton->setObjectName("pdftopsButton");
	connect(pdftopsButton, SIGNAL(clicked()), this, SLOT(browseCommand()));

	QGridLayout *commandsLayout = new QGridLayout;
	commandsLayout->addWidget(latexLabel, 0, 0);
	commandsLayout->addWidget(m_latexEdit, 0, 1);
	commandsLayout->addWidget(latexButton, 0, 2);
	commandsLayout->addWidget(pdftopsLabel, 1, 0);
	commandsLayout->addWidget(m_pdftopsEdit, 1, 1);
	commandsLayout->addWidget(pdftopsButton, 1, 2);
	QGroupBox *commandsBox = new QGroupBox(tr("Commands"));
	commandsBox->setLayout(commandsLayout);

	// Templates
	const QString replaceWhatsThis = "<p>" + tr("Enter the text which will be replaced by the TikZ code in the template here.") + "</p>";
	QLabel *replaceLabel = new QLabel(tr("&Replace text:"));
	m_replaceEdit = new LineEdit;
	m_replaceEdit->setWhatsThis(replaceWhatsThis);
	replaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	replaceLabel->setWhatsThis(replaceWhatsThis);
	replaceLabel->setBuddy(m_replaceEdit);

	const QString editorWhatsThis = "<p>" + tr("Enter the path to the executable of the text editor for the template here.") + "</p>";
	QLabel *editorLabel = new QLabel(tr("&Editor command:"));
	m_editorEdit = new LineEdit;
	m_editorEdit->setWhatsThis(editorWhatsThis);
	editorLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	editorLabel->setWhatsThis(editorWhatsThis);
	editorLabel->setBuddy(m_editorEdit);
	QToolButton *editorButton = new QToolButton;
	editorButton->setIcon(QIcon(":/images/document-open.png"));
	editorButton->setToolTip(tr("Browse command"));
	editorButton->setWhatsThis("<p>" + tr("Browse to the editor executable.") + "</p>");
	editorButton->setObjectName("editorButton");
	connect(editorButton, SIGNAL(clicked()), this, SLOT(browseCommand()));

	QGridLayout *templateLayout = new QGridLayout;
	templateLayout->addWidget(replaceLabel, 0, 0);
	templateLayout->addWidget(m_replaceEdit, 0, 1, 1, 2);
	templateLayout->addWidget(editorLabel, 1, 0);
	templateLayout->addWidget(m_editorEdit, 1, 1);
	templateLayout->addWidget(editorButton, 1, 2);
	QGroupBox *templateBox = new QGroupBox(tr("Templates"));
	templateBox->setLayout(templateLayout);

	// Put everything together
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(commandsBox);
	layout->addWidget(templateBox);
	layout->addStretch();
	page->setLayout(layout);

	return page;
}

QWidget *ConfigDialog::appearancePage(QWidget *parent)
{
	m_appearance = new ConfigAppearanceWidget(parent);

	QWidget *page = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(m_appearance);
	page->setLayout(layout);

	return page;
}

void ConfigDialog::setCommandsInDock(bool inDock)
{
	m_commandsInDock = inDock;
}

void ConfigDialog::selectFont()
{
	bool ok;
	const QFont newFont = QFontDialog::getFont(&ok, m_textFont, this);
	if (ok)
	{
		m_textFont = newFont;
		m_textFontEdit->setText(m_textFont.family() + " " + QString::number(m_textFont.pointSize()));
		m_textFontEdit->setFont(m_textFont);
	}
}

void ConfigDialog::browseCommand(QLineEdit *lineEdit)
{
	QString location = QFileDialog::getOpenFileName(this,
	    tr("Browse program"), QDir::rootPath(),
	    QString("%1 (*)").arg(tr("Program")), 0,
	    QFileDialog::DontResolveSymlinks);
	if (!location.isEmpty())
	{
		location.replace("\\", "/");
		lineEdit->setText(location);
	}
}

void ConfigDialog::browseCommand()
{
	QToolButton *button = qobject_cast<QToolButton*>(sender());
	if (button->objectName() == "latexButton")
		browseCommand(m_latexEdit);
	else if (button->objectName() == "pdftopsButton")
		browseCommand(m_pdftopsEdit);
	else if (button->objectName() == "editorButton")
		browseCommand(m_editorEdit);
}

void ConfigDialog::accept()
{
	writeSettings();
	emit settingsChanged();
	QDialog::accept();
}

void ConfigDialog::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_F1 && event->modifiers() == Qt::ShiftModifier)
	{
		QWhatsThis::enterWhatsThisMode();
	}
	QDialog::keyPressEvent(event);
}
