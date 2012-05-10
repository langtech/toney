/*
 * This material contains unpublished, proprietary software of
 * Entropic Research Laboratory, Inc. Any reproduction, distribution,
 * or publication of this work must be authorized in writing by Entropic
 * Research Laboratory, Inc., and must bear the notice:
 *
 *    "Copyright (c) 1990-1996 Entropic Research Laboratory, Inc.
 *                   All rights reserved"
 *
 * The copyright notice above does not evidence any actual or intended
 * publication of this source code.
 *
 * Written by:  Derek Lin
 * Checked by:
 * Revised by:  David Talkin
 *
 * Brief description:  Estimates F0 using normalized cross correlation and
 *   dynamic programming.
 *
 */


#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sndfile.h>

#include "f0.h"

/*
 * Some consistency checks on parameter values.
 * Return a positive integer if any errors detected, 0 if none.
 */

int check_f0_params(F0_params *par, double sample_freq)
{
  int	  error = 0;
  double  dstep;

  if((par->cand_thresh < 0.01) || (par->cand_thresh > 0.99)) {
    fprintf(stderr,
	    "ERROR: cand_thresh parameter must be between [0.01, 0.99].\n");
    error++;
  }
  if((par->wind_dur > .1) || (par->wind_dur < .0001)) {
    fprintf(stderr,
	    "ERROR: wind_dur parameter must be between [0.0001, 0.1].\n");
    error++;
  }
  if((par->n_cands > 100) || (par->n_cands < 3)){
    fprintf(stderr,
	    "ERROR: n_cands parameter must be between [3,100].\n");
    error++;
  }
  if((par->max_f0 <= par->min_f0) || (par->max_f0 >= (sample_freq/2.0)) ||
     (par->min_f0 < (sample_freq/10000.0))){
    fprintf(stderr,
	    "ERROR: min(max)_f0 parameter inconsistent with sampling frequency.\n");
    error++;
  }
  dstep = ((double)((int)(0.5 + (sample_freq * par->frame_step))))/sample_freq;
  if(dstep != par->frame_step) {
    fprintf(stderr,
	    "Frame step set to %f to exactly match signal sample rate.\n",
	    dstep);
    par->frame_step = dstep;
  }
  if((par->frame_step > 0.1) || (par->frame_step < (1.0/sample_freq))){
    fprintf(stderr,
	    "ERROR: frame_step parameter must be between [1/sampling rate, 0.1].\n");
    error++;
  }

  return(error);
}


F0_params* get_f0_params()
{
    F0_params *par = (F0_params *) malloc(sizeof(F0_params));
    par->cand_thresh = 0.3;
    par->lag_weight = 0.3;
    par->freq_weight = 0.02;
    par->trans_cost = 0.005;
    par->trans_amp = 0.5;
    par->trans_spec = 0.5;
    par->voice_bias = 0.0;
    par->double_cost = 0.35;
    par->min_f0 = 50.0;
    par->max_f0 = 550.0;
    par->frame_step = 0.01;
    par->wind_dur = 0.0075;
    par->n_cands = 20;
    par->mean_f0 = 200.0;     /* unused */
    par->mean_f0_weight = 0.0;  /* unused */
    par->conditioning = 0;    /*unused */
    return par;
}


extern int init_dp_f0(get_f0_session*, double, long*, long*);
extern void close_dp_f0(get_f0_session*);
extern int dp_f0(get_f0_session*, float*, int, int, double, float**, float**, float**, float**, int*, int);


get_f0_session* init_get_f0()
{
    get_f0_session *session = (get_f0_session*) calloc(1, sizeof(get_f0_session));
    session->par = get_f0_params();
    session->dp.first_time = 1;
    session->gcs.ncoeff = 127;
    return session;
}

void close_get_f0(get_f0_session *session)
{
  free(session->par);
  free(session);
}

int get_f0(const char *filename,
           get_f0_session *session,
           double start,
           double end,
           get_f0_callback_t callback,
	   void *user_data)
{
    SF_INFO sfinfo;
    SNDFILE *sndfile;
    sf_count_t start_frame;
    sf_count_t end_frame;
    long total_samps;
    long min_samps;
    long buff_size;
    long sdstep;
    float *fdata;
    long actsize;
    int done;
    float *f0p;
    float *vuvp;
    float *rms_speech;
    float *acpkp;
    int vecsize;

    if (end < start || start < 0.0)
        return 1;

    sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if (sndfile == 0)
        return 1;

    start_frame = start * sfinfo.samplerate;
    end_frame = end * sfinfo.samplerate;

    if (start_frame >= sfinfo.frames)
        return 1;

    if (end_frame > sfinfo.frames)
        end_frame = sfinfo.frames;

    if (check_f0_params(session->par, (double) sfinfo.samplerate)) {
        sf_close(sndfile);
        return 1;
    }

    total_samps = end_frame - start_frame;
    min_samps = (session->par->frame_step * 2.0 + session->par->wind_dur) * sfinfo.samplerate;
    if (total_samps < min_samps) {
        sf_close(sndfile);
        return 1;
    }

    if (init_dp_f0(session, (double) sfinfo.samplerate, &buff_size, &sdstep) != 0) {
        sf_close(sndfile);
        return 1;
    }

    if (buff_size > INT_MAX || sdstep > INT_MAX) {
        sf_close(sndfile);
        close_dp_f0(session);
        return 1;
    }

    fdata = (float*) malloc(sizeof(float) * buff_size);

    if (buff_size > total_samps)
        buff_size = total_samps;

    sf_command(sndfile, SFC_SET_NORM_FLOAT, 0, SF_FALSE);

    sf_seek(sndfile, start_frame, SEEK_SET);

    actsize = sf_read_float(sndfile, fdata, buff_size);

    while (1) {
        done = (actsize < buff_size) || (total_samps == buff_size);

        if (dp_f0(session, fdata, (int) actsize, (int) sdstep, (double) sfinfo.samplerate, &f0p, &vuvp, &rms_speech, &acpkp, &vecsize, done) != 0) {
            sf_close(sndfile);
            close_dp_f0(session);
			free(fdata);
            return 1;
        }

        (*callback)(f0p, vuvp, rms_speech, acpkp, vecsize, user_data);

        if (done)
            break;

        sf_seek(sndfile, sdstep - buff_size, SEEK_CUR);
        actsize = sf_read_float(sndfile, fdata, buff_size);
        total_samps -= sdstep;

        if (actsize > total_samps)
            actsize = total_samps;
    }

    sf_close(sndfile);
    close_dp_f0(session);
	free(fdata);

    return 0;
}

