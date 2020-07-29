FORCE_IO_FILES = 1;

if FORCE_IO_FILES == 1
    clear all;
    close all;
    FORCE_IO_FILES = 1;
    input_gain = 0; % dB
end

FIG = 1;
WRITE_OUTPUT = 1;

%% Open audio input file
if FORCE_IO_FILES == 1
    % input_filename  = 'Dorageh - Changer cette vie - master2.wav';
    % input_filename  = '18 Here I Come.wav';
    %input_filename  = 'speech_dft_stereo.wav';
%     input_filename  = 'getlucky.mp3';
%     input_filename = 'playlist/33932142.mp3';
%     output_filename = 'out.wav';
    input_filename = '../audio/03-Tchaikovsky_The_Firebird_Suite_48k.wav';
    output_filename = '../audio/out.wav';
end

[in_m, Fs] = audioread(input_filename);

tmin_s = 0;
tmax_s = 0;

tmin_sample = max(round(tmin_s*Fs),1);

if tmax_s > 0
    tmax_sample = min( round(tmax_s*Fs), length(in_m(:,1)) );
else
    tmax_sample = length(in_m(:,1));
end

range_v = tmin_sample:tmax_sample;
in_m = in_m(range_v,:);

N = length(in_m(:,1));
time_v = (0:N-1)/Fs;

%% Algorithm Parameters
param.rms_smooth_s = 8; % s
param.rms_target_db = -16; % dBFS
param.max_gain_db = 12; % dB
param.noise_level_db = -50; % dBFS
param.slow_gain_smooth_s = 2; % s

param.comp_threshold_db = 0; % dBFS
param.comp_release_s = 0.1; % s

in_m = in_m*db2mag(input_gain);

%% Processing
[out_m, ...
 rms_v, ...
 env_l_v, ...
 env_r_v, ...
 slow_gain_v, ...
 fast_gain_v] = auto_level(in_m, param, Fs);

%% Display Trace
if FIG == 1
    figure;

    s1 = subplot(311);
    plot(time_v, in_m(:,1));
    hold on;
    plot(time_v, out_m(:,1));
    plot(time_v, env_l_v);

    legend('in','out','env');
    grid minor;
    ylim([-1 1]);

    ylabel('L');

    s2 = subplot(312);
    plot(time_v, in_m(:,2));
    hold on;
    plot(time_v, out_m(:,2));
    plot(time_v, env_r_v);

    legend('in','out','env');
    grid minor;
    ylim([-1 1]);

    ylabel('R');

    s3 = subplot(313);
    plot(time_v, db(rms_v));
    hold on;
    plot(time_v, db(slow_gain_v));
    plot(time_v, db(fast_gain_v));
    plot(time_v, db(rms_v.*slow_gain_v),'g');
    plot(time_v, param.rms_target_db*ones(N,1),'k--');
    plot(time_v, param.noise_level_db*ones(N,1),'c--');

    legend('rms','slow gain','fast gain','total gain');
    grid minor;

    ylim([-40 20]);

    xlabel('Time (s)')

    linkaxes([s1 s2 s3],'x');
end

%% Write output file
if WRITE_OUTPUT == 1
    max_left  = max(abs(out_m(:,1)));
    max_right = max(abs(out_m(:,2)));
    max_tot = max(max_left,max_right);
    
    if max_tot>=1
        if db(max_tot)>0.1
            test=0;
        end
        out_m = out_m/max_tot;
    end
    
    audiowrite(output_filename, out_m, Fs);
end
