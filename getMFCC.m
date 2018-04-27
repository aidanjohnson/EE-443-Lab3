function [mfccs] = getMFCC(file, mkFig)
    %   Returns MFC coefficients, and plots time (audio) signal and MFCC.
    %   Plotting is optional (i.e., mkFig can be omitted).
    %   If mkFig is TRUE, plots audio and MFCCs, else no plots by default.
    %
    %   Example call:
    %
    %   [mfccs_duck] = getMFCC('Duck.wav', 1);
    %   [mfccs_bluejay] = getMFCC('Bluejay.mp3', 1);
    %   [mfccs_dove] = getMFCC('Dove.mp3', 1);
    %
    %   Requires:
    %   www.mathworks.com/matlabcentral/fileexchange/32849-htk-mfcc-matlab
    
    % Optional plot parameter set to default
    if ~exist('mkFig','var')
        mkFig = 0;
    end
    
    % Load audio signal
    [y, Fs] = audioread(file);
    % y = y(1:Fs*4,:); % only 4 s of sample
    
    ch = size(y,2); % number of audio channels
    dur = size(y,1); % sample length of audio input
    t = (0:dur-1)*(1/Fs); % time vector for file sampling frequency in Hz
    
    % Convert stereo to mono
    if (ch == 1)
        y_mono = y;
    else
        y_mono = zeros(dur,1);
        for i = 1:ch
           y_mono = y_mono + y(:,i)/ch; 
        end
    end
    
    % normalize audio
    y_max = max(y_mono);
    y_mono = y_mono./y_max;
    
    Tw = 21; % analysis frame duration (ms)
    Ts = 10; % analysis frame shift (ms) -- 10.48 for 380x13 4 s
    alpha = 0.97; % preemphasis coefficient
    R = [100, 10000]; % frequency range to consider (Hz)
    M = 20; % number of filterbank channels 
    C = 13; % number of cepstral coefficients
    L = 22; % cepstral sine lifter parameter

    % Create hamming window
    w = @(N)(0.54-0.46*cos(2*pi*(0:N-1).'/(N-1)));
    
    % Play speech samples
    % sound(y_mono, Fs);

    % Feature extraction (feature vectors as columns)
    [MFCC, FBE, frames] = mfcc(y_mono, Fs, Tw, Ts, alpha, w, R, M, C, L);
    
    if mkFig == 1
        % Plot cepstrum over time
        figure;

        % Plot speech samples given sampling rate
        subplot(2,1,1);
        plot(t, y_mono(:));
        title('Monaural ' + string(file) + ' (' + string(t(end)) + ' s)');
        ylabel('amplitude');
        xlabel('time [s]');

        % Plot MFC coefficients
        subplot(2,1,2);
        imagesc((1:size(MFCC,2)), (0:C-1), MFCC); 
        axis('xy');
        xlabel('frame index'); 
        ylabel('cepstrum index');
        dim = ': ' + string(size(MFCC,2)) + 'x' + string(size(MFCC,1));
        name = string(file) + ' Mel-Frequency Cepstral Coefficients (MFCC)';
        title(name + dim);
    end
    
    % MFC coefficients row vectors
    mfccs = MFCC';
end
