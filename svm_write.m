close all;
clear all;
delete(instrfindall);
s = serial('COM7', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
fopen(s);

load('SVMModel.mat');
load('Md1.mat');

class = 3;
sendUART(class, s);

nsv = [0,0,0];
for n = 1:3
    nsv(n) = size(Md1.BinaryLearners{n}.SupportVectors, 1);
    sendUART(nsv, s);
end

for n = 1:3
    for i = 1:nsv(n)
        for j = 1:13
            sv = Md1.BinaryLearners{n}.SupportVectors(i,j);
            sendUART(sv, s);
        end
    end
end

for n = 1:3
    for i = 1:nsv(n)
        sv_coef = Md1.BinaryLearners{n}.Alpha(i);
        sendUART(sv_coef, s);
    end
end

for n = 1:3
    rho = Md1.BinaryLearners{n}.Bias;
    sendUART(rho, s);
end

fclose(s);
delete(s);

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