clear all;
close all;

[mfccs_duck] = getMFCC('Duck.wav');
[mfccs_bluejay] = getMFCC('Bluejay.mp3');
[mfccs_dove] = getMFCC('Dove.mp3');

save('mfccs_duck.mat','mfccs_duck');
save('mfccs_bluejay.mat','mfccs_bluejay');
save('mfccs_dove.mat','mfccs_dove');

labels_duck = repmat({'Duck'},size(mfccs_duck, 1),1);
labels_bluejay = repmat({'Bluejay'},size(mfccs_bluejay, 1),1);
labels_dove = repmat({'Dove'},size(mfccs_dove, 1),1);

labels = [labels_duck; labels_bluejay; labels_dove];
birds = [mfccs_duck; mfccs_bluejay; mfccs_dove];
t = templateSVM('SaveSupportVectors', 'on');
Md1 = fitcecoc(birds, labels, 'Learners', t);

save('Md1.mat','Md1');