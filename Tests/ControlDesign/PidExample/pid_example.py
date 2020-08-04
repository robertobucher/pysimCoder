# Modules

import matplotlib.pyplot as plt
from control import *
from control.matlab import *
from numpy import pi, linspace,  sin, sqrt
from supsisim.RCPblk import *
from supsictrl.ctrl_utils import *
from supsictrl.ctrl_repl import *

g=tf([1],[1,6,5])
bode_plot(g);
plt.legend(['G(s)'],prop={'size':10})
plt.show()
# savefig('bodeplant.eps')

wgc = 10         # Desired Bandwidth
desiredPM = 60   # Desired Phase margin

# PI part
Ti=0.15
Gpi=tf([Ti,1],[Ti,0])
print("PI part is: ", Gpi)
plt.figure()
bode(g,dB=True,linestyle='dashed');
bode(Gpi*g,dB=True);
plt.legend((['G(s)','Gpi(s)*G(s)']),prop={'size':10})
plt.show()
# savefig('bodeplantpi.eps')

mag,phase,omega = bode(Gpi*g,[wgc],Plot=False)
ph = phase[0]/pi*180

if ph>=0:
    ph = ph-360;

Phase = -180+desiredPM
dPM = Phase-ph
print("Additional phase from Lead part: ", dPM)
# Lead part
dPMrad = dPM/180*pi
alfa = (1+sin(dPMrad))/(1-sin(dPMrad));
print("Alpha is: ", alfa)

TD = 1/(sqrt(alfa)*wgc);
Glead = tf([alfa*TD,1],[TD,1])
print("Lead part is: ", Glead)

plt.figure()
bode(g,dB=True,linestyle='dashed');
bode(Gpi*g,dB=True,linestyle='dotted');
bode(Gpi*Glead*g, dB=True);
plt.legend((['G(s)','Gpi(s)*G(s)','Gpi(s)*GLead(s)*G(s)']),prop={'size':10})
plt.show()
# savefig('bodeplantpilead.eps')

mag,phase,omega = bode(Gpi*Glead*g,[wgc],Plot=False)
print("Phase at wgc is: ", phase[0])
K=1/mag[0]
print("Gain to have MAG at gwc 0dB: ", K)
plt.figure()
bode(g,dB=True,linestyle='dashed');
bode(Gpi*g,dB=True,linestyle='dotted');
bode(Gpi*Glead*g, dB=True,linestyle='-.');
bode(K*Gpi*Glead*g, dB=True);
plt.legend((['G(s)','Gpi(s)*G(s)','Gpi(s)*GLead(s)*G(s)','K*Gpi(s)*GLead(s)*G(s)']),prop={'size':10})
plt.show()
# savefig('bodeplantpileadK.eps')

Contr = K*Gpi*Glead
print("Full controller: ", Contr)
mag,phase,omega = bode(K*Gpi*Glead*g,[wgc],Plot=False)
print("Data at wgc - wgc: ", omega[0], "Magnitude: ",mag[0], "Phase: ",phase[0])

gt=feedback(K*Gpi*Glead*g,1)
t=linspace(0,1.5,300)
y,t = step(gt,t)
plt.figure()
plt.plot(t,y), plt.xlabel('t'), plt.ylabel('y'), plt.title('Step response of the controlled plant')
plt.grid()
plt.show()
# savefig('pidexastep.eps')


