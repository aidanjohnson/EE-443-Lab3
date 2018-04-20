clear all;
close all;

[mfccs_duck] = getMFCC('Duck.wav');
[mfccs_bluejay] = getMFCC('Bluejay.mp3');
[mfccs_dove] = getMFCC('Dove.mp3');

birds = [mfccs_duck; mfccs_bluejay; mfccs_dove];
GMModel = fitgmdist(birds, 3);