/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
** Modified (c) 2009, 2011 by Glad Deschrijver <glad.deschrijver@gmail.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef KTIKZ_LINEEDIT_H
#define KTIKZ_LINEEDIT_H

#include <QtGlobal>
#include <QLineEdit>

class QToolButton;
class UrlCompletion;

class LineEdit : public QLineEdit
{
	Q_OBJECT

public:
	explicit LineEdit(const QString &text, QWidget *parent = 0);
	explicit LineEdit(QWidget *parent = 0);
	virtual QSize sizeHint() const;
	void setCompletionObject(UrlCompletion *urlCompletion);

protected:
	void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
	void updateClearButton(const QString &text);

private:
	void init();

	QToolButton *m_clearButton;
};

#endif // LINEEDIT_H
