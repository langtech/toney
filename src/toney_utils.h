#ifndef UTILS_H
#define UTILS_H

extern "C" {
#include "get_f0/f0.h"
}
#include "annotation.h"
#include "com.h"

bool get_f0_samples(
        char *audio,
        double beg,
        double end,
        float *result,
        int n_samples = 30,
        get_f0_session *session = 0
        );


void reclassify(QHash<const Annotation, QString> &s, int pos = 0);

// Check and return the sample rate of the given audio file.
// Returns -1 if there's an error.
int sample_rate(const char *audio_path);

#endif // UTILS_H
