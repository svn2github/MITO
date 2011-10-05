# CMAKE general settings
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# project settings
PROJECT(MITO)

SET(PRJ_NAME "MITO")
SET(PRJ_SRCS_DIR "src")
SET(EXECUTABLE_OUTPUT_PATH "bin")

SET(PRJ_SRCS
${PRJ_SRCS_DIR}/MITO.rc
${PRJ_SRCS_DIR}/anonymizeFilter.cxx
${PRJ_SRCS_DIR}/anonymizeFilter.h
${PRJ_SRCS_DIR}/appData.cxx
${PRJ_SRCS_DIR}/appData.h
${PRJ_SRCS_DIR}/appWxVtkInteractor.cxx
${PRJ_SRCS_DIR}/appWxVtkInteractor.h
${PRJ_SRCS_DIR}/colorTransferTable.h
${PRJ_SRCS_DIR}/dataHandler.cxx
${PRJ_SRCS_DIR}/dataHandler.h
${PRJ_SRCS_DIR}/exportFilter.cxx
${PRJ_SRCS_DIR}/exportFilter.h
${PRJ_SRCS_DIR}/flipFilter.cxx
${PRJ_SRCS_DIR}/flipFilter.h
${PRJ_SRCS_DIR}/fusionFilter.cxx
${PRJ_SRCS_DIR}/fusionFilter.h
${PRJ_SRCS_DIR}/globalState.h
${PRJ_SRCS_DIR}/imageDrawROI.cxx
${PRJ_SRCS_DIR}/imageDrawROI.h
${PRJ_SRCS_DIR}/imageProc.cxx
${PRJ_SRCS_DIR}/imageProc.h
${PRJ_SRCS_DIR}/infoFilter.cxx
${PRJ_SRCS_DIR}/infoFilter.h
${PRJ_SRCS_DIR}/itkDaub.h
${PRJ_SRCS_DIR}/itkFlip.h
${PRJ_SRCS_DIR}/itkFusion.h
${PRJ_SRCS_DIR}/itkFusion2.h
${PRJ_SRCS_DIR}/itkFusion3.h
${PRJ_SRCS_DIR}/itkImageToVTKImageFilter.h
${PRJ_SRCS_DIR}/itkImageToVTKImageFilter.txx
${PRJ_SRCS_DIR}/itkVtkData.cxx
${PRJ_SRCS_DIR}/itkVtkData.h
${PRJ_SRCS_DIR}/itkVtkFilter.cxx
${PRJ_SRCS_DIR}/itkVtkFilter.h
${PRJ_SRCS_DIR}/itkWavelet.h
${PRJ_SRCS_DIR}/mipFilter.cxx
${PRJ_SRCS_DIR}/mipFilter.h
${PRJ_SRCS_DIR}/mitoLocalDatabase.cxx
${PRJ_SRCS_DIR}/mitoLocalDatabase.h
${PRJ_SRCS_DIR}/pacsCommunicator.cxx
${PRJ_SRCS_DIR}/pacsCommunicator.h
${PRJ_SRCS_DIR}/pacsDataTypes.cxx
${PRJ_SRCS_DIR}/pacsDataTypes.h
${PRJ_SRCS_DIR}/pacsPreferences.cxx
${PRJ_SRCS_DIR}/pacsPreferences.h
${PRJ_SRCS_DIR}/pacsQuery.cxx
${PRJ_SRCS_DIR}/pacsQuery.h
${PRJ_SRCS_DIR}/pacsQueryWorkerThread.cxx
${PRJ_SRCS_DIR}/pacsQueryWorkerThread.h
${PRJ_SRCS_DIR}/pacsStoreSCP.cxx
${PRJ_SRCS_DIR}/pacsStoreSCP.h
${PRJ_SRCS_DIR}/pacsStoreSCU.cxx
${PRJ_SRCS_DIR}/pacsStoreSCU.h
${PRJ_SRCS_DIR}/point.h
${PRJ_SRCS_DIR}/roiMacro.h
${PRJ_SRCS_DIR}/roiManager.cxx
${PRJ_SRCS_DIR}/roiManager.h
${PRJ_SRCS_DIR}/segmentationFilter.cxx
${PRJ_SRCS_DIR}/segmentationFilter.h
${PRJ_SRCS_DIR}/segmentationManager.cxx
${PRJ_SRCS_DIR}/segmentationManager.h
${PRJ_SRCS_DIR}/surfaceRenderingFilter.cxx
${PRJ_SRCS_DIR}/surfaceRenderingFilter.h
${PRJ_SRCS_DIR}/thumb_md5.h
${PRJ_SRCS_DIR}/treeListCtrl.cxx
${PRJ_SRCS_DIR}/treeListCtrl.h
${PRJ_SRCS_DIR}/viewer.cxx
${PRJ_SRCS_DIR}/viewer.h
${PRJ_SRCS_DIR}/viewer2d.cxx
${PRJ_SRCS_DIR}/viewer2d.h
${PRJ_SRCS_DIR}/viewer3d.cxx
${PRJ_SRCS_DIR}/viewer3d.h
${PRJ_SRCS_DIR}/viewerDrawing.cxx
${PRJ_SRCS_DIR}/viewerDrawing.h
${PRJ_SRCS_DIR}/viewerHandler.cxx
${PRJ_SRCS_DIR}/viewerHandler.h
${PRJ_SRCS_DIR}/volumeRenderingFilter.cxx
${PRJ_SRCS_DIR}/volumeRenderingFilter.h
${PRJ_SRCS_DIR}/wx2dGui.cxx
${PRJ_SRCS_DIR}/wx2dGui.h
${PRJ_SRCS_DIR}/wxAnonymizeDialog.cxx
${PRJ_SRCS_DIR}/wxAnonymizeDialog.h
${PRJ_SRCS_DIR}/wxCDRatioDialog.cxx
${PRJ_SRCS_DIR}/wxCDRatioDialog.h
${PRJ_SRCS_DIR}/wxConfidenceConnectedDialog.cxx
${PRJ_SRCS_DIR}/wxConfidenceConnectedDialog.h
${PRJ_SRCS_DIR}/wxConnectedThresholdDialog.cxx
${PRJ_SRCS_DIR}/wxConnectedThresholdDialog.h
${PRJ_SRCS_DIR}/wxEventCatcher.cxx
${PRJ_SRCS_DIR}/wxEventCatcher.h
${PRJ_SRCS_DIR}/wxFusionDialog.cxx
${PRJ_SRCS_DIR}/wxFusionDialog.h
${PRJ_SRCS_DIR}/wxJPGHandler.cxx
${PRJ_SRCS_DIR}/wxJPGHandler.h
${PRJ_SRCS_DIR}/wxMainGui.cxx
${PRJ_SRCS_DIR}/wxMainGui.h
${PRJ_SRCS_DIR}/wxMITOApp.cxx
${PRJ_SRCS_DIR}/wxMITOApp.h
${PRJ_SRCS_DIR}/wxMotionFactorDialog.cxx
${PRJ_SRCS_DIR}/wxMotionFactorDialog.h
${PRJ_SRCS_DIR}/wxNeighborhoodConnectedDialog.cxx
${PRJ_SRCS_DIR}/wxNeighborhoodConnectedDialog.h
${PRJ_SRCS_DIR}/wxPacsListBox.cxx
${PRJ_SRCS_DIR}/wxPacsListBox.h
${PRJ_SRCS_DIR}/wxPacsPreferencesGui.cxx
${PRJ_SRCS_DIR}/wxPacsPreferencesGui.h
${PRJ_SRCS_DIR}/wxPacsQueryGuiBase.cxx
${PRJ_SRCS_DIR}/wxPacsQueryGuiBase.h
${PRJ_SRCS_DIR}/wxPacsQueryGui.cxx
${PRJ_SRCS_DIR}/wxPacsQueryGui.h
${PRJ_SRCS_DIR}/wxPacsStoreGui.cxx
${PRJ_SRCS_DIR}/wxPacsStoreGui.h
${PRJ_SRCS_DIR}/wxQueryGuiStatusBar.cxx
${PRJ_SRCS_DIR}/wxQueryGuiStatusBar.h
${PRJ_SRCS_DIR}/wxSurfaceRenderingGui.cxx
${PRJ_SRCS_DIR}/wxSurfaceRenderingGui.h
${PRJ_SRCS_DIR}/wxThumb.cxx
${PRJ_SRCS_DIR}/wxThumb.h
${PRJ_SRCS_DIR}/wxThumbnailFactory.cxx
${PRJ_SRCS_DIR}/wxThumbnailFactory.h
${PRJ_SRCS_DIR}/wxVolumeRenderingGui.cxx
${PRJ_SRCS_DIR}/wxVolumeRenderingGui.h
${PRJ_SRCS_DIR}/wxVTKRenderWindowInteractor.cxx
${PRJ_SRCS_DIR}/wxVTKRenderWindowInteractor.h
${PRJ_SRCS_DIR}/wxVtkViewer.cxx
${PRJ_SRCS_DIR}/wxVtkViewer.h
${PRJ_SRCS_DIR}/wxVtkViewer2d.cxx
${PRJ_SRCS_DIR}/wxVtkViewer2d.h
${PRJ_SRCS_DIR}/wxVtkViewer3d.cxx
${PRJ_SRCS_DIR}/wxVtkViewer3d.h
${PRJ_SRCS_DIR}/wxWLWWDialog.cxx
${PRJ_SRCS_DIR}/wxWLWWDialog.h
${PRJ_SRCS_DIR}/wxWiiManager.cxx
${PRJ_SRCS_DIR}/wxWiiManager.h
${PRJ_SRCS_DIR}/wxWiiManagerDialog.cxx
${PRJ_SRCS_DIR}/wxWiiManagerDialog.h
${PRJ_SRCS_DIR}/wxWiimoteEvent.cxx
${PRJ_SRCS_DIR}/wxWiimoteEvent.h
${PRJ_SRCS_DIR}/wxWiiDriver.cxx
${PRJ_SRCS_DIR}/wxWiiDriver.h
${PRJ_SRCS_DIR}/HIDdevice.cxx
${PRJ_SRCS_DIR}/HIDdevice.h
)

