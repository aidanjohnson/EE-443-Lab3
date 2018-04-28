close all;
clear all;
% delete(instrfindall);
% s = serial('COM7', 'BaudRate',115200);
% set(s,'InputBufferSize',1024);
% fopen(s);

load('SVMModel.mat');
load('Md1.mat');

class = 3;
% sendUART(class, s);


for n = 1:3
    nsv = size(Md1.BinaryLearners{n}.SupportVectors, 1);
    numberSV(n) = nsv;
%     sendUART(nsv, s);
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

k = 1;
for n = 1:3
    for i = 1:numberSV(n)
        sv_coef = Md1.BinaryLearners{n}.Alpha(i);
        coefficientsSV(k) = sv_coef;
        k = k + 1;
%         sendUART(sv_coef, s);
    end
end

for n = 1:3
    rho = Md1.BinaryLearners{n}.Bias;
    bias(n) = rho;
%     sendUART(rho, s);
end

% fclose(s);
% delete(s);

printTXT('SVMclass.txt',class);
printTXT('SVMn.txt',numberSV);
printTXT('SVM.txt',supportVectors);
printTXT('SVMcoef.txt',coefficientsSV);
printTXT('SVMrho.txt',bias);
params = [class, numberSV, supportVectors, coefficientsSV, bias];
printTXT('SVM_class_n_sv_coef_bias', params);

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