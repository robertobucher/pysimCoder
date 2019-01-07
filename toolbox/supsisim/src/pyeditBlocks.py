from supsisim.RCPblk import RCPblk

from control import tf2ss, TransferFunction
from scipy import mat, shape, size, array, zeros
from numpy import reshape, hstack

def stepBlk(pout, initTime, Val):
    """

    Call:   stepBlk(pout, initTime, Val)

    Parameters
    ----------
       pout: connected output port(s)
       initTime : Step Time
       Val : Step Value

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('step',[],pout,[0,0],0,[initTime, Val],[])
    return blk

def sineBlk(pout, Amp, Freq, Phase, Bias, Delay):
    """

    Call:   sineBlk(pout, Amp, Freq, Phase, Bias, Delay)

    Parameters
    ----------
       pout: connected output port(s)
       Amp : Amplitude
       Freq : Freq
       Phase : Phase
       Bias : Bias
       Delay : Delay

    Returns
    -------
        blk  : RCPblk
    
    """

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('sinus', [],pout,[0,0],0,[Amp, Freq, Phase, Bias, Delay],[])
    return blk

def squareBlk(pout, Amp, Period, Width, Bias, Delay):
    """

    Call:   squareBlk(pout, Amp, Period, Width, Bias, Delay)

    Parameters
    ----------
       pout: connected output port(s)
       Amp : Amplitude
       Period : Period
       Width : Width
       Bias : Bias
       Delay : Delay

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('square',[],pout,[0,0],0,[Amp, Period, Width, Bias, Delay],[])
    return blk

def printBlk(pin):
    """

    Call:   printBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('print',pin,[],[0,0],1,[],[])
    return blk

def plotBlk(pin, fname):
    """

    Call:   plotBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('plot',pin,[],[0,0],1,[],[], fname)
    return blk

def toFileBlk(pin, fname):
    """

    Call:   toFileBlk(pin, fname)

    Parameters
    ----------
       pin: connected input port(s)
       fname : File name

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('toFile',pin,[],[0,0],1,[],[], fname)
    return blk

def sumBlk(pin, pout, Gains):
    """

    Call:   sumBlk(pin, pout, Gains)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       Gains : Gains

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != size(Gains)):
        raise ValueError("Number of inputs (%i) should match Gain Size (%i)" % (size(pin),size(Gains)))
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('sum',pin,pout,[0,0],1,Gains,[])
    return blk

