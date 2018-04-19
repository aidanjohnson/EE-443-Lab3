function [] = getMFCC(input, Fs)

    Tw = 21;           % analysis frame duration (ms)
    Ts = 11;           % analysis frame shift/overlap (ms)
    alpha = 0.97;      % preemphasis coefficient
    R = [300, 3700];   % frequency range to consider
    M = 20;            % number of filterbank channels 
    C = 13;            % number of cepstral coefficients
    L = 22;            % cepstral sine lifter parameter

    % hamming window
    hamming = @(N)(0.54-0.46*cos(2*pi*(0:N-1).'/(N-1)));

    % Read speech samples, sampling rate and precision from file
    [y, Fs] = audioread('Bluejay.mp3');

    % Play speech sample
    sound(y,Fs);

    % Feature extraction (feature vectors as columns)
    [MFCC, FBE, frames] = mfcc(y(:,1), Fs, Tw, Ts, alpha, hamming, R, M, C, L);

    % Plot cepstrum over time
    figure('Position', [30 100 800 200], 'PaperPositionMode', 'auto', ... 
         'color', 'w', 'PaperOrientation', 'landscape', 'Visible', 'on' ); 

    imagesc((1:size(MFCC,2)), (0:C-1), MFCC); 
    axis('xy');
    xlabel('Frame index'); 
    ylabel('Cepstrum index');
    title('Mel frequency cepstrum');
end