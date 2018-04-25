close all; clc; close all;

x = randn(1,39); % floating point parameters 

% UART open
delete(instrfindall);
s = serial('COM5', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
fopen(s);

% sending floating point array
flag = 0;
ii = 1;
y = typecast(x,'uint8') % uint8 array 

while ii<=length(y)
    for jj=1:20, fwrite(s, y(ii)); end
    flag = fread(s,1,'uint8');
    if(flag==1)
        flag = 0;
        ii = ii + 1;
    end
end

for ii=1:100, fwrite(s, 0); end % garbage data

fclose(s);
delete(s);