/***************************************************************************
 *   Copyright (C) 2008, 2009, 2012, 2013 by Glad Deschrijver              *
 *     <glad.deschrijver@gmail.com>                                        *
 *   Copyright (C) 2013 by João Carreira <jfmcarreira@gmail.com>           *
 *   Copyright (C) 2016 by G. Prudhomme                                    *
 *     <gprud@users.noreply.github.com>                                    *
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
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "configeditorwidget.h"

#include <QtCore/QSettings>
#include <QTextCodec>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "../common/utils/fontdialog.h"

ConfigEditorWidget::ConfigEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.generalFontButton, SIGNAL(clicked()), this, SLOT(selectFont()));

	// ** Encoding
	initializeEncoding();
}
 
void ConfigEditorWidget::initializeEncoding()
{
	/// Local codec
	QTextCodec* localCodec = QTextCodec::codecForLocale();
	QString localCodecName;
	if (localCodec->aliases().empty())
		localCodecName = codecNameToString( localCodec);
	else
		localCodecName = codecNameToString( localCodec->aliases()[1]);
 
	ui.encodingComboBox->addItem(tr("System local - %1", "Encoding").arg(localCodecName), QString::fromLatin1("System"));
	ui.encodingComboBox->addItem(tr("UTF-8", "Encoding"), QString::fromLatin1("UTF-8 BOM"));
	ui.encodingComboBox->addItem(tr("UTF-8 without BOM", "Encoding"), QString::fromLatin1("UTF-8"));
	ui.encodingComboBox->addItem(tr("(Advanced)", "Encoding"), QString());
	ui.encodingComboBox->setCurrentIndex(ui.encodingComboBox->count()-1);
 
 
 
	QComboBox* cb (ui.defaultEncodingComboBox);
	cb->addItem(tr("(Local codec - %1)", "Encoding").arg(localCodecName), QVariant()
		);
	fillCodecComboBox(cb);
 
 
	cb = ui.readEncodingComboBox;
	cb->addItem(tr("(Local or unicode)", "Encoding"),  QVariant());
	fillCodecComboBox(cb);
 
	cb = ui.writeEncodingComboBox;
	cb->addItem(tr("(Same)", "Encoding"),  QVariant());
	fillCodecComboBox(cb);
}
 

QVariant ConfigEditorWidget::defaultSetting(const QString &key)
{
	if (key == QLatin1String("ShowLineNumberArea"))
		return true;
	else if (key == QLatin1String("Font"))
		return qApp->font().toString();
	else if (key == QLatin1String("ShowWhiteSpaces"))
		return false;
	else if (key == QLatin1String("ShowTabulators"))
		return false;
	else if (key == QLatin1String("ShowMatchingBrackets"))
		return true;
	else if (key == QLatin1String("ColorWhiteSpaces"))
		return QColor(Qt::gray);
	else if (key == QLatin1String("ColorTabulators"))
		return QColor(Qt::gray);
	else if (key == QLatin1String("ColorMatchingBrackets"))
		return QColor(Qt::darkGreen);
	else if (key == QLatin1String("ShowHighlightCurrentLine"))
		return true;
	else if (key == QLatin1String("ColorHighlightCurrentLine"))
	{
		const QColor lineColor(QApplication::palette().color(QPalette::Normal, QPalette::Base));
		return lineColor.darker(105);
	}
	else if (key == QLatin1String("UseCompletion"))
		return true;
	return QVariant();
}

void ConfigEditorWidget::readSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	ui.showLineNumberAreaCheck->setChecked(settings.value(QLatin1String("ShowLineNumberArea"), defaultSetting(QLatin1String("ShowLineNumberArea"))).toBool());
	m_generalFont.fromString(settings.value(QLatin1String("Font"), defaultSetting(QLatin1String("Font"))).toString());
	ui.generalFontEdit->setText(m_generalFont.family() + QLatin1Char(' ') + QString::number(m_generalFont.pointSize()));
	ui.generalFontEdit->setFont(m_generalFont);
	ui.showWhiteSpacesCheck->setChecked(settings.value(QLatin1String("ShowWhiteSpaces"), defaultSetting(QLatin1String("ShowWhiteSpaces"))).toBool());
	ui.showTabulatorsCheck->setChecked(settings.value(QLatin1String("ShowTabulators"), defaultSetting(QLatin1String("ShowTabulators"))).toBool());
	ui.showMatchingBracketsCheck->setChecked(settings.value(QLatin1String("ShowMatchingBrackets"), defaultSetting(QLatin1String("ShowMatchingBrackets"))).toBool());
	ui.whiteSpacesColorButton->setColor(settings.value(QLatin1String("ColorWhiteSpaces"), defaultSetting(QLatin1String("ColorWhiteSpaces"))).value<QColor>());
	ui.tabulatorsColorButton->setColor(settings.value(QLatin1String("ColorTabulators"), defaultSetting(QLatin1String("ColorTabulators"))).value<QColor>());
	ui.matchingBracketsColorButton->setColor(settings.value(QLatin1String("ColorMatchingBrackets"), defaultSetting(QLatin1String("ColorMatchingBrackets"))).value<QColor>());
	ui.highlightCurrentLineCheck->setChecked(settings.value(QLatin1String("ShowHighlightCurrentLine"), defaultSetting(QLatin1String("ShowHighlightCurrentLine"))).toBool());
	ui.highlightCurrentLineColorButton->setColor(settings.value(QLatin1String("ColorHighlightCurrentLine"), defaultSetting(QLatin1String("ColorHighlightCurrentLine"))).value<QColor>());

	ui.useCompletionCheck->setChecked(settings.value(QLatin1String("UseCompletion"), defaultSetting(QLatin1String("UseCompletion"))).toBool());
	
	// encoding
	settings.beginGroup(QLatin1String("encoding"));
		selectEncoding(ui.defaultEncodingComboBox, settings.value(QLatin1String("default")));
		selectEncoding(ui.readEncodingComboBox, settings.value(QLatin1String("decoder")));
		selectEncoding(ui.writeEncodingComboBox, settings.value(QLatin1String("encoder")));
		ui.bomCheckBox->setChecked(settings.value(QLatin1String("bom"), true).toBool());
		selectEncoding(ui.encodingComboBox, settings.value(QLatin1String("preset"), QLatin1String("System")));
	settings.endGroup();
	
	settings.endGroup();
}

void ConfigEditorWidget::writeSettings(const QString &settingsGroup)
{
	QSettings settings;
	settings.beginGroup(settingsGroup);
	settings.setValue(QLatin1String("ShowLineNumberArea"), ui.showLineNumberAreaCheck->isChecked());
	settings.setValue(QLatin1String("Font"), m_generalFont.toString());
	settings.setValue(QLatin1String("ShowWhiteSpaces"), ui.showWhiteSpacesCheck->isChecked());
	settings.setValue(QLatin1String("ShowTabulators"), ui.showTabulatorsCheck->isChecked());
	settings.setValue(QLatin1String("ShowMatchingBrackets"), ui.showMatchingBracketsCheck->isChecked());
	settings.setValue(QLatin1String("ColorWhiteSpaces"), ui.whiteSpacesColorButton->color());
	settings.setValue(QLatin1String("ColorTabulators"), ui.tabulatorsColorButton->color());
	settings.setValue(QLatin1String("ColorMatchingBrackets"), ui.matchingBracketsColorButton->color());
	settings.setValue(QLatin1String("ShowHighlightCurrentLine"), ui.highlightCurrentLineCheck->isChecked());
	settings.setValue(QLatin1String("ColorHighlightCurrentLine"), ui.highlightCurrentLineColorButton->color());
	settings.setValue(QLatin1String("UseCompletion"), ui.useCompletionCheck->isChecked());
	
	settings.beginGroup(QLatin1String("encoding"));
		settings.setValue(QLatin1String("default"),
			ui.defaultEncodingComboBox->itemData(ui.defaultEncodingComboBox->currentIndex()));
		settings.setValue(QLatin1String("encoder"),
			ui.writeEncodingComboBox->itemData(ui.writeEncodingComboBox->currentIndex()));
		settings.setValue(QLatin1String("decoder"),
			ui.readEncodingComboBox->itemData(ui.readEncodingComboBox->currentIndex()));
		settings.setValue(QLatin1String("bom"), ui.bomCheckBox->isChecked());
		settings.setValue(QLatin1String("preset"),
			ui.encodingComboBox->itemData(ui.encodingComboBox->currentIndex()));
	settings.endGroup();
	settings.endGroup();
}

void ConfigEditorWidget::selectFont()
{
	bool ok;
	const QFont newFont = FontDialog::getFont(&ok, m_generalFont, this);
	if (ok)
	{
		m_generalFont = newFont;
		ui.generalFontEdit->setText(m_generalFont.family() + QLatin1Char(' ') + QString::number(m_generalFont.pointSize()));
		ui.generalFontEdit->setFont(m_generalFont);
	}
}


class ComboItem
{
public:
	ComboItem() {}
	ComboItem(const QString &text, const QVariant &data):
		text(text), data(data)
	{}
	QString text;
	QVariant data;
	bool operator <(const ComboItem &other) const {return QString::compare(this->text,  other.text, Qt::CaseInsensitive) < 0;}
};
void ConfigEditorWidget::fillCodecComboBox(QComboBox *cb)
{
	const QList<QByteArray>  ca (QTextCodec::availableCodecs());
	QVector<ComboItem> ciList; ciList.reserve(ca.length());
	Q_FOREACH(const QByteArray &ba , ca)
		ciList.append(ComboItem(codecNameToString(ba), ba));
	qSort(ciList);
	Q_FOREACH (const ComboItem& ci, ciList) {
		cb->addItem(ci.text, ci.data);
	}
}
void ConfigEditorWidget::selectEncoding(QComboBox *cb, const QVariant &codecName)
{
	for(int i=0; i<cb->count(); ++i) {
		if(cb->itemData(i) == codecName)
		{
			cb->setCurrentIndex(i);
			return ;
		}
	}
}



void ConfigEditorWidget::on_encodingComboBox_currentIndexChanged(int index)
{
	bool b = (index == -1);
	//Q_ASSERT(b);
	if(Q_LIKELY( ~b) ) // true when the widget is initialized...
	{
		QString key=ui.encodingComboBox->itemData(index).toString();
		if (key.isEmpty())
		{
			b = true;
		}
		else
		{
			QVariant de, re, we;
			bool bom ;
			if (key.compare(QString::fromLatin1("UTF-8+BOM"))==0)
			{
				de = QString::fromLatin1("UTF-8");
				we = de;
				bom = true;
			}
			else if (key.compare(QString::fromLatin1("UTF-8"))==0)
			{
				de = QString::fromLatin1("UTF-8");
				we = de;
				bom = false;
			}
			else // if (key.compare(QString::fromLatin1("System"))==0)
			{
				bom = true;
			}

			selectEncoding(ui.defaultEncodingComboBox,de);
			selectEncoding(ui.readEncodingComboBox,re);
			selectEncoding(ui.writeEncodingComboBox,we);
			ui.bomCheckBox->setChecked(bom);
		}
	}
	ui.encodingAdvWidget->setVisible(b);


}

QString ConfigEditorWidget::codecNameToString(const QByteArray& codecName)
{
	return QString::fromLatin1(codecName.constData(), codecName.length());
}

QString ConfigEditorWidget::codecNameToString(QTextCodec *codec){return codecNameToString(codec->name());}
