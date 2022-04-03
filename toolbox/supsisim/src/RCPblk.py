"""
This is a procedural interface to the RCPblk library

roberto.bucher@supsi.ch

The following class is provided:

  RCPblk      - class with info for Rapid Controller Prototyping

"""
from numpy import array, ones

class RCPblk:
    def __init__(self, *args):  
        if len(args) == 8:
            (fcn,pin,pout,nx,uy,realPar,intPar,str) = args
        elif len(args) == 7:
            (fcn,pin,pout,nx,uy,realPar,intPar) = args
            str=''
        else:
            raise ValueError("Needs 6 or 7 arguments; received %i." % len(args))
        
        self.fcn = fcn
        self.pin = array(pin)
        self.pout = array(pout)
        self.dimPin = ones(self.pin.shape)
        self.dimPout = ones(self.pout.shape)
        self.nx = array(nx)
        self.uy = array(uy)
        self.realPar = array(realPar)
        self.realParNames = []
        self.intPar = array(intPar)
        self.intParNames = []
        self.str = str

    def __str__(self):
        """String representation of the Block"""
        str =  "Function           : " + self.fcn.__str__() + "\n"
        str += "Input ports        : " + self.pin.__str__() + "\n"
        str += "Output ports      : " + self.pout.__str__() + "\n"
        str += "Input dimensions : " + self.dimPin.__str__() + "\n"
        str += "Output dimensions : " + self.dimPout.__str__() + "\n"
        str += "Nr. of states      : " + self.nx.__str__() + "\n"
        str += "Relation u->y      : " + self.uy.__str__() + "\n"
        str += "Real parameters    : " + self.realPar.__str__() + "\n"
        str += "Names of real parameters : " + self.realParNames.__str__() + "\n"
        str += "Integer parameters : " + self.intPar.__str__() + "\n"
        str += "Names of integer parameters : " + self.intParNames.__str__() + "\n"
        str += "String Parameter   : " + self.str.__str__() + "\n"
        return str