def saturBlk(pin, pout, satP, satN):
    """

    Call:   saturBlk(pin, pout, satP, satN)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       satP : Upper saturation
       satN : Lower saturation

    Returns
    -------
       blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pin))
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('saturation',pin,pout,[0,0],1,[satP, satN],[])
    return blk

def dssBlk(pin,pout,sys,X0=[]):
    """

    Discrete state space block

    Call: dssBlk(pin,pout, sys,X0)

    Parameters
    ----------
       pin : connected input ports
       pout: connected output ports
       sys: Discrete system in SS form
       X0: Initial conditions

    Returns
    -------
       blk  : RCPblk

    """
    
    if isinstance(sys, TransferFunction):
        sys=tf2ss(sys)

    nin = size(pin)
    ni = shape(sys.B)[1];
    if (nin != ni):
        raise ValueError("Block have %i inputs: received %i input ports" % (nin,ni))
    
    no = shape(sys.C)[0]
    nout = size(pout)
    if(no != nout):
        raise ValueError("Block have %i outputs: received %i output ports" % (nout,no))
        
    a  = reshape(sys.A,(1,size(sys.A)),'C')
    b  = reshape(sys.B,(1,size(sys.B)),'C')
    c  = reshape(sys.C,(1,size(sys.C)),'C')
    d  = reshape(sys.D,(1,size(sys.D)),'C')
    nx = shape(sys.A)[0]

    if(size(X0) == nx):
        X0 = reshape(X0,(1,size(X0)),'C')
    else:
        X0 = mat(zeros((1,nx)))

    indA = 0
    indB = nx*nx
    indC =indB + nx*ni
    indD = indC + nx*no
    indX = indD + ni*no
    intPar = [nx,ni,no,indA, indB, indC, indD, indX]
    realPar = hstack((a,b,c,d,X0))

    if d.any() == True:
        uy = 1
    else:
        uy = 0
    
    blk = RCPblk('dss',pin,pout,[0,nx],uy,realPar,intPar)
    return blk

def cssBlk(pin,pout,sys,X0=[]):
    """ 

    Continous state space block

    Call: cssBlk(pin,pout, sys,X0)

    Parameters
    ----------
        pin : connected input ports
        pout: connected output ports
        sys: Discrete system in SS form
        X0: Initial conditions

    Returns
    -------
        blk  : RCPblk

    """
    
    if isinstance(sys, TransferFunction):
        sys=tf2ss(sys)

    nin = size(pin)
    ni = shape(sys.B)[1];
    if (nin != ni):
        raise ValueError("Block have %i inputs: received %i input ports" % (nin,ni))
    
    no = shape(sys.C)[0]
    nout = size(pout)
    if(no != nout):
        raise ValueError("Block have %i outputs: received %i output ports" % (nout,no))
        
    a  = reshape(sys.A,(1,size(sys.A)),'C')
    b  = reshape(sys.B,(1,size(sys.B)),'C')
    c  = reshape(sys.C,(1,size(sys.C)),'C')
    d  = reshape(sys.D,(1,size(sys.D)),'C')
    nx = shape(sys.A)[0]

    if(size(X0) == nx):
        X0 = reshape(X0,(1,size(X0)),'C')
    else:
        X0 = mat(zeros((1,nx)))

    indA = 1
    indB = indA + nx*nx
    indC = indB + nx*ni
    indD = indC + nx*no
    indX = indD + ni*no
    intPar = [nx,ni,no, indA, indB, indC, indD, indX]
    realPar = hstack((mat([0.0]),a,b,c,d,X0))

    if d.any() == True:
        uy = 1
    else:
        uy = 0
    
    blk = RCPblk('css',pin,pout,[nx,0],uy,realPar,intPar)
    return blk

def matmultBlk(pin, pout, Gains):
    """

    Matrix multiplication of the input signals
    
    Call: matmultBlk(pin,pout, Gains)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       Gains : Gains

    Returns
    -------
        blk  : RCPblk

    """
    
    Gains = mat(Gains)
    n,m = shape(Gains)
    if(size(pin) != m):
        raise ValueError("Block should have %i input port; received %i." % (m,size(pin)))
    if(size(pout) != n):
        raise ValueError("Block should have %i output port; received %i." % (n,size(pout)))
    realPar  = reshape(Gains,(1,size(Gains)),'C')
    blk = RCPblk('mxmult',pin,pout,[0,0],1,realPar,[n,m])
    return blk

def constBlk(pout, val):
    """

    Call:   constBlk(pout, val)

    Parameters
    ----------
       pout: connected output port(s)
       val : Value

    Returns
    -------
    blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))
    blk = RCPblk('constant',[],pout,[0,0],0,[val],[])
    return blk

def absBlk(pin, pout):
    """

    Call:   absBlk(pin, pout)

    Parameters
    ----------
        pin : connected input ports
        pout: connected output ports

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != size(pin)):
        raise ValueError("Block should have same input and output port sizes; received %i %i." % (size(pin),size(pout)))
    blk = RCPblk('absV',pin,pout,[0,0],1,[],[])
    return blk

def prodBlk(pin, pout):
    """

    Call:   prodBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('prod',pin,pout,[0,0],1,[],[])
    return blk

def extdataBlk(pout, length, fname):
    """

    Call:   extdataBlk(pout, length, fname)

    Parameters
    ----------
       pout: connected output port(s)
       length : Data length
       fname : File name

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('extdata',[],pout,[0,0],0,[],[length,0],fname)
    return blk

def epos_MotIBlk(pin, ID):
    """

    Call:   epos_MotIBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
        blk  : RCPblk

    """
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_motI',pin,[],[0,0],1,[],[ID])
    return blk

def epos_MotXBlk(pin, ID):
    """

    Call:   epos_MotXBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain
       derGain : Deriv. gain
       Vff : Vel. FeedForw
       Aff : Acc. Feed Forw.

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_motX',pin,[],[0,0],1,[],[ID])
    return blk

