/*
  input:
    - audio file (1 channel)
    - start time
    - end time

  output:
    16kHz 16-bit PCM wav file

  Compute f0 samples for given audio segment.
  Synthesize tones of frequencies given by the f0 samples.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.h>
#include <f0.h>

void usage(char *prog)
{
  printf("Usage: %s <audio file> <start time> <end time> <out audio file>\n\n", prog);
}

typedef struct {
  float *data;
  int wptr;      /* current write position in the data array */
} f0_samples_t;

void get_f0_callback(float *f0p,
		     float *vuvp,
		     float *rms_speech,
		     float *acpkp,
		     int vecsize,
		     void *userdata)
{
  f0_samples_t *v = (f0_samples_t*) userdata;

  int i;

  for (i=vecsize; i >= 0; --i) {
    v->data[v->wptr++] = f0p[i];
  }
}

void generate_sine_wave(f0_samples_t *obj, float step, char *filename)
{
  float pi = 3.14159265;
  float amplitude = 5000.0;
  float samplerate = 16000.0;

  SF_INFO sfinfo = {
    .samplerate = (int) samplerate,
    .format = SF_FORMAT_WAV | SF_FORMAT_PCM_16,
    .channels = 1
  };

  SNDFILE *sndfile = sf_open(filename, SFM_WRITE, &sfinfo);

  float c = pi * 2.0 / samplerate;
  int i = 0;
  float t = 0.0;
  float theta = 0.0;
  short buf[(int) (step * samplerate)];
  int counter = 0;
  for (; i < obj->wptr; ++i) {
    float delta = c * obj->data[i];
    if (delta < 0.00001)
      theta = 0.0;
    counter = 0;
    while (t < i * step) {
      float v = sin(theta) * amplitude;
      buf[counter++] = v;
      //printf("%f %f\n", t,v);
      theta += delta;
      theta -= floor(theta / (2.0 * pi)) * 2.0 * pi;
      t += 1.0 / samplerate;
    }
    sf_writef_short(sndfile, buf, counter);
  }

  sf_close(sndfile);
}
		     
int main(int argc, char *argv[])
{
  if (argc != 5) {
    usage(argv[0]);
    exit(1);
  }

  char *audiofile = argv[1];
  float beg = atof(argv[2]);
  float end = atof(argv[3]);
  char *out_audiofile = argv[4];

  SF_INFO sfinfo;
  SNDFILE* sndfile = sf_open(audiofile, SFM_READ, &sfinfo);
  
  if (sfinfo.channels != 1) {
    printf("There are more than 1 channels in the audio file.\n");
    exit(1);
  }

  if (beg < 0.0) {
    fprintf(stderr, "Warning: start offset is set to 0.0\n");
    beg = 0.0;
  }

  if (end <= beg) {
    printf("Error: end offset is smaller than start offset\n");
    exit(1);
  }

  sf_close(sndfile);

  get_f0_session *session = init_get_f0();

  session->par->min_f0 = 60.0;
  session->par->max_f0 = 650.0;
  session->par->wind_dur = 0.01;
  session->par->frame_step = 0.005;

  f0_samples_t f0_samples;
  int num_f0_samples = (int) ((end - beg) / session->par->frame_step + 0.5);
  f0_samples.data = (float*) malloc(sizeof(float) * num_f0_samples);
  f0_samples.wptr = 0;

  get_f0(audiofile, session, beg, end, get_f0_callback, &f0_samples);

  generate_sine_wave(&f0_samples, session->par->frame_step, out_audiofile);

  close_get_f0(session);

  free(f0_samples.data);

  return 0;
}
