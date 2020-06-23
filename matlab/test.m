clear all;
close all;

% Parameters
input_filename = '../audio/03-Tchaikovsky_The_Firebird_Suite_48k.wav';
output_filename = '../audio/out.wav';

% Open audio files
[in_m,Fs]  = audioread(input_filename);
[out_m,Fs] = audioread(output_filename);

N = min([length(in_m(:,1)) length(out_m(:,1))]);

in_m  = in_m(1:N,:);
out_m = out_m(1:N,:);

time_v = (0:N-1)'/Fs;

% Trace
figure;

subplot(211);

plot(time_v,in_m(:,1));
hold on;
plot(time_v,out_m(:,1));
plot(time_v,out_m(:,1)-in_m(:,1));

ylabel('L');
grid on;

subplot(212);

plot(time_v,in_m(:,2));
hold on;
plot(time_v,out_m(:,2));
plot(time_v,out_m(:,2)-in_m(:,2));

ylabel('R');
xlabel('Time (s)');
grid on;