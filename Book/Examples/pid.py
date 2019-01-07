# Modules

from matplotlib.pyplot import *
from control import *
from numpy import pi, linspace
from scipy import sin, sqrt
from supsictrl.RCPblk import *
from supsictrl.yottalab import *

g=tf([1],[1,6,5])
bode(g,dB=True);
subplot(211), legend(['G(s)'],prop={'size':10})
show()
# savefig('bodeplant.eps')

wgc = 10         # Desired Bandwidth
desiredPM = 60   # Desired Phase margin

# PI part
Ti=0.15
Gpi=tf([Ti,1],[Ti,0])
print "PI part is: ", Gpi
figure()
bode(g,dB=True,linestyle='dashed');
hold
bode(Gpi*g,dB=True);
subplot(211), legend((['G(s)','Gpi(s)*G(s)']),prop={'size':10})
show()
# savefig('bodeplantpi.eps')

mag,phase,omega = bode(Gpi*g,[wgc],Plot=False)
ph = phase[0]
if ph>=0:
    ph = phase[0]-360;

Phase = -180+desiredPM
dPM = Phase-ph
print "Additional phase from Lead part: ", dPM
# Lead part
dPMrad = dPM/180*pi
alfa = (1+sin(dPMrad))/(1-sin(dPMrad));
print "Alpha is: ", alfa

TD = 1/(sqrt(alfa)*wgc);
Glead = tf([alfa*TD,1],[TD,1])
print "Lead part is: ", Glead

figure()
bode(g,dB=True,linestyle='dashed');
hold
bode(Gpi*g,dB=True,linestyle='dotted');
bode(Gpi*Glead*g, dB=True);
subplot(211), legend((['G(s)','Gpi(s)*G(s)','Gpi(s)*GLead(s)*G(s)']),prop={'size':10})
show()
# savefig('bodeplantpilead.eps')

mag,phase,omega = bode(Gpi*Glead*g,[wgc],Plot=False)
print "Phase at wgc is: ", phase[0]
K=1/mag[0]
print "Gain to have MAG at gwc 0dB: ", K
figure()
bode(g,dB=True,linestyle='dashed');
hold
bode(Gpi*g,dB=True,linestyle='dotted');
bode(Gpi*Glead*g, dB=True,linestyle='-.');
bode(K*Gpi*Glead*g, dB=True);
subplot(211),legend((['G(s)','Gpi(s)*G(s)','Gpi(s)*GLead(s)*G(s)','K*Gpi(s)*GLead(s)*G(s)']),prop={'size':10})
show()
# savefig('bodeplantpileadK.eps')

Contr = K*Gpi*Glead
print "Full controller: ", Contr
mag,phase,omega = bode(K*Gpi*Glead*g,[wgc],Plot=False)
print "Data at wgc - wgc: ", omega[0], "Magnitude: ",mag[0], "Phase: ",phase[0]

gt=feedback(K*Gpi*Glead*g,1)
t=linspace(0,1.5,300)
y,t = step(gt,t)
figure()
plot(t,y), xlabel('t'), ylabel('y'), title('Step response of the controlled plant')
grid()
show()
# savefig('pidexastep.eps')


