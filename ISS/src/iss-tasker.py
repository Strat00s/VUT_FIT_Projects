from scipy.io.wavfile import read
from scipy.io.wavfile import write
import numpy as np
from matplotlib import pyplot as plt
from scipy import signal
import math
import cmath

#Input File     : '../audio/maskon_tone1s.wav'
#Channels       : 1
#Sample Rate    : 16000
#Precision      : 16-bit
#Duration       : 00:00:01.00 = 16001 samples ~ 75.1453 CDDA sectors
#File Size      : 32.1k
#Bit Rate       : 256k
#Sample Encoding: 16-bit Signed Integer PCM

time = 1
sr = 16000
samples = 16001
scope_lenght = 0.02 #20ms
frames = 99

#time = samples
#20ms = 0.02s -> 1/0.02 = 50
#16031/50 = 320,62
#20ms = 320,62 (320) samples -> scope size (20ms) in samples
samples_per_scope = int(samples/(time/scope_lenght))

moft = read("../audio/maskoff_tone1s.wav")
mont = read("../audio/maskon_tone1s.wav")
off_array_original = np.array(moft[1], dtype=float)  #our signal array
on_array_original = np.array(mont[1], dtype=float)  #our signal array

#----(TASK 3)----
off_array_un = np.copy(off_array_original)
on_array_un = np.copy(on_array_original)
#ustredneni
off_array_un -= np.mean(off_array_un)
on_array_un -= np.mean(on_array_un)
#normalizace
off_array_un /= np.abs(off_array_un).max()
on_array_un /= np.abs(on_array_un).max()

off_array = np.zeros(shape=(frames, samples_per_scope))
on_array = np.zeros(shape=(frames, samples_per_scope))
half = samples_per_scope/2

#rozdeleni na ramce
for i in range(0, frames):
    off_array[i] = off_array_un[int(i*half) : int(samples_per_scope + i * half)]
    on_array[i]  =  on_array_un[int(i*half) : int(samples_per_scope + i * half)]

#plot mask off
x = np.arange(0, 0.02, 0.0000625)
plt.subplot(2, 1, 1)
plt.plot(x, off_array[0])
plt.ylim([-1, 1])
plt.title("Mask off")
plt.ylabel("y")
plt.xlabel("t[s]")

#plot mask on
plt.subplot(2, 1, 2)
plt.plot(x, on_array[0])
plt.ylim([-1, 1])
plt.title("Mask on")
plt.ylabel("y")
plt.xlabel("t[s]")

plt.show()  #show graphs

#----(TAsk 4)----
off_clipped = np.copy(off_array)
on_clipped = np.copy(on_array)

#clippinng
#go through every frame
for i in range(0, frames):
    off_clip_treshold = 0.7*max(abs(off_clipped[i]))
    on_clip_treshold  = 0.7*max(abs(on_clipped[i]))

    for j in range(0, samples_per_scope):
        #off
        if  off_clipped[i][j] >=  off_clip_treshold:
            off_clipped[i][j] =   1
        elif off_clipped[i][j] <= -off_clip_treshold:
             off_clipped[i][j] =  -1
        else:
            off_clipped[i][j] = 0
        #on
        if  on_clipped[i][j] >=  on_clip_treshold:
            on_clipped[i][j] =   1
        elif on_clipped[i][j] <= -on_clip_treshold:
             on_clipped[i][j] =  -1
        else:
            on_clipped[i][j] = 0

#autocorr (not used because of speed)
#def autocorr(signal):
#    result = np.zeros(len(signal))
#    for lag in range(0, len(signal)):
#        suma = 0
#        for in_frame in range(0, len(signal)):
#            if in_frame + lag < len(signal):
#                suma += signal[in_frame]*signal[in_frame + lag]
#        result[lag] = suma
#    return result

