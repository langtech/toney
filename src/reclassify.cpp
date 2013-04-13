#include "annotation.h"
#include "toney_utils.h"
#include "RInside.h"
#include "Rcpp.h"

#include "rinstance.h"

/*Q Structure to hold info for building the matrix */
struct AnnDataRow {
    QString label;
    const float *f0;
};
typedef struct AnnDataRow AnnDataRow;

/*
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
*/

bool single_value(Rcpp::NumericVector v) {
    bool found_one = false;
    for (int i = 0; i < v.size(); i++) {
        if (v[i] > 0) {
            if (found_one) {
                return false; // more than one >0
            }
            else {
                found_one = true;
            }
        }
    }
    if (found_one == false) { // no >0s
        return false;
    }
    return true;
}

static bool passed_threshold(float threshold, Rcpp::NumericVector res) {
    // Score how close res is to the rounded result and return whether it passed the threshold
    float score = 0.0;
    int rounded;
    for (int t_i = 0; t_i < res.size(); ++t_i) {
        rounded = floor(res[t_i]+0.5);
        if (rounded > 1) {
            rounded = 1;
        }
        else if (rounded < 0) {
            rounded = 0;
        } // Binary values - make them 0 or 1. Max squared difference will be 1
        score += (rounded - res[t_i]) * (rounded - res[t_i]); // sum of squared differences
    }
    score = score / res.size(); // normalize score by number of components
    if (score < threshold) {
        return true;
    }
    return false;
}

