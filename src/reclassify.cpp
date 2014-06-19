#include <fstream>

#include <RInside.h>
#include <Rcpp.h>

#include "annotation.h"
#include "toney_utils.h"
/*#include "RInside.h"
#include "Rcpp.h"*/

#include "rinstance.h"

#define NBEST 5

/*Q Structure to hold info for building the matrix */
struct AnnDataRow {
    QString label;
    const float *f0;
};
typedef struct AnnDataRow AnnDataRow;

// For storing both the score and new label info
struct ScoreInfo {
    QString new_label;
    float score;
};
typedef struct ScoreInfo ScoreInfo;

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

float prediction_score(Rcpp::NumericVector v) {
    // Takes a predicted vector (unrounded), v
    // Calculate how close it is to the rounded vector
    int rounded;
    float score = 0;
    for (int i = 0; i < v.size(); ++i) {
        rounded = floor(v[i]+0.5);
        // just make sure we have only 0 and 1
        if (rounded > 0) {
            rounded = 1;
        }
        else {
            rounded = 0;
        }
        score += (v[i] - rounded) * (v[i] - rounded);
    }
    return sqrt(score); // technically RMS should be 1/sqrt(v.size())*this, but v.size() always the same
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
    const float threshold = 0.25; // threshold for min difference

    QHash<QString,float> cluster_mean;
    QHash<QString,int> cluster_num_samples;
    QHash<const Annotation,float> word_mean;

    // just wrap everything in a try...catch in case R stuff fails
    try {

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
        
        // this is for dumping the f0 values to a file - TODO take it out when I don't need it
        std::ofstream f0file("f0file.f0");
        //f0file.open("f0file.f0");

        for (; i != s.end(); ++i) {
            const Annotation ann = i.key();

            // chuck the f0 values into a file 
            const float *f0values = ann.getF0();
            for (int j = 0; j < Annotation::NUM_F0_SAMPLES; j++) {
                f0file << f0values[j] << " ";
            }
            //f0file << ann.getTargetLabel().toStdString() << " " << ann.getValue(pos).toStdString() << std::endl;
            f0file << ann.getTargetLabel().toStdString() << " " << ann.getValue(pos).toStdString() << " " << ann.getValue(pos+1).toStdString() << " " << ann.getValue(pos+2).toStdString() << std::endl;


            // See annotation.h for methods of Annotation class.
            // ann.getAudioPath()    -- path to the source audio file
            // ann.getTargetLabel()  -- word label
            // ann.getTargetStart()  -- start offset for the word
            // ann.getTargetEnd()    -- end offset for the word
            // ann.getValue(int pos) -- cluster label

            QString cluster = ann.getValue(pos);
            if (cluster != "") { /* ignore unclustered values */
                AnnDataRow new_row = {cluster, ann.getF0()}; // TODO This isn't very nice
                
                //debuggerising
                std::cout << "current cluster = " << cluster.toStdString() << ", label = " << ann.getTargetLabel().toStdString() << std::endl;

                data_rows.push_back(new_row);
                c_labels.insert(cluster);
            }
        }

        f0file.close();

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

            // debuggerising
            std::cout << "cluster label: " << (*label_iter).toStdString() << ", label number: " << label_i << std::endl;

            label_i++;
        }

        /*Q Convert data_rows QList into a NumericMatrix suitable to put into R */
        /* Because of the way matrix filling works, need to make the transpose */
        int k_rows = data_rows.size();
        int k_cols = Annotation::NUM_F0_SAMPLES + 1;
        Rcpp::NumericMatrix K(k_cols, k_rows); // transpose

        // int k_size = k_rows * k_cols;
        int kk = 0;

        QList<AnnDataRow>::iterator d_i = data_rows.begin();
        for (; d_i != data_rows.end(); ++d_i) {
            // Put each data row into a column of the transposed matrix
            //K[kk] = label_nos.value((*d_i).label) + 1; // need the +1 because R goes from 1
            K[kk] = label_nos.value((*d_i).label);
            kk++;
            for (int f0_i = 0; f0_i < Annotation::NUM_F0_SAMPLES; ++f0_i) {
                K[kk] = (*d_i).f0[f0_i];
                kk++;
            }
        }

        // convert to an R data frame
        (*RR)["f0data_t"] = K;
        (*RR).parseEvalQ("f0data = t(f0data_t);");
        // TODO strip zero rows, or smooth them using splines

        std::string rcommand;


        // ugly to_string hack
        std::ostringstream c_labels_size;
        c_labels_size << c_labels.size();

        std::ostringstream c_labels_size_plus_1;
        c_labels_size_plus_1 << (c_labels.size()+1);

        std::ostringstream k_cols_string;
        k_cols_string << k_cols;

        /*
        rcommand = std::string("f0df = data.frame(tones=I(as.matrix(f0data[,1])), obs=I(as.matrix(f0data[,2:" +
                   k_cols_string.str() + "])));";
        // print for debugging std::cout << rcommand << std::endl;
        (*RR).parseEvalQ(rcommand);

        (*RR).parseEvalQ("print(f0df);");
        */

        // randomly sample an equal number from each of the tones
        /*
         *
         * # get the minimum number of tokens for any tone
         * min_all <- min(table(f0data[,1]))
         * # now sample min_all from each tone
         * # have to turn the f0data into a dataframe first
         * f0data_df <- data.frame(f0data)
         * # tone will be X1, the first variable
         * rndid <- with(f0data_df, ave(X1, X1, FUN=function(x) {sample.int(length(x))}))
         * # now restrict it
         * f0data_df <- f0data_df[rndid<=min_all,]
         * # from http://stackoverflow.com/questions/23831711/selecting-n-random-rows-across-all-levels-of-a-factor-within-a-dataframe?lq=1
         *
         */

        rcommand = std::string("min_all <- min(table(f0data[,1]));"
                               "f0data_df <- data.frame(f0data);"
                               "rndid <- with(f0data_df, ave(X1, X1, FUN=function(x) {sample.int(length(x))}));"
                               "f0data_df <- f0data_df[rndid<=min_all,];");
        (*RR).parseEvalQ(rcommand);

        // Take X and Y matrices from the data
    /*    rcommand = std::string("Y = as.vector(f0data[,1]);"
                               "X = as.matrix(f0data[,2:" + k_cols_string.str() + "]);");
                               */
        rcommand = std::string("Y = as.vector(f0data_df[,1]);"
                               "X = as.matrix(f0data_df[,2:" + k_cols_string.str() + "]);");
        (*RR).parseEvalQ(rcommand);


        // train a model
        // load mixOmics library
        rcommand = "library(\"mixOmics\");";
        (*RR).parseEvalQ(rcommand);
        
        rcommand = "model.pls = splsda(X, Y, ncomp=3);"; // TODO: make the ncomp settable
        (*RR).parseEvalQ(rcommand);
        (*RR).parseEvalQ("print(\"built model\");");


        // iterate through annotations and assign a cluster label to them */
        
        // to take care of the nbest
        QList<float> scores;
        QHash<const Annotation,ScoreInfo> new_labels;

        for (i = s.begin(); i != s.end(); ++i) {
            // take a single annotation, put it through the model
            const Annotation ann = i.key();
            const float *r_f0 = ann.getF0();
            Rcpp::NumericMatrix A(1, Annotation::NUM_F0_SAMPLES);
            for (int r_i = 0; r_i < Annotation::NUM_F0_SAMPLES; ++r_i) {
                A[r_i] = r_f0[r_i];
            }
            (*RR)["newrow"] = A;
            //(*RR).parseEvalQ("print(newrow);");
            rcommand = "new.result = predict(model.pls, newrow);";
            (*RR).parseEvalQ(rcommand);
            //(*RR).parseEvalQ("print(new.result);");
         
            //debuggerising: print the actual values from prediction
            (*RR).parseEvalQ("print(new.result$predict[1,,3]);");

            // get the results wanted back from R and parse it in C++
            rcommand = "class = new.result$class$max.dist[,3];"
                       "predicted_value = new.result$predict[1,class,3];"
                       "other_values = new.result$predict[1,-class,3];"
                       "min_difference = min(abs(predicted_value - other_values));";
            (*RR).parseEvalQ(rcommand);
            Rcpp::IntegerVector new_class_v = (*RR).parseEval("class");
            Rcpp::NumericVector min_diff_v = (*RR).parseEval("min_difference");
            int new_class = new_class_v[0];
            float min_diff = min_diff_v[0];
            // debuggerising: print out the current label if there is one
            if (ann.getValue(pos) != "") {
                std::cout << "current label is " << ann.getValue(pos).toStdString() << std::endl;
            }
            else {
                std::cout << "no current label" << std::endl;
            }
            // debuggerising: print out the predicted label
            std::cout << "predicted label number is " << new_class << ", label associated is " << label_nos.key(new_class-1).toStdString() << std::endl;
            // if the min difference is greater than the threshold, reassign label
            if (min_diff > threshold) {
                QString new_label = label_nos.key(new_class-1); // -1 to allow for different indexing in C++ and R
                i.value() = new_label;
            }
            /*
            ans = (*RR).parseEval("new");
            Rcpp::NumericVector n(ans);
            std::cout << n[0] << " " << n[1] << std::endl;
            float score =  prediction_score(n);
            scores << score;
            int last_one = -1;
            for (int t_i = 0; t_i < n.size(); ++t_i) {
                // TODO: judge a score/threshold
                // Round to the closest of 0 and 1
                n[t_i] = floor(n[t_i]+0.5);
                if (n[t_i] > 0) {
                    n[t_i] = 1; // make sure it's 1 and not 2 or 3 etc...
                    last_one = t_i;
                }
                else {
                    n[t_i] = 0; // just in case it's negative
                }
            }
            // if there's more than one predicted value, leave it alone
            // otherwise, assign to the single predicted value
            if (single_value(n)) {
    //            QString new_label = label_nos.key(last_one);
    //            std::cout << "new label: " << label_nos.key(last_one).toStdString() << std::endl;
    //            i.value() = label_nos.key(last_one);
                ScoreInfo new_score;
                new_score.new_label = label_nos.key(last_one);
                new_score.score = score;
                //new_labels[ann] = {label_nos.key(last_one), score};
                new_labels.insert(ann, new_score);
            }


            // Sort scores, find the 5th best score and relabel only the ones with scores less than or equal to that. (Or make sure we don't have less than 5.
            qSort(scores.begin(), scores.end());
            float threshold;
            if (scores.size() > NBEST) {
                threshold = scores[NBEST-1];
            }
            else {
                threshold = scores[scores.size()-1];
            }

            QHash<const Annotation, ScoreInfo>::iterator score_iter = new_labels.begin();
            for (; score_iter != new_labels.end(); ++score_iter) {
                const Annotation this_ann = score_iter.key();
                ScoreInfo this_score = score_iter.value();
                if (this_score.score <= threshold) {
                    s[this_ann] = this_score.new_label;
                }

            }
            */

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
    catch(...) {
        // throw up an error
        QMessageBox pls_error_box;
        pls_error_box.critical(0, "Reclassification error", "Unable to predict categorisations (possibly insufficient information)");
        pls_error_box.setFixedSize(500,200);
    }

}
