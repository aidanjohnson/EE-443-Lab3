close all; clc; close all;
delete(instrfindall);
s = serial('COM5', 'BaudRate',115200);
set(s,'InputBufferSize',4);
fopen(s);
x = 0.05; % floating type data
flag = 0;
y = typecast(x,'uint8'); % uint8 array 
for ii=1:length(y)
    fwrite(s, y(ii));
    while(1)
        flag = fread(s,1,'uint8');
        if(flag==1) % waiting for ACK from LCDK
            flag = 0;
            break;
        end
    end
end

fclose(s);
delete(s);