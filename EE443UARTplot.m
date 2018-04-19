close all; clc; close all;
delete(instrfindall);
s = serial('COM7', 'BaudRate',115200);
set(s,'InputBufferSize',1024);
fopen(s);
buffersize = 257;
fs = 8000;
t = (0:buffersize-2)/fs;
f=-fs/2:2*fs/2/(buffersize-2):fs/2;

a = fread(s,buffersize*2,'uint8');
mag = max(abs(typecast(uint8(a),'int16')));
if(mag>3e4) 
    b = a(2:end-1);
else
    b = a(3:end);
end
c = typecast(uint8(b),'int16');
d = double(c-mean(c));

figure(1);
subplot(2,1,1); plot(t,d); xlabel('sec'); grid on;
subplot(2,1,2); plot(f,abs(fftshift(fft(d)))); grid on; xlabel('Hz');

fclose(s);
delete(s);