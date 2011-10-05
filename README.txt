#################################################################################################################
#														#
# Copyright (C) 2006-2009 ICAR-CNR & IBB-CNR									#
# Institute for High Performance Computing (ICAR) and Institute of Biostructure and Bioimaging (IBB) of the 	#
# Italian National Research Council (CNR)									#
# URL: http://amico.icar.cnr.it/mito.php									#
# License: GNU General Public License (GPL)									#
# e-mail: mito@na.icar.cnr.it										#
#														#
#################################################################################################################



				MITO - Medical Imaging TOolkit

MITO is an open-source, Windows-based software architecture for advanced Medical Imaging visualization. Main characteristics are: 
DICOM compliant, 2D-3D visualization (VR, SR, MIP), image segmentation and fusion, ROI, virtual reality.



###################################################################################################################


You may want alternatingly to install or build MITO in order to execute it.


INSTALLATION
============
Supported operating systems are 32-bits and 64-bits Microsoft Windows XP and Microsoft Windows Vista.
Unzip either mito_win32.zip or mito_x64.zip file according to your operating system.
Run setup.exe and follow the instructions presented on your screen. 



COMPILATION
===========
In order to compile correctly the solution, the following libraries are required:

- wxWidgets 2.8.10, available at http://www.wxwidgets.org/

- Insight Toolkit (ITK) 3.2.0, available at http://www.itk.org/

- Visualization Toolkit (VTK) 5.2.0, available at http://www.vtk.org/

- DICOM Toolkit (DCMTK) 3.5.4, available at http://dicom.offis.de/dcmtk

- OpenGL Utility Toolkit (GLUT) 3.7.6, available at http://www.opengl.org/resources/libraries/glut/

- IJG JPEG Library 6b, available at http://www.ijg.org/

- CxImage 6.00, availabe at http://www.xdp.it/cximage.htm

- Wiimote, available at the directory named 'libs' of this package



It is important that you overwrite some of VTK and CxImage files with the ones reported in the directory 'libs'.
Overwrite them to your libraries using the same directory structures. You have also to change the CMakeLists.txt
file under directory 'Rendering' of your VTK library before compiling VTK library by adding the following files
under the statement 'SET( Kit_SRCS':

 vtkInteractorStyleWII.cxx
 vtkInteractorStyleWIITrackball.cxx


Then, you have to compile all the libraries reported above (except 'Wiimote').

Finally, you need to change library paths in the CMakeLists.txt file in order to correctly create a Microsoft Visual
Studio.NET 2003 or Microsoft Visual Studio 2005 solution by means of the free tool CMake 2.6, available at http://www.cmake.org/.
Move the directory 'Wiimote' under directory 'libs' wherever you want and change correctly its path in the CMakeLists.txt file.

Now you can build the Microsoft Visual Studio solution in order to create the executable file named MITO.exe.