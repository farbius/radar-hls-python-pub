import numpy as np
import matplotlib.pyplot as plt


Fs      = 100e6 # sample rate
f0      = 16e6  # signal frequency
ts      = 1/Fs
Nsmpl   = 1024
SNR_dB  = 10   # signal-to-noise ratio

t_axis  = np.linspace(0, Nsmpl, Nsmpl) * ts
f_axis  = np.linspace(0, Nsmpl, Nsmpl) * Fs / Nsmpl
s       = np.exp(1j*np.pi*2*f0*t_axis)

# signal mixed with noise
u       = s + 10**(-SNR_dB/20)*np.random.randn(Nsmpl)
# normalized Numpy FFT 
uF      = np.abs(np.fft.fft(u)/Nsmpl)

plt.figure()
plt.plot(f_axis/1e6, 20*np.log10(uF), '.-b', label='SNR = {} dB'.format(SNR_dB))
plt.plot(f_axis/1e6, (-10*np.log10(Nsmpl)-SNR_dB)*np.ones(Nsmpl), '--r', label='Noise Floor')
plt.title("FFT {} points, FFT noise floor is {:3.2f} dB".format(Nsmpl, 10*np.log10(Nsmpl)), fontweight="bold", fontsize=14)
plt.legend(loc='upper right')
plt.grid()
plt.xlabel('f, MHz')
plt.ylabel('magnitude, dB')
plt.show()