SET(RUN_MODE "release")

# wxWindows compiler settings
SET(WXWINDOWS_INCLUDE_DIR "path/to/wxWidgets-2.8.10/include")
SET(WXWINDOWS_LIB_DIR "path/to/lib/wxWidgets-2.8.10/lib/vc_lib")
INCLUDE_DIRECTORIES(${WXWINDOWS_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${WXWINDOWS_INCLUDE_DIR}/msvc)

# ITK compiler settings
SET(ITK_DIR "path/to/lib/InsightToolkit-3.2.0")
INCLUDE (${CMAKE_ROOT}/Modules/FindITK.cmake)
IF (USE_ITK_FILE)
  INCLUDE(${USE_ITK_FILE})
ENDIF (USE_ITK_FILE)

# VTK compiler settings
SET(VTK_DIR "path/to/lib/vtk-5.2.0/bin")
INCLUDE (${CMAKE_ROOT}/Modules/FindVTK.cmake)
IF (USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
ENDIF (USE_VTK_FILE)

# DCMTK
SET(DCMTK_DIR "path/to/lib/dcmtk-3.5.4")
INCLUDE_DIRECTORIES(${DCMTK_DIR}/config/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/ofstd/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmnet/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmdata/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmjpeg/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmimgle/include)
INCLUDE_DIRECTORIES(${DCMTK_DIR}/dcmimage/include)

# Glut compiler setting
SET(GLUT_DIR "path/to/lib/glut-3.7.6")
INCLUDE_DIRECTORIES(${GLUT_DIR})

# JpegLib compiler setting
SET(JPEGLIB_INCLUDE_DIR "path/to/lib/Jpeg/include")
SET(JPEGLIB_LIB_DIR "path/to/lib/Jpeg/lib")
INCLUDE_DIRECTORIES(${JPEGLIB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${JPEGLIB_LIB_DIR})

# CxImage compiler setting
SET(CXIMAGE_INCLUDE_DIR "path/to/lib/CxImage-6.00")
SET(CXIMAGE_LIB_DIR "path/to/lib/CxImage-6.00")
INCLUDE_DIRECTORIES(${CXIMAGE_LIB_DIR}/jpeg)
INCLUDE_DIRECTORIES(${CXIMAGE_LIB_DIR}/CxImage)

# Wiimote compiler setting
SET(WIIMOTE_DIR "path/to/lib/Wiimote")
INCLUDE_DIRECTORIES(${WIIMOTE_DIR}/include)

# project settings
SET(OUT_EXE ${PRJ_NAME})
ADD_EXECUTABLE(${PRJ_NAME} WIN32 ${PRJ_SRCS})

#SET(CMAKE_EXE_LINKER_FLAGS "${WXWINDOWS_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
IF (${RUN_MODE} STREQUAL release)
	# wxWindows linker settings
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28_net.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28_odbc.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28_xml.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxexpat.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxjpeg.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_adv.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_aui.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_core.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_dbgrid.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_gl.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_html.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_media.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_qa.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_richtext.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28_xrc.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxpng.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxregex.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxtiff.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxzlib.lib)
ENDIF (${RUN_MODE} STREQUAL release)

IF (${RUN_MODE} STREQUAL debug)
	# wxWindows linker settings
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28d.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28d_net.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28d_odbc.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxbase28d_xml.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxexpatd.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxjpegd.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_adv.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_aui.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_richtext.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_core.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_dbgrid.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_gl.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_html.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_media.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_qa.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_richtext.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxmsw28d_xrc.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxpngd.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxregexd.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxtiffd.lib)
	TARGET_LINK_LIBRARIES(${OUT_EXE} ${WXWINDOWS_LIB_DIR}/wxzlibd.lib)
ENDIF (${RUN_MODE} STREQUAL debug)

IF (WIN32)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} comctl32.lib)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} odbc32.lib)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} odbccp32.lib)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} rpcrt4.lib)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} netapi32.lib)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} wsock32.lib)
	TARGET_LINK_LIBRARIES(${PRJ_NAME} snmpapi.lib)
	SET(CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS")
ENDIF (WIN32)

# ITK linker settings
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKAlgorithms.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKBasicFilters.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKCommon.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKDICOMParser.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKEXPAT.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKFEM.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkgdcm.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKIO.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkjpeg8.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkjpeg12.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkjpeg16.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKMetaIO.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKniftiio.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKNrrdIO.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKNumerics.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkopenjpeg.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkpng.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKSpatialObject.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKStatistics.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itksys.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itktestlib.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itktiff.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkv3p_netlib.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkvcl.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkvnl.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkvnl_algo.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkvnl_inst.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/itkzlib.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${ITK_DIR}/bin/${RUN_MODE}/ITKznz.lib)

