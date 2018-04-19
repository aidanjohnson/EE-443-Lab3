function [mfccs] = getMFCC(file)
    %   Returns MFC coefficients, plots time (audio) signal and MFCC
    %
    %   Example call:
    %
    %   close all; clear all; 
    %   getMFCC('Duck.wav');
    %   getMFCC('Bluejay.mp3');
    %   getMFCC('Dove.mp3');
    %
    %   See: https://www.mathworks.com/matlabcentral/fileexchange/32849-htk-mfcc-matlab

    % Load audio signal
    [y, Fs] = audioread(file);
    y = y(1:Fs*4,:);

    ch = size(y,2);
    dur = size(y,1);
    t = (0:dur-1)*(1/Fs);
    
    % Convert stereo to mono
    if (ch >= 2)
        y_mono = zeros(dur,1);
        for i = 1:ch
           y_mono = y_mono + y(:,i)/ch; 
        end
    end
    
    Tw = 21; % analysis frame duration (ms)
    Ts = 10; % analysis frame shift (ms) -- 10.48 for 380x13 4 s
    alpha = 0.97; % preemphasis coefficient
    R = [300, 3700]; % frequency range to consider
    M = 20; % number of filterbank channels 
    C = 13; % number of cepstral coefficients
    L = 22; % cepstral sine lifter parameter

    % Create hamming window
    w = @(N)(0.54-0.46*cos(2*pi*(0:N-1).'/(N-1)));

    % Plot speech samples given sampling rate
    figure;
    plot(t, y_mono(:));
    title('Monaural ' + string(file) + ' (' + string(t(end)) + ' s)');
    ylabel('amplitude');
    xlabel('time [s]');
    grid on;
    
    % Play speech samples
    sound(y_mono, Fs);

    % Feature extraction (feature vectors as columns)
    [MFCC, FBE, frames] = mfcc(y_mono, Fs, Tw, Ts, alpha, w, R, M, C, L);

    % Plot cepstrum over time
    figure('Position', [30 100 800 200], 'PaperPositionMode', 'auto', ... 
         'color', 'w', 'PaperOrientation', 'landscape', 'Visible', 'on' ); 

    imagesc((1:size(MFCC,2)), (0:C-1), MFCC); 
    axis('xy');
    xlabel('frame index'); 
    ylabel('cepstrum index');
    dim = ': ' + string(size(MFCC,2)) + 'x' + string(size(MFCC,1));
    name = string(file) + ' Mel-Frequency Cepstral Coefficients (MFCC)';
    title(name + dim);
    
    % MFC coefficients row vectors
    mfccs = MFCC';
end
%     Add below above, replacing plottin
%
%     % Generate plots
%     figure('Position', [30 30 800 600], 'PaperPositionMode', 'auto', ... 
%               'color', 'w', 'PaperOrientation', 'landscape', 'Visible', 'on' ); 
% 
%     subplot( 311 );
%     plot( time, speech, 'k' );
%     xlim( [ min(time_frames) max(time_frames) ] );
%     xlabel( 'Time (s)' ); 
%     ylabel( 'Amplitude' ); 
%     title( 'Speech waveform'); 
% 
%     subplot( 312 );
%     imagesc( time_frames, [1:M], logFBEs ); 
%     axis( 'xy' );
%     xlim( [ min(time_frames) max(time_frames) ] );
%     xlabel( 'Time (s)' ); 
%     ylabel( 'Channel index' ); 
%     title( 'Log (mel) filterbank energies'); 
% 
%     subplot( 313 );
%     imagesc( time_frames, [1:C], MFCCs(2:end,:) ); % HTK's TARGETKIND: MFCC
%     %imagesc( time_frames, [1:C+1], MFCCs );       % HTK's TARGETKIND: MFCC_0
%     axis( 'xy' );
%     xlim( [ min(time_frames) max(time_frames) ] );
%     xlabel( 'Time (s)' ); 
%     ylabel( 'Cepstrum index' );
%     title( 'Mel frequency cepstrum' );
% 
%     % Set color map to grayscale
%     colormap( 1-colormap('gray') ); 
% 
%     % Print figure to pdf and png files
%     print('-dpdf', sprintf('%s.pdf', mfilename)); 
%     print('-dpng', sprintf('%s.png', mfilename));