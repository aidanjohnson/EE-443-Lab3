close all; clear all;
delete(instrfindall);
s = serial('COM7', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
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
for n = 1:length(GMMmeans)
    fwrite(s, GMMmeans(1,n));
end

GMMweights = weights;
for n = 1:length(GMMweights)
    fwrite(s, GMMweights(1,n));
end

for k = 1:size(vars,3)
    GMMvariances(size(vars,2)*(k-1)+1:size(vars,2)*k) = diag(vars(:,:,k));
end
for n = 1:length(GMMvariances)
    fwrite(s, GMMvariances(1,n));
end

fclose(s);
delete(s);