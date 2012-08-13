#ifndef UTILS_H
#define UTILS_H

bool get_f0_samples(
        char *audio,
        double beg,
        double end,
        get_f0_session *session,
        QVector<float> &result);

#endif // UTILS_H