def epos_EncBlk(pout, ID, res):
    """

    Call:   epos_EncBlk(pout, ID, res)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       res : Resolution

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_enc',[],pout,[0,0],0,[4*res],[ID])
    return blk

def maxon_MotBlk(pin, ID, propGain, intGain):
    """

    Maxon driver for torque control

    Call: maxon_MotBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
       blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('maxon_mot',pin,[],[0,0],1,[propGain, intGain],[ID])
    return blk

def maxon_EncBlk(pout, ID, res):
    """

    Call:   maxon_EncBlk(pout, ID, res)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       res : Resolution

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('maxon_enc',[],pout,[0,0],0,[4*res],[ID])
    return blk

def epos_areadBlk(pout, ID, ch):
    """

    Call:   epos_areadBlk(pout, ID, ch)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       ch : Channel [0/1]

    Returns
    -------
       blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('epos_canopen_aread',[],pout,[0,0],0,[],[ID, ch])
    return blk

def init_encBlk(pin, pout, trgtime, defv, offset):
    """

    Call:   init_encBlk(pin, pout, trgtime, defv, offset)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       trgtime : Trigger Time
       defv : Default Output
       offset : Offset

    Returns
    -------
        blk  : RCPblk

    """
    
    if (size(pout) != 1) or (size(pin) != 1):
        raise ValueError("Block should have 1 input and 1 output port; received %i and %i." % (size(pin),size(pout)))

    blk = RCPblk('init_enc',pin,pout,[0,0],1,[trgtime, defv, offset, 0.0],[])
    return blk

def comediADBlk(pout, dev, ch, cr):
    """

    Call:   comediADBlk(pout, dev, ch, cr)

    Parameters
    ----------
       pout: connected output port(s)
       dev : Device
       ch : Channel
       cr : Range

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pout) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pout))

    blk = RCPblk('comedi_analog_input',[],pout,[0,0],0,[],[ch, cr],dev)
    return blk

def comediDABlk(pin, dev, ch, cr):
    """

    Call:   comediDABlk(pin, dev, ch, cr)

    Parameters
    ----------
       pin: connected input port(s)
       dev : Device
       ch : Channel
       cr : Range

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pin) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pin))

    blk = RCPblk('comedi_analog_output',pin,[],[0,0],1,[],[ch, cr],dev)
    return blk

def comediDIBlk(pout, dev, ch):
    """

    Call:   comediDIBlk(pout, dev, ch)

    Parameters
    ----------
       pout: connected output port(s)
       dev : Device
       ch : Channel

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pout) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pout))

    blk = RCPblk('comedi_digital_input',[],pout,[0,0],0,[],[ch],dev)
    return blk

def comediDOBlk(pin, dev, ch, thr):
    """

    Call:   comediDOBlk(pin, dev, ch, thr)

    Parameters
    ----------
       pin: connected input port(s)
       dev : Device
       ch : Channel
       thr : Threshold

    Returns
    -------
        blk  : RCPblk

    """
    
    if size(pin) != 1:
        raise ValueError("Block should have 1 input port; received %i !" % size(pin))

    blk = RCPblk('comedi_digital_output',pin,[],[0,0],1,[thr],[ch],dev)
    return blk

def baumer_EncBlk(pout, ID, res):
    """

    Call:   baumer_EncBlk(pout, ID, res)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       res : Resolution

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('baumer_enc',[],pout,[0,0],0,[4*res],[ID])
    return blk

def switchBlk(pin, pout, cond, val, pers):
    """

    Call:   switchBlk(pin, pout, cond, val, pers)

    Parameters
    ----------
        pin: connected input ports (3)
        pout: connected output port
        cond:  0 >, 1 <
        val:   value to compare
        pers:  switch can change again (0) or is fixed (1)

        Output switches from input 1 to input 2 if the condition is reached
        (input 3 > or <) than val;
        If pers is 1 the system doesn't switch back again if the condition is
        no more satisfied

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 3):
        raise ValueError("Block should have 3 input ports; received %i." % size(pin))

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('switcher',pin,pout,[0,0],1,[val],[cond, pers])
    return blk

def lutBlk(pin, pout, coeff):
    """

    Call:   lutBlk(pin, pout, coeff)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       coeff : Coeff 

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should 1 input port; received %i." % size(pin))

    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('lut',pin,pout,[0,0],1,[coeff],[size(coeff)])
    return blk

