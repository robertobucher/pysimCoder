# BlockEditor

This folder contains some utilities related to pysimCoder,

  * defBlocs.py is an application thar can be used to create new blocks for the editor
  * xblk2Blk.py transforms the new block into an element of the class "Block", included the skeleton of the C code for the "devices" folder

The folder contains also some utilities that allow to connect the generated code to external plot applications

  * RTScope.py which uses plots from PyQT qwt
  * qtRTScope.py which interfaces the generated code to pyqtgraph
  * PlotJugglerInterface.py which sends the data to PlotJuggler (**to be open as external application!**)

PlotJuggler can be simly installed using "snap".

The plot applications allow to save and reload custom configurations (saved as json files).
