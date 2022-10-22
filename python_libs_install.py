import sys
import subprocess

# assumes you got pip installed for the same python version you have 
# want to use for the tool chain

subprocess.call([sys.executable, "-m", "pip", "install", "-r requirements.txt"])
