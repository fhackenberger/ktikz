/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
** Modified (c) 2009 by Glad Deschrijver <glad.deschrijver@gmail.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef KTIKZ_LINEEDIT_H
#define KTIKZ_LINEEDIT_H

#ifdef KTIKZ_USE_KDE
#include <KLineEdit>

class LineEdit : public KLineEdit
{
	Q_OBJECT

public:
	explicit LineEdit(const QString &text, QWidget *parent = 0);
	LineEdit(QWidget *parent = 0);
};
#else
#include <QLineEdit>

class QToolButton;

class LineEdit : public QLineEdit
{
	Q_OBJECT

public:
	explicit LineEdit(const QString &text, QWidget *parent = 0);
	LineEdit(QWidget *parent = 0);
	virtual QSize sizeHint() const;

protected:
	void resizeEvent(QResizeEvent *event);

private slots:
	void updateClearButton(const QString &text);

private:
	void init();

	QToolButton *m_clearButton;
};
#endif

#endif // LINEEDIT_H
