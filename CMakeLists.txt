# first line
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(MITO)

# --------------------------------------------------------------------------

find_package(VTK REQUIRED)
find_package(ITK REQUIRED)
#find_package(DCMTK COMPONENTS config ofstd dcmdata dcmnet dcmjpeg dcmimgle dcmimage REQUIRED)
find_package(wxWidgets COMPONENTS core base net xml xrc adv REQUIRED)

include(${VTK_USE_FILE})
include(${ITK_USE_FILE})
include(${wxWidgets_USE_FILE})

#INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIRS})

#DCMTK
SET(DCMTK_DIR "C:/Libs/dcmtk-3.5.4")
INCLUDE_DIRECTORIES(${DCMTK_DIR}/config/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/ofstd/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmnet/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmdata/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmjpeg/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmimgle/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmimage/include)

# Glut compiler setting
SET(GLUT_DIR "C:/Libs/glut")
INCLUDE_DIRECTORIES(${GLUT_DIR})

# JpegLib compiler setting
SET(JPEGLIB_INCLUDE_DIR "C:/Libs/jpeg-6b-4-lib/include")
SET(JPEGLIB_LIB_DIR "C:/Libs/jpeg-6b-4-lib/lib")
INCLUDE_DIRECTORIES(${JPEGLIB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${JPEGLIB_LIB_DIR})

# CxImage compiler setting
SET(CXIMAGE_INCLUDE_DIR "C:/Libs/cximage701_full")
SET(CXIMAGE_LIB_DIR "C:/Libs/cximage701_full")
INCLUDE_DIRECTORIES(${CXIMAGE_LIB_DIR}/jpeg)
INCLUDE_DIRECTORIES(${CXIMAGE_LIB_DIR}/CxImage)

# VS2008 + WinDDK bug fix
INCLUDE_DIRECTORIES("$(VCInstallDir)include")

# Wiimote compiler setting
#SET(WDK_LIB_DIR "C:/Libs/WinDDK/7600.16385.0/lib/win7/i386") # 32 Bit
SET(WDK_LIB_DIR "C:/Libs/WinDDK/7600.16385.0/lib/win7/amd64") # 64 Bit
SET(WDK_INC_DIR "C:/Libs/WinDDK/7600.16385.0/inc/api")
INCLUDE_DIRECTORIES(${WDK_INC_DIR})

# wbemuuid compiler setting
#SET(WBEMUUID_LIB_DIR "C:/Libs/WinDDK/7600.16385.0/lib/wnet/i386") # 32 Bit
SET(WBEMUUID_LIB_DIR "C:/Libs/WinDDK/7600.16385.0/lib/wnet/amd64") # 64 Bit

# DIRECTX SDK
#SET(DIRECTX_LIBRARY_DIR "C:/Program Files (x86)/Microsoft DirectX SDK (August 2009)/Lib/x86") # 32 Bit
SET(DIRECTX_LIBRARY_DIR "C:/Program Files (x86)/Microsoft DirectX SDK (August 2009)/Lib/x64") # 64 Bit

# --------------------------------------------------------------------------
# Sources

  set(MITO_SRCS

	src/MITO.rc
	src/anonymizeFilter.cxx
	src/appData.cxx
	src/appWxVtkInteractor.cxx
	src/dataHandler.cxx
	src/exportFilter.cxx
	src/flipFilter.cxx
	src/fusionFilter.cxx
	src/imageDrawROI.cxx
	src/imageProc.cxx
	src/infoFilter.cxx
	src/itkVtkData.cxx
	src/itkVtkFilter.cxx
	src/mipFilter.cxx
	src/mitoLocalDatabase.cxx
	src/wxMotionFactorDialog.cxx
	src/pacsCommunicator.cxx
	src/pacsDataTypes.cxx
	src/pacsPreferences.cxx
	src/pacsQuery.cxx
	src/pacsQueryWorkerThread.cxx
	src/pacsStoreSCP.cxx
	src/pacsStoreSCU.cxx
	src/roiManager.cxx
	src/segmentationFilter.cxx
	src/segmentationManager.cxx
	src/surfaceRenderingFilter.cxx
	src/treeListCtrl.cxx
	src/viewer.cxx
 	src/viewer2d.cxx
	src/viewer3d.cxx
 	src/viewerDrawing.cxx
 	src/viewerHandler.cxx
 	src/volumeRenderingFilter.cxx
 	src/wx2dGui.cxx
 	src/wxAnonymizeDialog.cxx
 	src/wxConfidenceConnectedDialog.cxx
 	src/wxConnectedThresholdDialog.cxx
 	src/wxEventCatcher.cxx
 	src/wxFusionDialog.cxx
 	src/wxJPGHandler.cxx
 	src/wxMainGui.cxx
 	src/wxMITOApp.cxx
 	src/wxNeighborhoodConnectedDialog.cxx
 	src/wxPacsListBox.cxx
 	src/wxPacsPreferencesGui.cxx
 	src/wxPacsQueryGuiBase.cxx
 	src/wxPacsQueryGui.cxx
 	src/wxPacsStoreGui.cxx
 	src/wxQueryGuiStatusBar.cxx
 	src/wxSurfaceRenderingGui.cxx
 	src/wxThumb.cxx
 	src/wxThumbnailFactory.cxx
 	src/wxVolumeRenderingGui.cxx
 	src/wxVTKRenderWindowInteractor.cxx
 	src/wxVtkViewer.cxx
 	src/wxVtkViewer2d.cxx
 	src/wxVtkViewer3d.cxx
 	src/wxWLWWDialog.cxx
 	src/wxWiiManager.cxx
 	src/wxCDRatioDialog.cxx
 	src/wxWiiManagerDialog.cxx
 	src/wxWiimoteEvent.cxx
 	src/wxWiiDriver.cxx
 	src/HIDdevice.cxx
	src/wxShadingDialog.cxx
	src/vtkInteractorStyleWII.cxx
	src/vtkInteractorStyleWIITrackball.cxx
	src/vtkMitoCustomCamera.cxx
	src/vtkBoxWidgetWii.cxx
  )
  
  set(MITO_ADDITIONAL_HEADERS

	src/anonymizeFilter.h
	src/appData.h
	src/appWxVtkInteractor.h
	src/colorTransferTable.h
	src/dataHandler.h
	src/exportFilter.h
	src/flipFilter.h
	src/fusionFilter.h
	src/globalState.h
	src/imageDrawROI.h
	src/imageProc.h
	src/infoFilter.h
	src/itkDaub.h
	src/itkFlip.h
	src/itkFusion.h
	src/itkFusion2.h
	src/itkFusion3.h
	src/itkImageToVTKImageFilter.h
	src/itkImageToVTKImageFilter.txx
	src/itkVtkData.h
	src/itkVtkFilter.h
	src/itkWavelet.h
	src/mipFilter.h
	src/mitoLocalDatabase.h
	src/wxMotionFactorDialog.h
	src/pacsCommunicator.h
	src/pacsDataTypes.h
	src/pacsPreferences.h
	src/pacsQuery.h
	src/pacsQueryWorkerThread.h
	src/pacsStoreSCP.h
	src/pacsStoreSCU.h
	src/point.h
	src/roiMacro.h
	src/roiManager.h
	src/segmentationFilter.h
	src/segmentationManager.h
	src/surfaceRenderingFilter.h
	src/thumb_md5.h
	src/treeListCtrl.h
	src/viewer.h
 	src/viewer2d.h
	src/viewer3d.h
 	src/viewerDrawing.h
 	src/viewerHandler.h
 	src/volumeRenderingFilter.h
 	src/wx2dGui.h
 	src/wxAnonymizeDialog.h
 	src/wxConfidenceConnectedDialog.h
 	src/wxConnectedThresholdDialog.h
 	src/wxEventCatcher.h
 	src/wxJPGHandler.h
 	src/wxFusionDialog.h
 	src/wxMainGui.h
 	src/wxMITOApp.h
 	src/wxNeighborhoodConnectedDialog.h
 	src/wxPacsListBox.h
 	src/wxPacsPreferencesGui.h
	src/wxPacsQueryGuiBase.h
 	src/wxPacsQueryGui.h
 	src/wxPacsStoreGui.h
 	src/wxQueryGuiStatusBar.h
 	src/wxSurfaceRenderingGui.h
 	src/wxThumb.h
 	src/wxThumbnailFactory.h
 	src/wxVolumeRenderingGui.h
 	src/wxVTKRenderWindowInteractor.h
 	src/wxVtkViewer.h
 	src/wxVtkViewer2d.h
 	src/wxVtkViewer3d.h
 	src/wxWLWWDialog.h
 	src/wxWiiManager.h
 	src/wxCDRatioDialog.h
 	src/wxWiiDriver.h
 	src/wxWiiManagerDialog.h
 	src/wxWiimoteEvent.h
 	src/HIDdevice.h
	src/wxShadingDialog.h
	src/vtkInteractorStyleWII.h
	src/vtkInteractorStyleWIITrackball.h
	src/vtkMitoCustomCamera.h
	src/vtkBoxWidgetWii.h
  )

add_executable(MITO WIN32 ${MITO_SRCS} ${MITO_ADDITIONAL_HEADERS})

# --------------------------------------------------------------------------
#linker

#VTK

target_link_libraries(MITO Cosmo)
target_link_libraries(MITO MapReduceMPI )
target_link_libraries(MITO mpistubs )
target_link_libraries(MITO VPIC )
target_link_libraries(MITO vtkalglib )
target_link_libraries(MITO vtkCharts )
target_link_libraries(MITO vtkCommon )
target_link_libraries(MITO vtkDICOMParser )
target_link_libraries(MITO vtkexoIIc )
target_link_libraries(MITO vtkexpat )
target_link_libraries(MITO vtkFiltering )
target_link_libraries(MITO vtkfreetype )
target_link_libraries(MITO vtkftgl )
target_link_libraries(MITO vtkGenericFiltering )
target_link_libraries(MITO vtkGeovis )
target_link_libraries(MITO vtkGraphics )
target_link_libraries(MITO vtkHybrid )
target_link_libraries(MITO vtkImaging )
target_link_libraries(MITO vtkInfovis )
target_link_libraries(MITO vtkIO )
target_link_libraries(MITO vtkjpeg )
target_link_libraries(MITO vtklibxml2 )
target_link_libraries(MITO vtkmetaio )
target_link_libraries(MITO vtkNetCDF )
target_link_libraries(MITO vtkNetCDF_cxx )
target_link_libraries(MITO vtkParallel )
target_link_libraries(MITO vtkpng )
target_link_libraries(MITO vtkproj4 )
target_link_libraries(MITO vtkRendering )
target_link_libraries(MITO vtksqlite )
target_link_libraries(MITO vtksys )
target_link_libraries(MITO vtktiff )
target_link_libraries(MITO vtkverdict )
target_link_libraries(MITO vtkViews )
target_link_libraries(MITO vtkVolumeRendering )
target_link_libraries(MITO vtkWidgets )
target_link_libraries(MITO vtkzlib )

#ITK
target_link_libraries(MITO ITKAlgorithms)
target_link_libraries(MITO ITKBasicFilters)
target_link_libraries(MITO ITKCommon)
target_link_libraries(MITO ITKDICOMParser)
target_link_libraries(MITO ITKEXPAT)
target_link_libraries(MITO ITKFEM)
target_link_libraries(MITO itkgdcm)
target_link_libraries(MITO ITKIO)
target_link_libraries(MITO itkjpeg12)
target_link_libraries(MITO itkjpeg16)
target_link_libraries(MITO itkjpeg8)
target_link_libraries(MITO ITKMetaIO)
target_link_libraries(MITO itkNetlibSlatec)
target_link_libraries(MITO ITKniftiio)
target_link_libraries(MITO ITKNrrdIO)
target_link_libraries(MITO ITKNumerics)
target_link_libraries(MITO itkopenjpeg)
target_link_libraries(MITO itkpng)
target_link_libraries(MITO ITKSpatialObject)
target_link_libraries(MITO ITKStatistics)
target_link_libraries(MITO itksys)
target_link_libraries(MITO itkTestDriver)
target_link_libraries(MITO itktestlib)
target_link_libraries(MITO itktiff)
target_link_libraries(MITO itkv3p_netlib)
target_link_libraries(MITO itkvcl)
target_link_libraries(MITO itkvnl)
target_link_libraries(MITO itkvnl_algo)
target_link_libraries(MITO itkvnl_inst)
target_link_libraries(MITO itkzlib)
target_link_libraries(MITO ITKznz)

IF (WIN32)
	#TARGET_LINK_LIBRARIES(MITO comctl32.lib)
	#TARGET_LINK_LIBRARIES(MITO odbc32.lib)
	#TARGET_LINK_LIBRARIES(MITO odbccp32.lib)
	#TARGET_LINK_LIBRARIES(MITO rpcrt4.lib)
	TARGET_LINK_LIBRARIES(MITO netapi32.lib)
	#TARGET_LINK_LIBRARIES(MITO wsock32.lib)
	#TARGET_LINK_LIBRARIES(MITO snmpapi.lib)
	#SET(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS")
ENDIF (WIN32)

SET(RUN_MODE "debug")

# wxWidgets
TARGET_LINK_LIBRARIES(MITO ${wxWidgets_LIBRARIES} )

#GLUT
TARGET_LINK_LIBRARIES(MITO ${GLUT_DIR}/glut32.lib)

# JPEG
TARGET_LINK_LIBRARIES(MITO ${JPEGLIB_LIB_DIR}/jpeg.lib)

# Wiimote linker settings
TARGET_LINK_LIBRARIES(MITO ${WDK_LIB_DIR}/hid.lib)
TARGET_LINK_LIBRARIES(MITO ${WDK_LIB_DIR}/setupapi.lib)

# WBEMUUID
TARGET_LINK_LIBRARIES(MITO ${WBEMUUID_LIB_DIR}/wbemuuid.lib)

# DIRECTX SDK
TARGET_LINK_LIBRARIES(MITO ${DIRECTX_LIBRARY_DIR}/d3d9.lib)

#DCMTK
#TARGET_LINK_LIBRARIES(MITO ${DCMTK_LIBRARIES} )

#DCMTK
TARGET_LINK_LIBRARIES(MITO ${DCMTK_DIR}/bin/ofstd/libsrc/${RUN_MODE}/ofstd.lib)
TARGET_LINK_LIBRARIES(MITO ${DCMTK_DIR}/bin/dcmdata/libsrc/${RUN_MODE}/dcmdata.lib)
TARGET_LINK_LIBRARIES(MITO ${DCMTK_DIR}/bin/dcmnet/libsrc/${RUN_MODE}/dcmnet.lib)
TARGET_LINK_LIBRARIES(MITO ${DCMTK_DIR}/bin/dcmjpeg/libsrc/${RUN_MODE}/dcmjpeg.lib)
TARGET_LINK_LIBRARIES(MITO ${DCMTK_DIR}/bin/dcmimgle/libsrc/${RUN_MODE}/dcmimgle.lib)
TARGET_LINK_LIBRARIES(MITO ${DCMTK_DIR}/bin/dcmimage/libsrc/${RUN_MODE}/dcmimage.lib)

# CXIMAGE
TARGET_LINK_LIBRARIES(MITO ${CXIMAGE_LIB_DIR}/CxImage/${RUN_MODE}/cximage.lib)
TARGET_LINK_LIBRARIES(MITO ${CXIMAGE_LIB_DIR}/jpeg/${RUN_MODE}/Jpeg.lib)
TARGET_LINK_LIBRARIES(MITO ${CXIMAGE_LIB_DIR}/mng/${RUN_MODE}/mng.lib)
TARGET_LINK_LIBRARIES(MITO ${CXIMAGE_LIB_DIR}/raw/${RUN_MODE}/libdcr.lib)
TARGET_LINK_LIBRARIES(MITO ${CXIMAGE_LIB_DIR}/jasper/${RUN_MODE}/jasper.lib)

# last line