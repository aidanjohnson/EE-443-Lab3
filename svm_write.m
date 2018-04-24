close all; clear all;
delete(instrfindall);
s = serial('COM7', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
fopen(s);

load('SVMModel.mat');
load('Md1.mat');

class = 3;
fswrite(s, class);

for n = 1:3
    sv = Md1.BinaryLearners{n}.SupportVectors;
    fwrite(s, sv);
end

for n = 1:3
    nsv = length(SVMModel.BinaryLearners{n}.SupportVectors);
    fwrite(s, nsv);
end

for n = 1:3
    sv_coef = Md1.BinaryLearners{n}.Alpha;
    fwrite(s, sv_coef);
end

for n = 1:3
    rho = Md1.BinaryLearners{n}.Bias;
    fwrite(s, rho);
end
 
fclose(s);
delete(s);