# EE 443 Lab 3

## Problem 1: (Matlab) [MFCC](https://www.mathworks.com/matlabcentral/fileexchange/32849-htk-mfcc-matlab)
Status: In Progress
- getMFCC.m
- Frame sounds using A=21ms windows, B=10ms shift, and A-B=11ms of overlapping. Generate 13 MFCC coefficients for every frame. (4 seconds of sounds will generate 380 frames, that is the sample will have 380x13 MFCC coefficients matrix). Plot the 380x13 MFCC coefficients for all three Bird sounds in Matlab.

## Problem 2: (Matlab) Training [GMM](https://www.mathworks.com/help/stats/fitgmdist.html) using MFCC features
Status: In Progress
- gmm_traning.m
- Find the GMM parameters of the features found in Problem 1 to cluster the features.

GMM parameters: In Matlab, 'CovarianceType', 'diagonal' to generate k times D covariances, where k is the number of classes, and D is the number of features.

```MATLAB
GMModel = fitgmdist(X,k,'CovarianceType','diagonal');
```

|            | Matlab              | gmm.c       |
| ---------- | ------------------- | ----------- |
| Means      | GMModel.mu          | GMM.means   |
| Variances  | GMModel.Sigma       | GMM.covars  |
| Weights    | GMModel.PComponents | GMM.weights |


## Problem 3: (LCDK) Classification of Bird sound using GMM
Status: In Progress
- gmm_write.m
- lab3_problem3 directory (main.c & ISRs.c)--gmm.h array lengths for means, covars, P(k|x) changed to 39; not sure why JP hard coded it and changed it from the memory allocation performed by the original.
- Use GMM model trained in Problem 2 to classify the Bird sound received through Line input of LCDK.

## Problem 4: (Matlab) Training [SVM](https://www.mathworks.com/help/stats/fitcecoc.html) for Bird sound classification
Status: In Progress
- svm_training.m
- Train the SVM model using the features found in Problem 1.

SVM parameters: In Matlab, 'SaveSupportVectors', 'on' helps to save the support vectors in a SVM model.

```MATLAB
t = templateSVM('SaveSupportVectors','on')
Mdl = fitcecoc(X,Y,'Learners',t);
```

|                           | Matlab                                            | svm.cpp         |
| ------------------------- | ------------------------------------------------- | --------------- |
| Number of Classes         | 3                                                 | model->nr_class |
| Support Vectors           | Md1.BinaryLearners{1}.SupportVectors              | model->SV       |
|                           | Md1.BinaryLearners{2}.SupportVectors              |                 |
|                           | Md1.BinaryLearners{3}.SupportVectors              |                 |
| Number of Support Vectors | length(SVMmodel.BinaryLearners{1}.SupportVectors) | model->nSV      |
|                           | length(SVMmodel.BinaryLearners{2}.SupportVectors) |                 |
|                           | length(SVMmodel.BinaryLearners{3}.SupportVectors) |                 |
| Alpha                     | Md1.BinaryLearners{1}.Alpha                       | model->sv_coef  |
|                           | Md1.BinaryLearners{2}.Alpha                       |                 |
|                           | Md1.BinaryLearners{3}.Alpha                       |                 |
| Bias                      | Md1.BinaryLearners{1}.Bias                        | model->rho      |
|                           | Md1.BinaryLearners{2}.Bias                        |                 |
|                           | Md1.BinaryLearners{3}.Bias                        |                 |

## Problem 5: (LCDK) Classification of Bird sound using SVM
Status: Started
- svm_write.m
- lab3_problem5 directory (main.c & ISRs.c)
- SVM to classify the features. Use MFCC features to classify Bird sounds.