# VTK linker settings
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkCommon.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkDICOMParser.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkexoIIc.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkexpat.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkFiltering.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkfreetype.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkftgl.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkGenericFiltering.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkGraphics.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkHybrid.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkImaging.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkIO.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkjpeg.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkNetCDF.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkParallel.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkpng.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkRendering.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtksys.lib)
#TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtktiff.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkVolumeRendering.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkWidgets.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${VTK_DIR}/bin/${RUN_MODE}/vtkzlib.lib)

# DCMTK linker settings
TARGET_LINK_LIBRARIES(${OUT_EXE} ${DCMTK_DIR}/bin/ofstd/libsrc/${RUN_MODE}/ofstd.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${DCMTK_DIR}/bin/dcmdata/libsrc/${RUN_MODE}/dcmdata.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${DCMTK_DIR}/bin/dcmnet/libsrc/${RUN_MODE}/dcmnet.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${DCMTK_DIR}/bin/dcmjpeg/libsrc/${RUN_MODE}/dcmjpeg.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${DCMTK_DIR}/bin/dcmimgle/libsrc/${RUN_MODE}/dcmimgle.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${DCMTK_DIR}/bin/dcmimage/libsrc/${RUN_MODE}/dcmimage.lib)

# Glut linker settings
#TARGET_LINK_LIBRARIES(${OUT_EXE} ${GLUT_DIR}/glut)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${GLUT_DIR}/glut32.lib)

# JpegLib linker settings
TARGET_LINK_LIBRARIES(${OUT_EXE} ${JPEGLIB_LIB_DIR}/jpeg.lib)

# CxImage linker settings
TARGET_LINK_LIBRARIES(${OUT_EXE} ${CXIMAGE_LIB_DIR}/CxImage/Debug/cximage.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${CXIMAGE_LIB_DIR}/jpeg/Debug/Jpeg.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${CXIMAGE_LIB_DIR}/mng/Debug/mng.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${CXIMAGE_LIB_DIR}/raw/Debug/libdcr.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${CXIMAGE_LIB_DIR}/jasper/Debug/jasper.lib)

# Wiimote linker settings
TARGET_LINK_LIBRARIES(${OUT_EXE} ${WIIMOTE_DIR}/lib/hid.lib)
TARGET_LINK_LIBRARIES(${OUT_EXE} ${WIIMOTE_DIR}/lib/setupapi.lib)