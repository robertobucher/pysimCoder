import subprocess

def plotDlg(nin, nout, pars, name):
    fn = '/tmp/'+name
    cmd = 'dataplot ' + fn
    try:
        subprocess.Popen(cmd, shell=True)
    except:
        pass
    return pars

