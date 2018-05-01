clc; clear all; close all;

Tw = 21;           % analysis frame duration (ms)
Ts = 10;           % analysis frame shift (ms)
alpha = 0.97;      % preemphasis coefficient
R = [ 300 3700 ];  % frequency range to consider
M = 48;            % number of filterbank channels 
C = 13;            % number of cepstral coefficients
L = 22;            % cepstral sine lifter parameter

% hamming window (see Eq. (5.2) on p.73 of [1])
hamming = @(N)(0.54-0.46*cos(2*pi*[0:N-1].'/(N-1)));

% Read speech samples, sampling rate and precision from file
[ speech, fs ] = audioread( 'Dove.mp3' );

% Feature extraction (feature vectors as columns)
[ MFCCs{1}, FBEs, frames ] = ...
              mfcc( speech(:,1), fs, Tw, Ts, alpha, hamming, R, M, C, L );

% Read speech samples, sampling rate and precision from file
[ speech, fs ] = audioread( 'Bluejay.mp3' );

% Feature extraction (feature vectors as columns)
[ MFCCs{2}, FBEs, frames ] = ...
              mfcc( speech(:,1), fs, Tw, Ts, alpha, hamming, R, M, C, L );

% Read speech samples, sampling rate and precision from file
[ speech, fs ] = audioread( 'Duck.wav' );

% Feature extraction (feature vectors as columns)
[ MFCCs{3}, FBEs, frames ] = ...
              mfcc( speech(:,1), fs, Tw, Ts, alpha, hamming, R, M, C, L );


X = [MFCCs{1} MFCCs{2} MFCCs{3}]';
Y = [ones(length(MFCCs{1}),1); 2*ones(length(MFCCs{2}),1); 3*ones(length(MFCCs{3}),1)];
k = 3;

t = templateSVM('SaveSupportVectors','on');
SVMmodel = fitcecoc(X,Y,'Learners',t);

sv_coef = [SVMmodel.BinaryLearners{1}.Alpha.*SVMmodel.BinaryLearners{1}.SupportVectorLabels;...
    SVMmodel.BinaryLearners{2}.Alpha.*SVMmodel.BinaryLearners{2}.SupportVectorLabels;...
    SVMmodel.BinaryLearners{3}.Alpha.*SVMmodel.BinaryLearners{3}.SupportVectorLabels]';
rho = [SVMmodel.BinaryLearners{1}.Bias SVMmodel.BinaryLearners{2}.Bias SVMmodel.BinaryLearners{3}.Bias];
nSV = [length(SVMmodel.BinaryLearners{1}.Alpha) length(SVMmodel.BinaryLearners{2}.Alpha) length(SVMmodel.BinaryLearners{3}.Alpha)];
SV = [SVMmodel.BinaryLearners{1}.SupportVectors' SVMmodel.BinaryLearners{2}.SupportVectors' SVMmodel.BinaryLearners{3}.SupportVectors'];
SV = reshape(SV, [1 429]);

x = [rho nSV sv_coef SV];

printTXT('SVM_class_bias_n_coef_sv.txt', x);

function [] = printTXT(name, out)
    fileID = fopen(name,'w');
    fprintf(fileID,'{%f',out(1));
    if length(out) > 1
        fprintf(fileID,',%f',out(2:end));
    end
    fprintf(fileID,'}');
end

% UART open
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
%     for jj=1:20, fwrite(s, y(ii)); end
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