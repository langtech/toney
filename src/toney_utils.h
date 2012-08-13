#ifndef UTILS_H
#define UTILS_H

extern "C" {
#include "get_f0/f0.h"
}

bool get_f0_samples(
        char *audio,
        double beg,
        double end,
        float *result,
        int n_samples = 30,
        get_f0_session *session = 0
        );

#endif // UTILS_H
