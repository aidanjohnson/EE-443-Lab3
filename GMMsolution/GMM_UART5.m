clc; clear all; close all;

% Read speech samples, sampling rate and precision from file
[ speech{1}, fs(1) ] = audioread( 'Birdsound\Dove.mp3' );
[ speech{2}, fs(2) ] = audioread( 'Birdsound\Bluejay.mp3' );
[ speech{3}, fs(3) ] = audioread( 'Birdsound\Duck.wav' );

for k=1:3
    % resampling
    [p,q] = rat(12000/fs(k),0.0001);
    speech{k} = resample(speech{k}(:,1),p,q);
    fs(k) = 12000;
    speech{k} = 1000*speech{k}/sum(abs(speech{k}))*length(speech{k});
    % Framing
    A = floor(21.4e-3*fs(k));
    B = floor(10e-3*fs(k));

    % Parameters
    binSize = A;
    NumFilters = 48;

    for fr=1:floor((length(speech{k})-A)/B)
        sp = speech{k}(((fr-1)*B+1):((fr-1)*B+A),1);
        %sp = sp.*hamming(binSize);
        spectralData = abs(fft(sp,binSize));
 
        % MFCC parameters
        for mf = 0:12
            MFCCs{k}(fr,mf+1) = GetCoefficient(spectralData, fs(k), NumFilters, binSize, mf);
        end
    end
end

X = [MFCCs{1}; MFCCs{2}; MFCCs{3}];
k = 3;

GMModel = fitgmdist(X, k, 'CovarianceType','diagonal');
%x = [GMModel.PComponents reshape(GMModel.mu',[1 39]) reshape(GMModel.Sigma,[1 39])];

% % UART open
% delete(instrfindall);
% s = serial('COM5', 'BaudRate',115200);
% set(s,'InputBufferSize',1024);
% fopen(s);
% 
% % sending floating point array
% flag = 0;
% ii = 1;
% y = typecast(x,'uint8') % uint8 array 
% 
% while ii<=length(y)
%     for jj=1:1, fwrite(s, y(ii)); end
%     flag = fread(s,1,'uint8');
%     if(flag==1)
%         flag = 0;
%         ii = ii + 1
%     end
% end
% 
% for ii=1:100, fwrite(s, 0); end % garbage data
% 
% fclose(s);
% delete(s);