// Param s: A hash of Annotations. The values are empty string. The reclassify
//   function should update the values to an appropriate cluster label.
// Param pos: A position in the values vector.
void reclassify(QHash<const Annotation,QString> &s, int pos)
{

    /* R session is already open and named R, from rinstance.h */
    SEXP ans;

    //const float threshold = 0.5; // Threshold for scoring; want the score to be under this threshold

    QHash<QString,float> cluster_mean;
    QHash<QString,int> cluster_num_samples;
    QHash<const Annotation,float> word_mean;

    /*Q Goal:
     * - Iterate over all the annotations / yup
     * - For each annotation, if it's unclustered, ignore it
     * - Else if it's in a cluster, grab its cluster number and all its data
     * -- Put them into a matrix /yup
     *
     * - Use the matrix to build a model
     * - Iterate over the annotations again and use the model to predict
     * -- Label each one with the prediction if it's within a threshold of a real number
     * --- Just calculate the distance from the closest single thing
     */

/* Original code
    QHash<const Annotation,QString>::iterator i = s.begin();

    for (; i != s.end(); ++i) {
        const Annotation ann = i.key();

        // See annotation.h for methods of Annotation class.
        // ann.getAudioPath()    -- path to the source audio file
        // ann.getTargetLabel()  -- word label
        // ann.getTargetStart()  -- start offset for the word
        // ann.getTargetEnd()    -- end offset for the word
        // ann.getValue(int pos) -- cluster label

        QString cluster = ann.getValue(pos);
        float v = sum_f0_samples(ann);
        if (cluster != "") {
            cluster_mean[cluster] += v;
            cluster_num_samples[cluster] += Annotation::NUM_F0_SAMPLES;
        }
        word_mean[ann] = v / Annotation::NUM_F0_SAMPLES;
    }
*/


    /*Q Build a matrix of the clustered data */
    QHash<const Annotation,QString>::iterator i = s.begin();
    QList<AnnDataRow> data_rows;
    QSet<QString> c_labels; // for storing the cluster labels

    for (; i != s.end(); ++i) {
        const Annotation ann = i.key();

        // See annotation.h for methods of Annotation class.
        // ann.getAudioPath()    -- path to the source audio file
        // ann.getTargetLabel()  -- word label
        // ann.getTargetStart()  -- start offset for the word
        // ann.getTargetEnd()    -- end offset for the word
        // ann.getValue(int pos) -- cluster label

        QString cluster = ann.getValue(pos);
        if (cluster != "") { /* ignore unclustered values */
            AnnDataRow new_row = {cluster, ann.getF0()}; // TODO This isn't very nice
            data_rows.push_back(new_row);
            c_labels.insert(cluster);
        }
    }

    /*Q
    // debug: print out the data rows
    QList<AnnDataRow>::iterator d_i = data_rows.begin();
    for (; d_i != data_rows.end(); ++d_i) {
        std::cout << (*d_i).label.toStdString() << " " << (*d_i).f0 << std::endl;
    }
    */

    /*Q Build a hash mapping labels to numbers */
    QHash<QString, int> label_nos;
    int label_i = 0;
    QSet<QString>::iterator label_iter = c_labels.begin();
    for (; label_iter != c_labels.end(); ++label_iter) {
        label_nos.insert(*label_iter, label_i);
        label_i++;
    }

    /*Q Convert data_rows QList into a NumericMatrix suitable to put into R */
    /* Because of the way matrix filling works, need to make the transpose */
    int k_rows = data_rows.size();
    int k_cols = c_labels.size() + Annotation::NUM_F0_SAMPLES;
    Rcpp::NumericMatrix K(k_cols, k_rows); // transpose

    // int k_size = k_rows * k_cols;
    int kk = 0;

    QList<AnnDataRow>::iterator d_i = data_rows.begin();
    for (; d_i != data_rows.end(); ++d_i) {
        // Put each data row into a column of the transposed matrix
        for (int c_i = 0; c_i < c_labels.size(); ++c_i) {
            // Figure out which column to fill for cluster label
            if (label_nos.value((*d_i).label) == c_i) {
                K[kk] = 1;
            }
            else {
                K[kk] = 0;
            }
            kk++;
        }
        for (int f0_i = 0; f0_i < Annotation::NUM_F0_SAMPLES; ++f0_i) {
            K[kk] = (*d_i).f0[f0_i];
            kk++;
        }
    }

    // convert to an R data frame
    (*RR)["f0data_t"] = K;
    (*RR).parseEvalQ("f0data = t(f0data_t);");
    // TODO strip zero rows

    std::string rcommand;


    // ugly to_string hack
    std::ostringstream c_labels_size;
    c_labels_size << c_labels.size();

    std::ostringstream c_labels_size_plus_1;
    c_labels_size_plus_1 << (c_labels.size()+1);

    std::ostringstream k_cols_string;
    k_cols_string << k_cols;

    rcommand = std::string("f0df = data.frame(tones=I(as.matrix(f0data[,1:") +
               c_labels_size.str() + "])), obs=I(as.matrix(f0data[," +
               c_labels_size_plus_1.str() + ":" + k_cols_string.str() +
               "])));";
    // print for debugging std::cout << rcommand << std::endl;
    (*RR).parseEvalQ(rcommand);

    (*RR).parseEvalQ("print(f0df);");

    // train a model
    rcommand = "library(\"pls\");";
    (*RR).parseEvalQ(rcommand);
    
    // CV uses 10 by default, so breaks if there are fewer than 10 observations;
    // change it to LOO in that case
    if (k_rows >= 10) {
        rcommand = "model=plsr(tones~obs, data=f0df, validation=\"CV\");";
    }
    else {
        rcommand = "model=plsr(tones~obs, data=f0df, validation=\"LOO\");";
    }
    (*RR).parseEvalQ(rcommand);
    (*RR).parseEvalQ("print(\"built model\");");


    // iterate through annotations and assign a cluster label to them */

    for (i = s.begin(); i != s.end(); ++i) {
        // take a single annotation, put it through the model
        const Annotation ann = i.key();
        const float *r_f0 = ann.getF0();
        Rcpp::NumericMatrix A(1, Annotation::NUM_F0_SAMPLES);
        for (int r_i = 0; r_i < Annotation::NUM_F0_SAMPLES; ++r_i) {
            A[r_i] = r_f0[r_i];
        }
        (*RR)["newrow"] = A;
        (*RR).parseEvalQ("print(newrow);");
        rcommand = "new = predict(model, ncomp=model$ncomp, newdata=newrow);";
        (*RR).parseEvalQ(rcommand);
        (*RR).parseEvalQ("print(new);");

        // get the result back from R and parse it in C++
        ans = (*RR).parseEval("new");
        Rcpp::NumericVector n(ans);
        std::cout << n[0] << " " << n[1] << std::endl;
        int last_one = -1;
        for (int t_i = 0; t_i < n.size(); ++t_i) {
            // TODO: judge a score/threshold
            n[t_i] = floor(n[t_i]+0.5);
            if (n[t_i] > 0) {
                last_one = t_i;
            }
        }
        // if there's more than one predicted value, leave it alone
        // otherwise, assign to the single predicted value
        if (single_value(n)) {
//            QString new_label = label_nos.key(last_one);
            std::cout << "new label: " << label_nos.key(last_one).toStdString() << std::endl;
            i.value() = label_nos.key(last_one);
        }

        /*
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
        */
    }

}
