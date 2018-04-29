clear all;
close all;

[mfccs_duck] = getMFCC('Duck.wav',1,0);
[mfccs_bluejay] = getMFCC('Bluejay.mp3',1,0);
[mfccs_dove] = getMFCC('Dove.mp3',1,0);

save('mfccs_duck.mat','mfccs_duck');
save('mfccs_bluejay.mat','mfccs_bluejay');
save('mfccs_dove.mat','mfccs_dove');

birds = [mfccs_duck; mfccs_bluejay; mfccs_dove];
GMModel = fitgmdist(birds, 3);

save('GMModel.mat','GMModel');