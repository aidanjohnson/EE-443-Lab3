function [] = getMFCC(y, Fs)
       
    y_mono = zeros(size(y,1),1);
    for i = 1:size(y,2)
       y_mono = y_mono + y(:,i)/size(y,2); 
    end
    
    Tw = 21;           % analysis frame duration (ms)
    Ts = 11;           % analysis frame shift/overlap (ms)
    alpha = 0.97;      % preemphasis coefficient
    R = [300, 3700];   % frequency range to consider
    M = 20;            % number of filterbank channels 
    C = 13;            % number of cepstral coefficients
    L = 22;            % cepstral sine lifter parameter

    % Creates hamming window
    w = @(N)(0.54-0.46*cos(2*pi*(0:N-1).'/(N-1)));

    % Plot speech samples given sampling rate
    figure;
    t = (0:length(y)-1)*(1/Fs);
    plot(t, y_mono(:));
    title('Monaural Speech');
    ylabel('amplitude');
    xlabel('time [s]');
    grid on;
    
    % Play speech samples
    sound(y_mono, Fs);

    % Feature extraction (feature vectors as columns)
    [MFCC, FBE, frames] = mfcc(y_mono(:), Fs, Tw, Ts, alpha, w, R, M, C, L);

    % Plot cepstrum over time
    figure('Position', [30 100 800 200], 'PaperPositionMode', 'auto', ... 
         'color', 'w', 'PaperOrientation', 'landscape', 'Visible', 'on' ); 

    imagesc((1:size(MFCC,2)), (0:C-1), MFCC); 
    axis('xy');
    xlabel('frame index'); 
    ylabel('cepstrum index');
    title('Mel-Frequency Cepstral Coefficients (MFCC)');
end