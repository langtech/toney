#include "toney_utils.h"
#include <sndfile.h>
#include <QString>
#include <QVector>
#include <cmath>
#include <QDebug>

#include <iostream>

static void callback(
        float *f0p,
        float *vuvp,
        float *rms_speech,
        float *acpkp,
        int vecsize,
        void *userdata)
{
    Q_UNUSED(rms_speech);
    Q_UNUSED(acpkp);
    QVector<float> *v = (QVector<float>*) userdata;
    for (int i=0; i < vecsize; ++i) {
        v->push_front(vuvp[i]);
        v->push_front(f0p[i]);
    }
}

static void linear_interpolation(QVector<float> &f0_samples,
                                 QVector<float> &output,
                                 float start,
                                 float duration,
                                 float step)
{

    std::cout << "linear_interpolation mammoth" << std::endl;

    static const float E = 0.000001;

    int N = f0_samples.size();

    std::cout << "N = " << N << std::endl;
    std::cout << "Values: ";
    for (int j = 0; j < N; j++) {
        std::cout << f0_samples.at(j) << " ";
    }
    std::cout << std::endl;

    if (N == 0)
        return;

    int i = 0;
    float tmp_val;

    output.clear();

    QVector<float> input(f0_samples);

    /*
    i = (int) floor(start / step);
    std::cout << "first i = " << i << std::endl;
    tmp_val = f0_samples.value(i, 0.0);
    std::cout << "tmp_val = " << tmp_val << std::endl;
    while (i >= 0)
        input[i--] = tmp_val;
    i = (int) floor((start + duration) / step);
    std::cout << "second i = " << i << std::endl;
    tmp_val = f0_samples.value(i, 0.0);
    std::cout << "tmp_val = " << tmp_val << std::endl;
    while (i < f0_samples.size())
        input[i++] = tmp_val;
    */

    /* Instead of extending out the ends of the segment, find the first and
     * last non-zero values and extend using those instead
     * */
    for (i = 0; i < N && input.at(i) < E; ++i);
    // i is at the first non-zero value
    tmp_val = f0_samples.value(i, 0.0);
    while (i >= 0) {
        input[i--] = tmp_val;
    }
    // do the same for the end
    for (i = f0_samples.size() - 1; i >= 0 && input.at(i) < E; --i);
    tmp_val = f0_samples.value(i, 0.0);
    while (i < f0_samples.size()) {
        input[i++] = tmp_val;
    }

    std::cout << "tmp_val adjusted: ";
    for (int j = 0; j < N; j++) {
        std::cout << input[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "Skip over zeros" << std::endl;
    for (i=0; i < N && input.at(i) < E; ++i);
    std::cout << "i = " << i << std::endl;

    if (i < N) {
        tmp_val = input.at(i);
        std::cout << "pushing back i instances of tmp_val = " << tmp_val << std::endl;
        for (int k=0; k < i; ++k)
            output.push_back(tmp_val);
    }
    else {
        std::cout << "all zeros" << std::endl;
        for (int k=0; k < N; ++k)
            output.push_back(0.0);
        return;
    }


    while (i < N) {
        if (input.at(i) < E) {
            int x1 = i - 1;
            int x2 = i + 1;
            while (x2 < N && input.at(x2) < E) ++x2;
            std::cout << "Interpolating between " << x1 << " and " << x2 << std::endl;
            if (x2 < N) {
                float v1 = input.at(x1);
                float v2 = input.at(x2);
                float w = x2 - x1;
                while (i < x2) {
                    float v = v1 + (v2 - v1) / w * (i++ - x1);
                    output.push_back(v);
                }
            }
            else {
                tmp_val = input.at(x1);
                while (i++ < N) {
                    output.push_back(tmp_val);
                }
            }
        }
        else {
            output.push_back(input.at(i++));
        }
    }

    std::cout << "Output size = " << output.size() << std::endl;
    std::cout << "Output values: ";
    for (int j = 0; j < output.size(); j++) {
        std::cout << output.at(j) << " ";
    }
    std::cout << std::endl;

}

static void filter(int order, float *b, float *a,
                   QVector<float> &input, QVector<float> &output)
{
    // order: order of the filter
    // b: moving average coefficients (MA, applied to input)
    // a: autoregressive coefficients (AR, applied to history)
    // input: input vector
    // output: output vector

    output.clear();
    for (int i=0; i < input.size(); ++i) {
        float y = 0.0;
        for (int k=0; k <= order; ++k)
            y += b[k] * input.value(i-k, 0.0);
        for (int k=1; k <= order; ++k) // a[0] is ignored
            y -= a[k] * output.value(i-k, 0.0);
        output.push_back(y);
    }
}

static void smooth(QVector<float> &f0_samples,
                   QVector<float> &output,
                   float offset,
                   float duration,
                   float step)
{
    // Coefficients returned by butter(3, 0.1) from R/SciPy/MATLAB
    // B is moving average coefficients (MA)
    // A is autoregressive coefficients (AR)
    //
    // The f0 samples have been padded in the front and at the end. offset and
    // duration indicates where the main samples are.
    
    std::cout << "smooth aardvark" << std::endl;

    static float B[4] = {0.002898195, 0.008694584, 0.008694584, 0.002898195};
    static float A[4] = {1.0000000, -2.3740947, 1.9293557, -0.5320754};

    QVector<float> input;
    QVector<float> l;  // for debugging; stores interpolated samples

    int N = f0_samples.size();

    linear_interpolation(f0_samples, l, offset, duration, step);
    filter(3, B, A, l, output);
    for (int i=0; i < N; ++i)
        input.push_back(output.at(N-i-1));
    filter(3, B, A, input, output);
    for (int i=0; i < N; ++i)
        input[i] = output.at(N-i-1);
    for (int i=0; i < N; ++i) {
        output[i] = input.at(i);
    }

    std::cout << "Smoothing output size = " << output.size() << std::endl;
    std::cout << "Smoothing output values: ";
    for (int j = 0; j < output.size(); j++) {
        std::cout << output.at(j) << " ";
    }
    std::cout << std::endl;



}

bool get_f0_samples(
        char *audio,
        double beg,
        double end,
        float *result,
        int n_samples,
        get_f0_session *session
        )
{
    if (audio == 0)
        return false;

    if (n_samples < 2)
        return false;

    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(audio, SFM_READ, &sfinfo);
    if (sndfile == 0)
        return false;
    sf_close(sndfile);

    get_f0_session *f0_session = session;
    if (f0_session == 0) {
        f0_session = init_get_f0();
        f0_session->par->min_f0 = 60.0;
        f0_session->par->max_f0 = 650.0;
        f0_session->par->wind_dur = 0.01;
        f0_session->par->frame_step = 0.005;
    }

    double fend = ((double) sfinfo.frames) / sfinfo.samplerate;
    double org_beg = beg;
    double org_end = end;
    beg -= 0.5;
    end += 0.5;
    if (beg < 0.0)
        beg = 0.0;
    if (end > fend)
        end = fend;

    QVector<float> v;
    QVector<float> f0_samples;
    QVector<float> smoothed;
    get_f0(audio, f0_session, beg, end, callback, &v);
    std::cout << "v: " << std::endl;
    for (int z = 0; z < v.size(); z++) {
        std::cout << v.at(z) << " ";
    }
    std::cout << std::endl;
    for (int i=0; i<v.size(); i+=2)
        f0_samples.push_back(v.at(i));
    smooth(f0_samples,
           smoothed,
           org_beg - beg,
           org_end - org_beg,
           f0_session->par->frame_step);
    for (int i=0; i<smoothed.size(); ++i)
        v[i*2] = smoothed.at(i);

    double t0 = org_beg - beg;
    double step = (org_end - org_beg) / (n_samples - 1);
    for (int i=0; i < n_samples; ++i) {
        double t = t0 + step * i;
        double x = t / f0_session->par->frame_step;
        double x1 = floor(x);
        double x2 = ceil(x);
        double x0 = x1 - 1.0;
        double x3 = x2 + 1.0;
        int i0 = int(x0) * 2;
        int i1 = int(x1) * 2;
        int i2 = int(x2) * 2;
        int i3 = int(x3) * 2;

        float vp0, vp1, vp2, vp3;
        if (v.size() > i3+1) {
            vp0 = i0 >= 0 ? v.at(i0+1) : 0.0;
            vp1 = v.at(i1+1);
            vp2 = v.at(i2+1);
            vp3 = i3 < 60 ? v.at(i3+1) : 0.0;
        }
        else {
            vp0 = 0.0;
            vp1 = 0.0;
            vp2 = 0.0;
            vp3 = 0.0;
        }
        double y;

        /*
        if (vp1 < 0.5) {
            // x1 unvoiced
            if (vp2 < 0.5) {
                // x1, x2 unvoiced
                y = -1.0;
            }
            else {
                // x1 unvoiced, x2 voiced
                if (x - x1 < 0.5) {
                    // x is closer to unvoiced x1
                    y = -1.0;
                }
                else {
                    // x is closer to voiced x2
                    if (vp3 < 0.5) {
                        // x3 is unvoiced or ob
                        y = v.at(i2);
                    }
                    else {
                        // x3 is voiced
                        // extrapolate with x2 and x3
                        int y2 = v.at(i2);
                        int y3 = v.at(i3);
                        y = (x3*y2 - x2*y3 + (y3-y2)*x) / (x3 - x2);
                    }
                }
            }
        }
        else {
            // x1 is voiced
            if (vp2 < 0.5) {
                // x1 voiced, x2 unvoiced
                if (x2 - x < 0.5) {
                    // x is closer to unvoiced x2
                    y = -1.0;
                }
                else {
                    // x is closer to voiced x1
                    if (vp0 < 0.5) {
                        // x0 is unvoiced or ob
                        y = v.at(i1);
                    }
                    else {
                        // x0 is voiced
                        // extraploate with x0 and x1
                        int y0 = v.at(i0);
                        int y1 = v.at(i1);
                        y = (x1*y0 - x0*y1 + (y1-y0)*x) / (x1 - x0);
                    }
                }
            }
            else {
                // interpolate with x1 and x2
                double y1 = v.at(i1);
                double y2 = v.at(i2);
                y = (y2 - y1) / (x2 - x1) * (x - x1) + y1;
            }
        }
        */
        
        // interpolate with x1 and x2 (always, regardless of voicing)
        double y1 = v.at(i1);
        double y2 = v.at(i2);
        y = (y2 - y1) / (x2 - x1) * (x - x1) + y1;

        result[i] = (float) y;
    }

    if (session == 0)
        close_get_f0(f0_session);

    return true;
}
