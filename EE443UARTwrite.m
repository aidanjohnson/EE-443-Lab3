close all; clc; close all;
delete(instrfindall);
s = serial('COM7', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
fopen(s);
 
fwrite(s, 'a'); 

fclose(s);
delete(s);