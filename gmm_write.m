close all; clear all;
delete(instrfindall);
s = serial('COM4', 'BaudRate',115200);
set(s,'InputBufferSize',4);
fopen(s);

load('GMModel.mat');

means = GMModel.mu;
weights = GMModel.PComponents;
vars = GMModel.Sigma;

GMMmeans = zeros(1,size(means,1)*size(means,2));
GMMvariances = zeros(1,size(vars,3)*size(vars,2));

for k = 1:size(means,1)
    GMMmeans(size(means,2)*(k-1)+1:size(means,2)*k) = means(k,:);
end
% sendUART(GMMmeans, s);

GMMweights = weights;
% sendUART(GMMweights, s);

for k = 1:size(vars,3)
    GMMvariances(size(vars,2)*(k-1)+1:size(vars,2)*k) = diag(vars(:,:,k));
end
% sendUART(GMMvariances, s);

fclose(s);
delete(s);

function [] = sendUART(x, s)
    flag = 0;
    y = typecast(x,'uint8'); % uint8 array 
    for ii=1:length(y)
        fwrite(s, y(ii));
%         pause(1);
        while(1)
            flag = fread(s,1,'uint8');
            if(flag==1) % waiting for ACK from LCDK
                flag = 0;
                break;
            end
        end
    end
end