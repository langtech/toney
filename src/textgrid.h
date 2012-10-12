#ifndef TEXTGRID_H
#define TEXTGRID_H

#include <annotationset.h>
#include <QFile>
#include <QDebug>

struct interval {
    double beg;
    double end;
    QString label;
    int cbeg; // start character offset of the label
    int clen; // lengh of the label
};

struct ann_idx {
    int offset;
    int length;
    Annotation ann;
};

class TextGrid : public AnnotationSet
{
public:
    TextGrid();
    bool open(const QString &path);
    bool saveAs(const QString &filename);

private:
    bool detect_encoding(QFile *file);

private:
    QString _rawtext;
    QVector<struct ann_idx> _offsets;
    QTextCodec *_codec;
    QString _codec_name;
};

#endif // TEXTGRID_H