def autocorr(signal):
    result = np.correlate(signal, signal, mode='full')
    return result[len(result)//2:]

off_autocorr = np.zeros(shape=(frames, samples_per_scope))
on_autocorr = np.zeros(shape=(frames, samples_per_scope))
for i in range(0, frames):
    off_autocorr[i] = autocorr(off_clipped[i])
    on_autocorr[i] = autocorr(on_clipped[i])

treshold = int(samples*(1/500)) #samples * time / freq (500Hz)

off_lag = np.zeros(frames)
on_lag = np.zeros(frames)
off_lag_value = np.zeros(frames)
on_lag_value = np.zeros(frames)
off_ff = np.zeros(frames)
on_ff = np.zeros(frames)
for i in range(0, frames):
    off_lag[i]       = np.argmax(off_autocorr[i][treshold:]) + treshold
    on_lag[i]        = np.argmax(on_autocorr[i][treshold:])  + treshold
    off_lag_value[i] = np.max   (off_autocorr[i][treshold:])
    on_lag_value[i]  = np.max   (on_autocorr[i][treshold:])
    off_ff[i] = (16000/off_lag[i])
    on_ff[i] =  (16000/on_lag[i])

#plot mask off
plt.subplot(4, 1, 1)
plt.plot(x, off_array[0])
plt.ylim([-1, 1])
plt.title("Ramec")
plt.ylabel("y")
plt.xlabel("t[s]")
#plot clipped off
plt.subplot(4, 1, 2)
plt.plot(x, off_clipped[0])
plt.title("Centralni klipovani s 70%")
plt.ylabel("y")
plt.xlabel("t[s]")
#plot autocorr
plt.subplot(4, 1, 3)
plt.plot(off_autocorr[0])
plt.axvline(treshold, color='gray',  label="Prah")
plt.stem([off_lag[0]], [off_lag_value[0]], linefmt='red', label="Lag")
plt.title("Autokorelace")
plt.ylabel("y")
plt.xlabel("vzorky")
plt.legend()
#plot ff
plt.subplot(4, 1, 4)
plt.plot(off_ff, label="bez rousky")
plt.plot(on_ff, label="s rouskou")
plt.title("Zakladni frekvence ramcu")
plt.ylabel("f0")
plt.xlabel("ramce")
plt.legend()

plt.show()

#epxected value - stredni hodnota
#E = 1 / (imax - imin + 1) * sum(x[i])
#max = frames | min = 0
print("E(off):", np.mean(off_ff))
print("E(on):",  np.mean(on_ff))

#Variance - rozptyl
print("Var(off):", np.var(off_ff))
print("VAr(on):", np.var(on_ff))

#----(TASK 5)----
#DFT function (not used because of speed)
def dft(signal, N):
    signal_copy = np.zeros(1024, dtype='complex_')
    signal_copy = np.pad(signal, (0, N - samples_per_scope))
    result = np.zeros(N, dtype='complex_')
    for k in range(0, N):
        sums = 0
        for n in range(0, N):
            sums += signal_copy[n] * cmath.exp( (-2j * np.pi * k * n) / N)
        result[k] = sums
    return result

off_dft = np.zeros(shape=(frames, 512))
on_dft  = np.zeros(shape=(frames, 512))
off_dft_padded = np.zeros(shape=(frames, 1024), dtype='complex_')
on_dft_padded  = np.zeros(shape=(frames, 1024), dtype='complex_')

for i in range(0, frames):
    #off_dft_padded[i] = dft(off_array[i], 1024)
    #on_dft_padded[i] = dft(on_array[i], 1024)
    off_dft_padded[i] = np.fft.fft(off_array[i], 1024)
    on_dft_padded[i] = np.fft.fft(on_array[i], 1024)
    for k in range(0, 512):
        off_dft[i][k] = 10 * (np.log10(np.abs(off_dft_padded[i][k])**2))   #using second "inverted" half -> no need to invert axis
        on_dft[i][k]  = 10 * (np.log10(np.abs(on_dft_padded[i][k] )**2))

plt.subplot(2, 1, 1)
im = plt.imshow(off_dft.T, origin='upper', interpolation='nearest', aspect='auto', extent=[0, 1, 8000, 0])
plt.xlabel("cas")
plt.ylabel("frekvence")
plt.title("Spektogram bez rousky")
plt.colorbar(im)
ax = plt.gca()
ax.invert_yaxis()

plt.subplot(2, 1, 2)
im = plt.imshow(on_dft.T, origin='upper', interpolation='nearest', aspect='auto', extent=[0, 1, 8000, 0])
plt.gca().invert_yaxis
plt.xlabel("cas")
plt.ylabel("frekvence")
plt.title("Spektogram s rouskou")
plt.colorbar(im)
ax = plt.gca()
ax.invert_yaxis()

plt.show()

#----(TASK 6)----
freq_char = np.array(np.divide(np.abs(on_dft_padded), np.abs(off_dft_padded)))
freq_char_prum = np.zeros(512)
for i in range(0, 512):
    for k in range(0, frames):
        freq_char_prum[i] += freq_char[k][i]

freq_char_prum = freq_char_prum / frames

x = np.arange(0, 8000, 15.625)
plt.plot(x, 10 * np.log10(np.abs(freq_char_prum)**2))
plt.title("Frekvencni charakteristika rousky")
plt.xlabel("frekvence")
plt.ylabel("y")
plt.show()

#----(TASK 7)----
#IDFT
def idft(signal, N):
    signal_copy = np.zeros(1024, dtype='complex_')
    signal_copy = np.pad(signal, (0, N - samples_per_scope))
    result = np.zeros(N, dtype='complex_')
    for k in range(0, N):
        sums = 0
        for n in range(0, N):
            sums += signal_copy[n] * cmath.exp( (2j * np.pi * k * n) / N)
        result[k] = (sums / N)
    return result

idft_res = np.fft.ifft(freq_char_prum, 1024)[:512]
#idft_res = idft(freq_char_prum, 1024)[:512]

plt.plot(x, idft_res.real)
plt.title("Impulsni odezva rousky")
plt.xlabel("frekvence")
plt.show()

#----(TASK 8)----
smof = read("../audio/maskoff_sentence.wav")
smon = read("../audio/maskon_sentence.wav")
off_sentence = np.array(smof[1], dtype=float)  #our signal array
on_sentence  = np.array(smon[1], dtype=float)
final_sentence = signal.lfilter(np.real(idft_res), 1, off_sentence)

off_sentence -= np.mean(off_sentence)
on_sentence -= np.mean(on_sentence)
final_sentence -= np.mean(final_sentence)

off_sentence /= np.abs(off_sentence).max()
on_sentence /= np.abs(on_sentence).max()
final_sentence /= np.abs(final_sentence).max()


final_note = signal.lfilter(np.real(idft_res), 1, off_array_original)
final_note -= np.mean(final_note)
final_note /= np.abs(final_note).max()

write("sim_maskon_sentence.wav", sr, final_sentence)
write("sim_maskon_note.wav", sr, final_note)

plt.subplot(3, 1, 1)
plt.plot(off_sentence[24000:84000+15000])
plt.title("Veta bez rousky")
plt.xlabel("vzorky")
plt.ylabel("y")
plt.subplot(3, 1, 2)
plt.plot(on_sentence[10000:70000+15000])
plt.title("Veta s rouskou")
plt.xlabel("vzorky")
plt.ylabel("y")
plt.subplot(3, 1, 3)
plt.plot(final_sentence[24000:84000+15000])
plt.title("Veta se simulovanou rouskou")
plt.xlabel("vzorky")
plt.ylabel("y")

plt.show()

#plt.subplot(3, 1, 1)
#plt.plot(off_sentence[22000:24000])
#plt.title("Ramec 1 - bez rousky")
#plt.xlabel("vzorky")
#plt.ylabel("y")
#plt.subplot(3, 1, 2)
#plt.plot(on_sentence[22000-5000:24000-5000])
#plt.title("Ramec 1 - s rouskou")
#plt.xlabel("vzorky")
#plt.ylabel("y")
#plt.subplot(3, 1, 3)
#plt.plot(final_sentence[22000:24000])
#plt.title("Ramec 1 - sim.")
#plt.xlabel("vzorky")
#plt.ylabel("y")
#
#plt.show()

#----(TASK 12)----
off_clipped = np.copy(off_array)
on_clipped = np.copy(on_array)

#clippinng
#go through every frame
for i in range(0, frames):
    off_clip_treshold = 0.8*max(abs(off_clipped[i]))    # 0.7 -> 0.8
    on_clip_treshold  = 0.8*max(abs(on_clipped[i]))     # 0.7 -> 0.8

    for j in range(0, samples_per_scope):
        #off
        if  off_clipped[i][j] >=  off_clip_treshold:
            off_clipped[i][j] =   1
        elif off_clipped[i][j] <= -off_clip_treshold:
             off_clipped[i][j] =  -1
        else:
            off_clipped[i][j] = 0
        #on
        if  on_clipped[i][j] >=  on_clip_treshold:
            on_clipped[i][j] =   1
        elif on_clipped[i][j] <= -on_clip_treshold:
             on_clipped[i][j] =  -1
        else:
            on_clipped[i][j] = 0

off_autocorr = np.zeros(shape=(frames, samples_per_scope))
on_autocorr = np.zeros(shape=(frames, samples_per_scope))
for i in range(0, frames):
    off_autocorr[i] = autocorr(off_clipped[i])
    on_autocorr[i] = autocorr(on_clipped[i])

treshold = int(samples*(1/500)) #samples * time / freq (500Hz)

off_lag = np.zeros(frames)
on_lag = np.zeros(frames)
off_lag_value = np.zeros(frames)
on_lag_value = np.zeros(frames)
off_ff = np.zeros(frames)
on_ff = np.zeros(frames)
for i in range(0, frames):
    off_lag[i]       = np.argmax(off_autocorr[i][treshold:]) + treshold
    on_lag[i]        = np.argmax(on_autocorr[i][treshold:])  + treshold
    off_lag_value[i] = np.max   (off_autocorr[i][treshold:])
    on_lag_value[i]  = np.max   (on_autocorr[i][treshold:])
    off_ff[i] = (16000/off_lag[i])
    on_ff[i] =  (16000/on_lag[i])

plt.subplot(2, 1, 1)
plt.plot(off_ff)
plt.plot(on_ff)
plt.title("Graf s dvojitym lagem")
plt.xlabel("ramce")
plt.ylabel("f0")

off_lag_med = np.median(off_ff)
on_lag_med = np.median(on_ff)

off_wrong_lag = []
on_wrong_lag = []

for i in range(0, frames):
    if off_ff[i] > off_lag_med + 20:
        off_ff[i] = off_lag_med
        off_wrong_lag.append(i)
    elif off_ff[i] < off_lag_med - 20:
        off_ff[i] = off_lag_med
        off_wrong_lag.append(i)
    
    if on_ff[i] > on_lag_med + 20:
        on_ff[i] = off_lag_med
        on_wrong_lag.append(i)
    elif on_ff[i] < on_lag_med - 20:
        on_ff[i] = off_lag_med
        on_wrong_lag.append(i)

for i in range(0, len(off_wrong_lag)):
    plt.axvline(off_wrong_lag[i], color="gray")
for i in range(0, len(on_wrong_lag)):
    plt.axvline(on_wrong_lag[i], color="gray")

plt.subplot(2, 1, 2)
plt.plot(off_ff)
plt.plot(on_ff)
plt.title("Graf s opravenym lagem")
plt.xlabel("ramce")
plt.ylabel("f0")
for i in range(0, len(off_wrong_lag)):
    plt.axvline(off_wrong_lag[i], color="gray")
for i in range(0, len(on_wrong_lag)):
    plt.axvline(on_wrong_lag[i], color="gray")

plt.show()

#2 off frames are wrong
plt.subplot(2, 1, 1)
plt.plot(off_autocorr[off_wrong_lag[0]])
plt.axvline(treshold, color='gray',  label="Prah")
plt.stem([off_lag[off_wrong_lag[0]]], [off_lag_value[off_wrong_lag[0]]], linefmt='red', label="Lag")
plt.title("Autokorelace s dvojitym lagem")
plt.ylabel("y")
plt.xlabel("vzorky")
plt.legend()

plt.subplot(2, 1, 2)
plt.plot(off_autocorr[off_wrong_lag[1]])
plt.axvline(treshold, color='gray',  label="Prah")
plt.stem([off_lag[off_wrong_lag[1]]], [off_lag_value[off_wrong_lag[1]]], linefmt='red', label="Lag")
plt.title("Autokorelace s dvojitym lagem")
plt.ylabel("y")
plt.xlabel("vzorky")
plt.legend()

plt.show()


#----(TASK 15)----
x = np.arange(0, 0.02, 0.0000625)
scope_lenght = 0.025 #25ms
samples_per_scope = int(samples/(time/scope_lenght))
frames = 98 #remove single frame to fit

off_array = np.zeros(shape=(frames, samples_per_scope))
on_array = np.zeros(shape=(frames, samples_per_scope))
half = 160 #10ms

#rozdeleni na ramce
for i in range(0, frames):
    off_array[i] = off_array_un[int(i*half) : int(samples_per_scope + i * half)]
    on_array[i]  =  on_array_un[int(i*half) : int(samples_per_scope + i * half)]

plt.subplot(3, 1, 1)
plt.plot(x, off_array[60][:320], label="bez rousky")
plt.plot(x, on_array[60][:320], label="s rouskou")
plt.ylabel("y")
plt.xlabel("t[s]")
plt.title("Ramec 60 pred zarovnanim")
plt.legend()

off_clipped = np.copy(off_array)
on_clipped = np.copy(on_array)

#clippinng
for i in range(0, frames):
    off_clip_treshold = 0.7*max(abs(off_clipped[i]))
    on_clip_treshold  = 0.7*max(abs(on_clipped[i]))

    for j in range(0, samples_per_scope):
        #off
        if  off_clipped[i][j] >=  off_clip_treshold:
            off_clipped[i][j] =   1
        elif off_clipped[i][j] <= -off_clip_treshold:
             off_clipped[i][j] =  -1
        else:
            off_clipped[i][j] = 0
        #on
        if  on_clipped[i][j] >=  on_clip_treshold:
            on_clipped[i][j] =   1
        elif on_clipped[i][j] <= -on_clip_treshold:
             on_clipped[i][j] =  -1
        else:
            on_clipped[i][j] = 0

def corr(signal1, signal2):
    result = np.correlate(signal1, signal2, mode='full')
    return result[len(result)//2:]

off_on_corr = np.zeros(shape=(frames, samples_per_scope))
on_off_corr = np.zeros(shape=(frames, samples_per_scope))
for i in range(0, frames):
    off_on_corr[i] = corr(off_clipped[i], on_clipped[i])
    on_off_corr[i] = corr(on_clipped[i], off_clipped[i])

phase_shift = np.zeros(frames, dtype=int)

for i in range(0, frames):
    if off_on_corr[i].argmax() < on_off_corr[i].argmax():
        phase_shift[i] = off_on_corr[i].argmax()
        off_array[i] = np.pad(off_array[i][phase_shift[i]:], (0, phase_shift[i]))
        on_array[i] =  np.pad(on_array[i][:samples_per_scope-phase_shift[i]], (phase_shift[i], 0))
        on_array[i] = np.roll(on_array[i], -phase_shift[i])
    else:
        phase_shift[i] = on_off_corr[i].argmax()
        on_array[i] =  np.pad(on_array[i][phase_shift[i]:], (0, phase_shift[i]))
        off_array[i] = np.pad(off_array[i][:samples_per_scope-phase_shift[i]], (phase_shift[i], 0))
        off_array[i] = np.roll(off_array[i], -phase_shift[i])

plt.subplot(3, 1, 2)
plt.plot(x, off_array[60][:320], label="bez rousky")
plt.plot(x, on_array[60][:320], label="s rouskou")
plt.ylabel("y")
plt.xlabel("t[s]")
plt.title("Ramec 60 po zarovnani")
plt.legend()

plt.subplot(3, 1, 3)
plt.plot(phase_shift)
plt.ylabel("posun [vzorky]")
plt.xlabel("ramce")
plt.title("Posun v ramcich")

plt.show()
