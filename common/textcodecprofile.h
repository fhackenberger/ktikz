#ifndef TEXTCODECPROFILE_H
#define TEXTCODECPROFILE_H

class QTextStream;

class TextCodecProfile{
public:
    /* /// Return the QTextCodec for saving TeX files.
    /// @return Non-null instance of QTextCodec.
    virtual QTextCodec* getEncoder() const
    {
        return QTextCodec::codecForLocale();
    }*/
    /// Configure a QTextStream to encode a TeX file.
    /// @arg textStream A non-null instance of QTextStream.
    void configureStreamEncoding(QTextStream& textStream) const
    {
        Q_UNUSED(textStream);
    }
    /// Configure a QTextStream to decode a TeX file.
    /// @arg textStream A non-null instance of QTextStream.
    void configureStreamDecoding(QTextStream& textStream) const
    {
        Q_UNUSED(textStream);
    }
};

#endif // TEXTCODECPROFILE_H
