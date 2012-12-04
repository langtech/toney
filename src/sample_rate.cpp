#include <sndfile.h>

int sample_rate(const char *path)
{
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(path, SFM_READ, &sfinfo);
    if (sndfile == 0)
        return -1;  // error
    int result = sfinfo.samplerate;
    sf_close(sndfile);
    return result;
}
