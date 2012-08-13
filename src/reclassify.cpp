#include "annotation.h"
#include "toney_utils.h"

void reclassify(QHash<const Annotation,QString> &s)
{
    QHash<QString,QSet<const Annotation> > clusters;

    QHash<const Annotation,QString>::iterator i = s.begin();
    for (; i != s.end(); ++i) {
        const Annotation ann = i.key(); // see annotation.h for methods
        // ann.getAudioPath()    -- path to the source audio file
        // ann.getTargetLabel()  -- string label for the target region
        // ann.getTargetStart()  -- start offset of the target region
        // ann.getTargetEnd()    -- end offset of the target region
        // ann.getTone()         -- cluster label

        // put annotations into different bins
        clusters[ann.getTone()].insert(ann);
    }

    QHash<QString, QSet<const Annotation> >::iterator j = clusters.begin();
    for (; j != clusters.end(); ++j) {
        if (j.key().isEmpty())
            // unclassified targets
            continue;

        float sum = 0.0;
        int n = 0;
        foreach (const Annotation ann, j.value()) {
            const float *samples = ann.getF0();
            for (int k=0; k < 30; ++k) {
                if (samples[k] > 0) {
                    sum += samples[k];
                    ++n;
                }
            }
        }
        qDebug() << "mean value for (" << j.key() << ")" << sum / n;
    }


}
