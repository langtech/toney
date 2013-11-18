#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "f0.h"

void usage(const char *msg)
{
    printf("Usage: get_f0_60_650_10_5 FILE START END\n"
           "\n"
           "  FILE    input audio file\n"
           "  START   start time of the region to analyze\n"
           "  END     end time of the region to analyze\n"
           "\n");
    if (msg)
        printf("%s\n\n", msg);
    exit(1);
}

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
    if (argc != 4)
        usage(0);
    struct stat stat_buf;
    if (stat(argv[1], &stat_buf) == -1) {
        usage(strerror(errno));
    }
    char *p;
    float beg = strtof(argv[2], &p);
    if (p != argv[2] + strlen(argv[2])) {
        usage("start time must be a number");
    }
    float end = strtof(argv[3], &p);
    if (p != argv[3] + strlen(argv[3])) {
        usage("end time must be a number");
    }

    get_f0_session *session = init_get_f0();
    session->par->min_f0 = 60.0;
    session->par->max_f0 = 650.0;
    session->par->wind_dur = 0.01;
    session->par->frame_step = 0.005;
    get_f0(argv[1], session, beg, end, &callback, 0);
    close_get_f0(session);
    return 0;
}
