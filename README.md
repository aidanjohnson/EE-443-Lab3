# EE 443 Lab3

## Problem 1: (Matlab) [MFCC](https://www.mathworks.com/matlabcentral/fileexchange/32849-htk-mfcc-matlab)
Status: In Progress
Complete:
- N/A
In Progress:
- Frame sounds using A=21ms windows, B=10ms shift, and A-B=11ms of overlapping. Generate 13 MFCC coefficients for every frame. (4 seconds of sounds will generate 380 frames, that is the sample will have 380x13 MFCC coefficients matrix). Plot the 380x13 MFCC coefficients for all three Bird sounds in Matlab.

## Problem 2: (Matlab) Training [GMM](https://www.mathworks.com/help/stats/fitgmdist.html) using MFCC features
Status: Not Started
- Gaussian Mixture Model (GMM) helps to cluster the features. Find the GMM parameters of the features found in Problem 1.

## Problem 3: (LCDK) Classification of Bird sound using GMM
Status: Not Started
- Use GMM model trained in Problem 2. to classify the Bird sound received through Line input of LCDK.

## Problem 4: (Matlab) Training [SVM](https://www.mathworks.com/help/stats/fitcecoc.html) for Bird sound classification
Status: Not Started
- Train the SVM model using the features found in Problem 1.

## Problem 5: (LCDK) Classification of Bird sound using SVM
Status: Not Started
- SVM helps to classify the features. Use MFCC features to classify Bird sounds.