def intgBlk(pin,pout,X0=0.0):
    """ 

    Continous integral block

    Call: intgBlk(pin,pout,X0)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       X0 : Initial conditions

    Returns
    -------
        blk  : RCPblk

    """
    
    nin = size(pin)
    if (nin != 1):
        raise ValueError("Block have 1 input: received %i input ports" % nin)

    nout = size(pout)
    if(nout != 1):
        raise ValueError("Block have 1 output1: received %i output ports" % nout)
        
    blk = RCPblk('integral',pin,pout,[1,0],0,[0.0 ,X0],[])
    return blk

def zdelayBlk(pin, pout, X0=0.0):
    """

    Call:   zdelayBlk(pin, pout, X0=0.0)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       X0: Initial conditions

    Returns
    -------
        blk  : RCPblk

    """
    
    nin = size(pin)
    if (nin != 1):
        raise ValueError("Block have 1 input: received %i input ports" % nin)

    nout = size(pout)
    if(nout != 1):
        raise ValueError("Block have 1 output1: received %i output ports" % nout)
        
    blk = RCPblk('unitDelay',pin,pout,[0,1],0,[X0],[])
    return blk

def trigBlk(pin, pout, tp):
    """

    Call:   trigBlk(pin, pout, tp)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       tp : sin->1 cos->2 tan->3

    Returns
    -------
        blk  : RCPblk

    """
    
    nin = size(pin)
    if (nin != 1):
        raise ValueError("Block have 1 input: received %i input ports" % nin)

    nout = size(pout)
    if(nout != 1):
        raise ValueError("Block have 1 output1: received %i output ports" % nout)
        
    blk = RCPblk('trigo',pin,pout,[0,0],1,[],[tp])
    return blk

