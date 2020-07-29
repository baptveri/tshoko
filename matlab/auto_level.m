function [out_m, ...
          rms_v, ...
          env_l_v, ...
          env_r_v, ...
          slow_gain_v, ...
          fast_gain_v] = auto_level(in_m, ...
                                    param, ...
                                    Fs)

% [out_m, env_v, slow_gain_v, fast_gain_v] = auto_level(in_m, param, FS)
%
% Compute auto level algorithm processing
%
% INPUT:
% * in_m: input signal (stereo)
% * param: parameters
% * Fs: Sampling frequency (Hz)
% OUTPUT:
% * out_m: output signal (stereo)
% * rms_v: rms signal (mono)
% * env_l_v: input left envelope signal (mono)
% * env_r_v: input right envelope signal (mono)
% * slow_gain_v: rms signal (mono)
% * fast_gain_v: compressor fast gain signal (mono)

%% Parameter conversion
z0_rms = exp(-3/(param.rms_smooth_s*Fs));
rms_target = db2mag(param.rms_target_db);
max_gain = db2mag(param.max_gain_db);
min_gain = db2mag(-param.max_gain_db);
noise_level = db2mag(param.noise_level_db);
z0_slow_gain = exp(-3/(param.slow_gain_smooth_s*Fs));

noise_time_s = 0.01; % s
noise_time_sample = round(noise_time_s*Fs); % s

comp_threshold = db2mag(param.comp_threshold_db);
z0_release = exp(-3/(param.comp_release_s*Fs));

%% Processing

% Allocate local buffers
N = length(in_m(:,1));

out_m = zeros(N,2);

rms_v = zeros(N,1);
env_l_v = zeros(N,1);
env_r_v = zeros(N,1);
slow_gain_v = zeros(N,1);
fast_gain_v = zeros(N,1);

% Compute RMS
rms2_l = 0;
rms2_r = 0;
epsilon = db2mag(-120);

for k = 1:N
    rms2_l = in_m(k,1)^2 - z0_rms * (in_m(k,1)^2 - rms2_l);
    rms2_r = in_m(k,2)^2 - z0_rms * (in_m(k,2)^2 - rms2_r);
    rms_v(k) = sqrt(rms2_l) + sqrt(rms2_r);
end

% Compute Slow gain
noise_count = 0;
slow_gain = 1;

for k = 1:N
    if rms_v(k) < noise_level
        if noise_count < noise_time_sample
            noise_count = noise_count+1;
        end
    else
        noise_count = 0;
    end
    
    if noise_count < noise_time_sample
        target_gain = rms_target/(rms_v(k)+epsilon);     
    else
        target_gain = 1;
    end
    
    target_gain = min(target_gain, max_gain);
    target_gain = max(target_gain, min_gain);
    
    slow_gain = target_gain - z0_slow_gain * (target_gain - slow_gain);

    slow_gain_v(k) = slow_gain;
end

% Compute Compressor fast gain
env_l = 0;
env_r = 0;

for k = 1:N
    peak_l = abs(in_m(k,1));
    peak_r = abs(in_m(k,2));
    
    if peak_l > env_l
        env_l = peak_l;
    else
        env_l = z0_release * env_l;
    end
    
    if peak_r > env_r
        env_r = peak_r;
    else
        env_r = z0_release * env_r;
    end
    
    env_l_v(k) = env_l;
    env_r_v(k) = env_r;
    
    env = max(env_l,env_r);
    if env*slow_gain_v(k) > comp_threshold
        fast_gain_v(k) = comp_threshold / (env*slow_gain_v(k));
    else
        fast_gain_v(k) =1;
    end
end

% Compute Output
out_m(:,1) = in_m(:,1) .* (slow_gain_v .* fast_gain_v);
out_m(:,2) = in_m(:,2) .* (slow_gain_v .* fast_gain_v);




