#include "textgrid.h"
#include <QRegExp>

TextGrid::TextGrid():
    AnnotationSet()
{
}

bool TextGrid::open(const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly) == false) {
        return false;
    }

    _anns.clear();
    _offsets.clear();
    _path = path;

    _rawtext = QString::fromUtf8(file.readAll());
    QRegExp p_item("\\s+item\\s*\\[\\d+\\]\\s*:");
    QRegExp p_name("\\s+name\\s*=\\s*\"([^\"]+)\"");
    QRegExp p_interval("\\s+intervals\\s+");
    QRegExp p_min("\\s+xmin\\s*=\\s*([0-9.]+)");
    QRegExp p_max("\\s+xmax\\s*=\\s*([0-9.]+)");
    QRegExp p_text("\\s+text\\s*=\\s*\"([^\"]*)\"");

    QHash<QString,QVector<struct interval> > layers;

    // parse TextGrid file and store tiers to "layers"
    int i, pos = 0;

    QVector<int> item_start;

    while (1) {
        i = p_item.indexIn(_rawtext, pos);
        if (i == -1) break;
        pos = i + p_item.matchedLength();
        item_start.push_back(pos);
    }
    item_start.push_back(_rawtext.length());

    pos = item_start.first();
    item_start.pop_front();

    foreach (int b, item_start) {

        // find "name = ..."
        if (p_name.indexIn(_rawtext, pos) == -1) break;

        QVector<struct interval> &v = layers[p_name.cap(1)];

        pos = p_name.pos() + p_name.matchedLength();

        // find "intervals ..."
        if (p_interval.indexIn(_rawtext, pos) == -1) break;

        pos = p_interval.pos() + p_interval.matchedLength();

        while (1) {
            p_min.indexIn(_rawtext, pos);
            p_max.indexIn(_rawtext, pos);
            p_text.indexIn(_rawtext, pos);

            if (p_min.pos() == -1 || p_max.pos() == -1 || p_text.pos() == -1)
                break;

            if (p_min.pos() >= b || p_max.pos() >= b || p_text.pos() >= b)
                break;

            bool ok;
            double beg = p_min.cap(1).toDouble(&ok);
            if (!ok) return false;
            double end = p_max.cap(1).toDouble(&ok);
            if (!ok) return false;

            struct interval t = {beg, end, "", -1, -1};

            if (p_text.pos(1) != -1) {
                t.label = p_text.cap(1);
                t.cbeg = p_text.pos(1);
                t.clen = t.label.size();
            }
            v.push_back(t);

            pos = p_text.pos() + p_text.matchedLength();
        }

        pos = b;
    }

    int frm_i = 0;
    int spkr_i = 0;
    const QVector<struct interval> &frm_v = layers["frame"];
    const QVector<struct interval> &spkr_v = layers["speaker"];
    QRegExp pat("(.*):(.*)");

    foreach (const struct interval &t, layers["target"]) {
        if (t.label.isEmpty())
            continue;

        while ((t.beg >= frm_v[frm_i].end ||
                t.end <= frm_v[frm_i].beg) && frm_i < frm_v.size()) {
            frm_i++;
        }
        while ((t.beg >= spkr_v[spkr_i].end ||
                t.end <= spkr_v[spkr_i].beg) && spkr_i < spkr_v.size()) {
            spkr_i++;
        }
        Annotation ann;

        QStringList values = t.label.split(QRegExp("\\s*:\\s*"));
        int pos = 0;
        foreach (QString val, values) {
            ann.setValue(pos++, val);
        }
        ann.setTargetStart(t.beg);
        ann.setTargetEnd(t.end);

        if (frm_i < frm_v.size()) {
            const struct interval &frm = frm_v[frm_i];
            ann.setFrame(frm.label, frm.beg, frm.end);
        }
        if (spkr_i < spkr_v.size())
            ann.setSpeaker(spkr_v[spkr_i].label);

        _anns.insert(ann);

        struct ann_idx aidx = {t.cbeg, t.clen, ann};
        _offsets.push_back(aidx);
    }

    resetModificationFlag();
    return true;
}

bool TextGrid::saveAs(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::WriteOnly) == false) {
        return false;
    }
    QTextStream ts(&file);
    int idx = 0;
    foreach (const struct ann_idx& w, _offsets) {
        ts << _rawtext.mid(idx, w.offset - idx);
        ts << w.ann.getValues().join(":");
        idx = w.offset + w.length;
    }
    ts << _rawtext.mid(idx);
    resetModificationFlag();
    return true;
}
