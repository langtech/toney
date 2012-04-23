#include <stdio.h>
#include "f0.h"

void callback(
    float *f0p,
    float *vuvp,
    float *rms_speech,
    float *acpkp,
    int vecsize,
    void *userdata
    )
{
    int i = vecsize - 1;
    for (; i >= 0; --i) {
        printf("%f %f %f %f\n", f0p[i], vuvp[i], rms_speech[i], acpkp[i]);
    }
}

int main(int argc, char *argv[])
{
    int i;
    int j;
    for (i=0; i < 1; ++i) {
        get_f0_session *session = init_get_f0();
        for (j=0; j < 2500; ++j) {
            get_f0(argv[1], session, 0, 120, &callback, 0);
        }
        close_get_f0(session);
    }
    return 0;
}
