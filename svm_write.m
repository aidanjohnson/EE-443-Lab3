close all;
clear all;
% delete(instrfindall);
% s = serial('COM7', 'BaudRate',115200);
% set(s,'InputBufferSize',1024);
% fopen(s);

load('Md1.mat');

class = 3;
% sendUART(class, s);


for n = 1:3
    rho = Md1.BinaryLearners{n}.Bias;
    bias(n) = rho;
%     sendUART(rho, s);
end

for n = 1:3
    nsv = size(Md1.BinaryLearners{n}.SupportVectors, 1);
    numberSV(n) = nsv;
%     sendUART(nsv, s);
end

k = 1;
for n = 1:3
    for i = 1:numberSV(n)
        sv_coef = Md1.BinaryLearners{n}.Alpha(i);
        coefficientsSV(k) = sv_coef;
        k = k + 1;
%         sendUART(sv_coef, s);
    end
end

k = 1;
for n = 1:3
    for i = 1:numberSV(n)
        for j = 1:13
            sv = Md1.BinaryLearners{n}.SupportVectors(i,j);
            supportVectors(k) = sv;
            k = k + 1;
%             sendUART(sv, s);
        end
    end
end

alpha1 = Md1.BinaryLearners{1}.Alpha;
y1 = Md1.BinaryLearners{1}.SupportVectorLabels;
SV1 = Md1.BinaryLearners{1}.SupportVectors;
w1 = (SV1'*(alpha1.*y1))';

alpha2 = Md1.BinaryLearners{2}.Alpha;
y2 = Md1.BinaryLearners{2}.SupportVectorLabels;
SV2 = Md1.BinaryLearners{2}.SupportVectors;
w2 = (SV2'*(alpha2.*y2))';

alpha3 = Md1.BinaryLearners{3}.Alpha;
y3 = Md1.BinaryLearners{3}.SupportVectorLabels;
SV3 = Md1.BinaryLearners{3}.SupportVectors;
w3 = (SV3'*(alpha3.*y3))';

w = [w1, w2, w3];
printTXT('w.txt',w);

% fclose(s);
% delete(s);

printTXT('SVMclass.txt',class);
printTXT('SVMrho.txt',bias);
printTXT('SVMn.txt',numberSV);
printTXT('SVMcoef.txt',coefficientsSV);
printTXT('SVM.txt',supportVectors);
params = [class, bias, numberSV, coefficientsSV, supportVectors];
printTXT('SVM_class_bias_n_coef_sv.txt', params);

function [] = printTXT(name, out)
    fileID = fopen(name,'w');
    fprintf(fileID,'{%f',out(1));
    if length(out) > 1
        fprintf(fileID,',%f',out(2:end));
    end
    fprintf(fileID,'}');
end

function [] = sendUART(x, s)
    flag = 0;
    ii = 1;
    y = typecast(x,'uint8') % uint8 array 
    while ii<=length(y)
        fwrite(s, y(ii));
        flag = fread(s,1,'uint8');
        if(flag==1)
            flag = 0;
            ii = ii + 1;
        end
    end
end