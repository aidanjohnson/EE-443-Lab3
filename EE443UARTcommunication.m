clear all; close all; clc;
%% Create the serial object
delete(instrfindall);      % delete previous connection
serialPort = 'COM5';       % set a serial port (UART)
baud = 115200;
serialObject = serial(serialPort, 'BaudRate',baud); % define an object
fopen(serialObject);       % open the UART connection
flushinput(serialObject);
%% Set up the figure window
w1 = 0;
time = 0;
sample = 0;
target1 = 0;
sampleTime = 1/baud;
pauseInterval = sampleTime * 1;

figureHandle = figure('NumberTitle','off',...
    'Name','EE443 USB data transmission Demo',...
    'Visible','off');

axesHandle = axes('Parent',figureHandle,'YGrid','on','XGrid','on');

hold on;
plotHandle = plot(axesHandle,time,w1,time,target1,'LineWidth',2);

% Create xlabel
xlabel('Timp [s]','FontWeight','bold','FontSize',14,'Color',[0 0 1]);

% Create ylabel
ylabel('Received value','FontWeight','bold','FontSize',14,'Color',[0 0 1]);

% Create title
title('EE443 USB data transmission Demo','FontSize',15,'Color',[0 0 1]);

%% Collect data
count = 1;
set(figureHandle,'Visible','on');
hf=figure('position',[0 0 eps eps],'menubar','none');

while 1
    % sending the data (count) to the board through UART (serial port),
    % repeat 4-times to ensure it send the data
    for ba=1:4, fwrite(serialObject, count); end
    
    % read the data from the board through UART
    samp = fread(serialObject,1,'uint16');
    sample = typecast(uint16(samp),'int16')
    time(count) = count * sampleTime;
    
    w1(count) = sample;
    
    if count  >= 256
        count = 0;
    end
    
    hold off;
    
    set(plotHandle, {'YData'}, {w1}, {'XData'}, {time});

    pause(pauseInterval);
    count = count + 1;
end

%% Clean up the serial object
fclose(serialObject);
delete(serialObject);
clear serialObject;