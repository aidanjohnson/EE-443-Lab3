clear all; close all; clc;
delete(instrfindall);

s = serial('COM7', 'BaudRate', 115200);

fopen(s);
i = 0;

while(1)
    i = i + 1;
    fscanf(s)
end

fclose(s);