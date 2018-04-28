clear all;
close all;

[mfccs_duck] = getMFCC('Duck.wav');
[mfccs_bluejay] = getMFCC('Bluejay.mp3');
[mfccs_dove] = getMFCC('Dove.mp3');

save('mfccs_duck.mat','mfccs_duck');
save('mfccs_bluejay.mat','mfccs_bluejay');
save('mfccs_dove.mat','mfccs_dove');

birds = [mfccs_duck; mfccs_bluejay; mfccs_dove];
GMModel = fitgmdist(birds, 3);

save('GMModel.mat','GMModel');