def unixsocketCBlk(pin, sockname):
    """

    Call:   unixsocketCBlk(pin, sockname)

    Parameters
    ----------
       pin: connected input port(s)
       sockname : Socket

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk('unixsockC',pin,[],[0,0],1,[],[0],'/tmp/'+sockname)
    return blk

def unixsocketSBlk(pout, sockname, defvals):
    """

    Call:   unixsocketSBlk(pout, sockname, defvals)

    Parameters
    ----------
       pout: connected output port(s)
       sockname : Socket
       defvals : Default outputs

    Returns
    -------
        blk  : RCPblk

    """
    
    outputs = len(pout)
    vals = zeros(outputs,float)
    if len(defvals) > outputs:
        N=outputs
    else:
        N = len(defvals)

    for n in range(N):
        vals[n]=defvals[n]
        
    blk = RCPblk('unixsockS',[],pout,[0,0],0,vals,[0,0],'/tmp/'+sockname)
    return blk

def FH_3XXX_INIT_XBlk(ID, Kp, Kd, rescnt):
    """

    Call:   FH_3XXX_INIT_XBlk(ID, Kp, Kd, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Kd : Deriv. gain
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_3XXX_INIT_X',[],[],[0,0],0,[Kp, Kd],[ID, rescnt])
    return blk

def FH_5XXX_INIT_XBlk(ID, Kp, Kd, rescnt):
    """

    Call:   FH_5XXX_INIT_XBlk(ID, Kp, Kd, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Kd : Deriv. gain
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_5XXX_INIT_X',[],[],[0,0],0,[Kp, Kd],[ID, rescnt])
    return blk

def FH_3XXX_INIT_VBlk(ID, Kp, Ki, rescnt):
    """

    Call:   FH_3XXX_INIT_VBlk(ID, Kp, Ki, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Ki : Intgr. gain
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_3XXX_INIT_V',[],[],[0,0],0,[Kp, Ki],[ID, rescnt])
    return blk

def FH_5XXX_INIT_VBlk(ID, Kp, Ki, rescnt):
    """

    Call:   FH_5XXX_INIT_VBlk(ID, Kp, Ki, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Ki : Intgr. gain
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_5XXX_INIT_V',[],[],[0,0],0,[Kp, Ki],[ID, rescnt])
    return blk

def FH_5XXX_INIT_TQBlk(ID, Kp, Ti, rescnt):
    """

    Call:   FH_5XXX_INIT_VBlk(ID, Kp, Ki, rescnt)

    Parameters
    ----------
       ID : Device ID
       Kp : Prop. gain
       Ti : Intgr. time
       rescnt: reset counter

    Returns
    -------
        blk  : RCPblk

    """    
    blk = RCPblk('FH_5XXX_INIT_TQ',[],[],[0,0],0,[Kp, Ti],[ID, rescnt])
    return blk

def FH_3XXX_XBlk(pin, ID):
    """

    Call:   FH_3XXX_XBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('FH_3XXX_X',pin,[],[0,0],1,[],[ID])
    return blk

def FH_5XXX_XBlk(pin, ID):
    """

    Call:   FH_5XXX_XBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
 
    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('FH_5XXX_X',pin,[],[0,0],1,[],[ID])
    return blk

def FH_3XXX_VBlk(pin, ID):
    """

    Call:   FH_3XXX_VBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
  
    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('FH_3XXX_V', pin, [], [0,0], 1, [], [ID])
    return blk

def FH_5XXX_VBlk(pin, ID):
    """

    Call:   FH_5XXX_VBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
   
    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('FH_5XXX_V', pin, [], [0,0], 1, [], [ID])
    return blk

def FH_5XXX_setTQBlk(pin, ID):
    """

    Call:   FH_5XXX_setTQBlk(pin, ID)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
  
    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('FH_5XXX_setTQ', pin, [], [0,0], 1, [], [ID])
    return blk

def FH_3XXX_ENCBlk(pout, ID, res):
    """

    Call:   FH_3XXX_ENCBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       res : Resolution

    Returns
    -------
        blk: RCPblk

    """

    blk = RCPblk('FH_3XXX_ENC',[],pout,[0,0],0,[res],[ID])
    return blk

def FH_5XXX_ENCBlk(pout, ID, res):
    """

    Call:   FH_5XXX_ENCBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       res : Resolution
 
    Returns
    -------
        blk: RCPblk

    """

    blk = RCPblk('FH_5XXX_ENC',[],pout,[0,0],0,[res],[ID])
    return blk

def FH_3XXX_getVBlk(pout, ID):
    """

    Call:   FH_3XXX_getVBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_3XXX_getV', [], pout, [0,0], 0, [], [ID])
    return blk

def FH_5XXX_getVBlk(pout, ID):
    """

    Call:   FH_5XXX_getVBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_5XXX_getV', [], pout, [0,0], 0, [], [ID])
    return blk

def FH_5XXX_getTQBlk(pout, ID):
    """

    Call:   FH_5XXX_getTQBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('FH_5XXX_getTQ', [], pout, [0,0], 0, [], [ID])
    return blk

def FH_3XXX_ADBlk(pout, ID):
    """

    Call:   FH_3XXX_ADBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
 
    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('FH_3XXX_AD',[],pout,[0,0],0,[],[ID])
    return blk

def FH_5XXX_ADBlk(pout, ID, ch):
    """

    Call:   FH_5XXX_ADBlk(pout, ID)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       ch: Channel (0 or 1)

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pout) != 1):
        raise ValueError("Block should have 1 output port; received %i." % size(pout))

    blk = RCPblk('FH_5XXX_AD',[],pout,[0,0],0,[],[ID, ch])
    return blk

def FH_CO_MotXBlk(pin, ID, res):
    """

    Call:   FH_CO_MotXBlk(pin, ID, res)

    Parameters
    ----------
       pin: connected input port(s)
       ID : Device ID
       res : Resolution

    Returns
    -------
        blk  : RCPblk

    """
    
    if(size(pin) != 1):
        raise ValueError("Block should have 1 input port; received %i." % size(pout))

    blk = RCPblk('FH_canopen_motX',pin,[],[0,0],1,[res],[ID])
    return blk

def genericBlk(pin, pout, nx, uy, rP, iP, strP, fname):
    """

    Call: genericBlk()

    Parameters
    ----------
        pin     : connected input ports
        pout    : connected output ports
        nx      : states [cont, disc]
        uy      : Feedforw input->output
        rP      : real parameters
        iP:     : integer parameters
        strP:   : Block string
        fname   : filename (implementation file .c)

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk(fname,pin,pout,nx,uy,rP,iP, strP)
    return blk

def init_maxon_MotBlk(ID, propGain, intGain):
    """

    Call:   init_maxon_MotIBlk(ID, propGain, intGain)

    Parameters
    ----------
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
        blk  : RCPblk

    """
    
    blk = RCPblk('init_maxon_Mot',[],[],[0,0],0,[propGain, intGain],[ID])
    
    return blk

def init_epos_MotIBlk(ID, propGain, intGain, mode):
    """

    Call:   init_epos_MotIBlk(ID, propGain, intGain)

    Parameters
    ----------
       ID : Device ID
       propGain : Prop. gain
       intGain : Integ. gain

    Returns
    -------
        blk  : RCPblk

    """
    blk = RCPblk('init_epos_Mot',[],[],[0,0],0,[propGain, intGain],[ID, -1*mode])
    
    return blk

def can_sdo_sendThBlk(pin, ID, index, subindex, data, useInp):
    """

    Call:   can_sdo_sendThBlk(pin, pout, ID, index, subindex, data, useInp)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       ID : Device ID
       index : Index
       subindex : SubIndex
       data :  Data
       useInp :  Use Input [0/1 no/yes]

    Returns
    -------
        blk: RCPblk

    """
    
    blk = RCPblk('can_sdo_send', pin, [], [0,0], 1, [], [ID, index, subindex, data, useInp])
    return blk

def can_sdo_recvBlk(pout, ID, index, subindex, K):
    """

    Call:   can_sdo_recvBlk(pout, ID, index, subindex, K)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       index : Index
       subindex : SubIndex
       K : Conversion factor

    Returns
    -------
        blk  : RCPblk

    """
    
    retID = 0x580+ID
    blk = RCPblk('can_sdo_recv',[],pout,[0,0],0,[K],[ID, index, subindex, retID])
    return blk

def can_gen_recvBlk(pout, ID, retID, index, subindex, K):
    """

    Call:   can_gen_recvBlk(pout, ID, retID, index, subindex, K)

    Parameters
    ----------
       pout: connected output port(s)
       ID : Device ID
       retID : ReturnID
       index : Index
       subindex : SubIndex
       K : Conversion factor

    Returns
    -------
       blk  : RCPblk

    """
    
    blk = RCPblk('can_gen_recv',[],pout,[0,0],0,[K],[ID, index, subindex, retID])
    return blk

def scopeStream(pin):
    """Create an interactive scope."""
    blk = RCPblk("scope", pin, [], [0,0], 1, [], [])
    return blk

def loggerBlk(pin, log_file):
    """Wrtie pin to log_file."""
    blk = RCPblk("logger", pin, [], [0,0], 1, [], [], log_file)
    return blk

def comediPWMBlk(pin, devicename, channel, prescaler, pfiout):
    """Create Comedi_PWM block 

    Comedi_PWM
    
    Call:  ComediPWMBlk(pin, devicename, channel, prescaler, pfiout)

    Parameters
    ----------
    pin : connected input port
    devicename: Comedi device name /path
    channel : Channel
    prescaler: Clock Prescaler
    pfiout: Output pin

    Returns
    -------
    blk  : RCPblk
    """
    if size(pin)!=1:
        raise ValueError("Block should have only 1 input port, receivesd %i" % size(pin))
    
    blk = RCPblk("comedi_pwm_generator",pin,[],[0,0],1,[],[channel,prescaler,pfiout], devicename)
    return blk
    

def deadzoneBlk(pin, pout, start, end):
    """

    Call:   deadzoneBlk(pin, pout, start, end)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       start : Start
       end : End

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('deadzone', pin, pout, [0,0], 1, [start, end], [])
    return blk

def getTimerBlk(pout):
    """

    Call:   getTimerBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('getTimer', [], pout, [0,0], 0, [], [])
    return blk

def UDPsocketRxBlk(pout, IP, port):
    """

    Call:   UDPsocketRxBlk(pout, IP, port)

    Parameters
    ----------
       pout: connected output port(s)
       IP : IP Addr
       port :  Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('UDPsocketRx', [], pout, [0,0], 0, [], [port, 0], IP)
    return blk

def UDPsocketTxBlk(pin, IP, port):
    """

    Call:   UDPsocketTxBlk(pin, IP, port)

    Parameters
    ----------
       pin: connected input port(s)
       IP : IP Addr
       port :  Port

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('UDPsocketTx', pin, [], [0,0], 1, [], [port, 0], IP)
    return blk

def nullBlk(pin):
    """

    Call:   nullBlk(pin)

    Parameters
    ----------
       pin: connected input port(s)

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('toNull', pin, [], [0,0], 1, [], [])
    return blk
