#include "annotation.h"
#include "toney_utils.h"

static float sum_f0_samples(const Annotation &ann)
{
    float sum = 0.0;
    const float *samples = ann.getF0();
    for (int i=0; i < Annotation::NUM_F0_SAMPLES; ++i) {
        if (samples[i] > 0)
            sum += samples[i];
    }
    return sum;
}

// Param s: A hash of Annotations. The values are empty string. The reclassify
//   function should update the values to an appropriate cluster label.
void reclassify(QHash<const Annotation,QString> &s)
{
    QHash<QString,float> cluster_mean;
    QHash<QString,int> cluster_num_samples;
    QHash<const Annotation,float> word_mean;

    QHash<const Annotation,QString>::iterator i = s.begin();

    for (; i != s.end(); ++i) {
        const Annotation ann = i.key();

        // See annotation.h for methods of Annotation class.
        // ann.getAudioPath()    -- path to the source audio file
        // ann.getTargetLabel()  -- word label
        // ann.getTargetStart()  -- start offset for the word
        // ann.getTargetEnd()    -- end offset for the word
        // ann.getTone()         -- cluster label

        QString cluster = ann.getTone();
        float v = sum_f0_samples(ann);
        if (cluster != "") {
            cluster_mean[cluster] += v;
            cluster_num_samples[cluster] += Annotation::NUM_F0_SAMPLES;
        }
        word_mean[ann] = v / Annotation::NUM_F0_SAMPLES;
    }

    foreach (QString cluster, cluster_mean.keys())
        cluster_mean[cluster] /= cluster_num_samples[cluster];

    for (i = s.begin(); i != s.end(); ++i) {
        QHash<QString,float>::iterator c = cluster_mean.begin();
        for (; c != cluster_mean.end(); ++c) {
            const Annotation ann = i.key();
            float wm = word_mean[ann];  // word mean
            float cm = c.value();       // cluster mean
            float diff = fabsf(wm - cm);
            if (diff < 1.0) {
                //qDebug() << ann.getTargetLabel() << ann.getTone() << "-->" << c.key();
                i.value() = c.key();
            }
        }
    }
}
