close all; close all;
delete(instrfindall);
s = serial('COM7', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
fopen(s);

load('SVMModel.mat');
 
fwrite(s, SVMModel); 

fclose(s);
delete(s);