import matplotlib.pyplot as plt
import numpy as np

Niter       = 15

N           = 50
# k-th cell is 75 % from size of sliding window
KTH_CELL    = (N*75) // 100
# scaling factor calculating for Pfa
dPfa_0 = np.zeros(Niter)
for k in range(Niter):
    alpha = k + 1
    dPfa_0[k] = (np.math.factorial(N) * np.math.factorial(alpha + N - KTH_CELL)) / (np.math.factorial(N - KTH_CELL) * np.math.factorial(alpha + N))
    
N            = 30
# k-th cell is 75 % from size of sliding window
KTH_CELL     = (N*75) // 100
# scaling factor calculating for Pfa
dPfa_1 = np.zeros(Niter)
for k in range(Niter):
    alpha = k + 1
    dPfa_1[k] = (np.math.factorial(N) * np.math.factorial(alpha + N - KTH_CELL)) / (np.math.factorial(N - KTH_CELL) * np.math.factorial(alpha + N))


N            = 10
# k-th cell is 75 % from size of sliding window
KTH_CELL     = (N*75) // 100
# scaling factor calculating for Pfa
dPfa_2 = np.zeros(Niter)
for k in range(Niter):
    alpha = k + 1
    dPfa_2[k] = (np.math.factorial(N) * np.math.factorial(alpha + N - KTH_CELL)) / (np.math.factorial(N - KTH_CELL) * np.math.factorial(alpha + N))



plt.figure()
plt.semilogy(np.arange(Niter), dPfa_0, '--b', label='N= {}, kth = {}'.format(50, (50*75) // 100))
plt.semilogy(np.arange(Niter), dPfa_1, '--r', label='N= {}, kth = {}'.format(30, (30*75) // 100))
plt.semilogy(np.arange(Niter), dPfa_2, '--g', label='N= {}, kth = {}'.format(10, (10*75) // 100))
plt.xlabel('Threshold Scale Factor')
plt.ylabel('Probability of False Alarm')
plt.title('Scale Factor calculation')
plt.legend()
plt.ylim((10**(-6), 10**(-1)))
plt.grid()

plt.show()