/**
 * \file roiManager.cxx
 * \brief Gestione ROI
 * \author ICAR-CNR Napoli
 */
#include "roiManager.h"
#include "wx2dGui.h"


// Costruttore 
roiManager::roiManager(itkVtkData* itkVtkData) {
	_pixelMin = itkVtkData->getMinPixelValue();
	_vtkImageRoi = vtkImageData::New();
	_rgb = itkVtkData->getRgb();
	int *outExt;
	if(!_rgb) {
			_itkImageExtraction = itkVtkData->getItkImage();
			_appCom = vtkImageAppendComponents::New();
			vtkImageShiftScale* scale = vtkImageShiftScale::New();
			scale->SetOutputScalarTypeToLong();
			scale->SetShift(0);
			scale->SetScale(1);
			scale->SetInput(itkVtkData->getVtkImage());
			scale->ReleaseDataFlagOn();
			_appCom->AddInput(0, scale->GetOutput());
			_appCom->AddInput(0, scale->GetOutput());
			_appCom->AddInput(0, scale->GetOutput());
			_appCom->Update();
			_vtkImageRoi->DeepCopy(_appCom->GetOutput());
			scale->Delete();
			outExt = _vtkImageRoi->GetWholeExtent();
			_spacing = _itkImageExtraction->GetSpacing();
			_size = _itkImageExtraction->GetLargestPossibleRegion().GetSize();
	}
	else {
		_itkRgbImageExtraction = itkVtkData->getItkRgbImage();
		_appCom =vtkImageAppendComponents::New();
		vtkImageShiftScale* scale = vtkImageShiftScale::New();
		scale->SetOutputScalarTypeToLong();
		scale->SetShift(0);
		scale->SetScale(1);
		scale->SetInput(itkVtkData->getVtkImage());
		_appCom->AddInput(0, scale->GetOutput());
		_appCom->Update();
		_vtkImageRoi->DeepCopy(_appCom->GetOutput());
		scale->Delete();
		outExt = _vtkImageRoi->GetWholeExtent();
		_spacing = _itkRgbImageExtraction->GetSpacing();
		_size = _itkRgbImageExtraction->GetLargestPossibleRegion().GetSize();
	}
	_vtkExtract = _vtkImageRoi;
	_vtk = vtkImageData::New();
	
	_drroi = new imageDrawRoi();
	_drroi->Set_pixelSpacingX(_spacing[0]);
	_drroi->setMinMax(outExt);
	_drroi->Set_pixelSpacingY(_spacing[1]);
	_drroi->Set_sliceThickness(_spacing[2]);
	
	
	_state = INACTIVE_STATE;
	_typeExtraction = 0;
	_sliceExtraction = 0;
	_intervalMax = _size[2];
	_intervalMin = 0;
	_currentSlice = 0;
	_horFlip = false;
	_verFlip = false;
}

// Distruttore 
roiManager::~roiManager()
{
	_drroi->Delete();
	_vtkImageRoi->Delete();
	_vtk->Delete();
	if(_appCom) _appCom->Delete();
}

//Determina se un punto è interno alla ROI
int roiManager::cnInclusion(float p[])
{
	vtkPoints* poly = _drroi->getPoints();
	double p0[3];
	double p1[3];
    int n = poly->GetNumberOfPoints();
	int cn, i; 
	cn = 0;
    for (i = 0; i < n; i++)
    {
		poly->GetPoint(i, p0);
		if(i == n-1)
		{
			poly->GetPoint(0, p1);
		}
		else 
		{		
			poly->GetPoint(i+1, p1);
		}
		
		/*  
		 *	Il metodo utilizzato é il Crossing Number(cn): un punto è considerato 
		 *	interno ad una ROI se, per ogni raggio uscente dal punto, il numero di 
		 *	attraversameti dei lati della ROI è dispari.
		 */		
		if (((p0[1] <= p[1]) && (p1[1] > p[1])) 
		   || ((p0[1] > p[1]) && (p1[1] <= p[1]))) 
				{ 
            
        	float vt = (float)(p[1] - p0[1]) / (p1[1] - p0[1]);
          if (p[0] < p0[0] + vt * (p1[0] - p0[0])) 
          	++cn;   
        }
	}
	/* return 0 se cn è pari (out), e 1 se dispari (in) */
    return (cn&1);    
}

// Estrae la ROI
vtkImageData* roiManager::extraction()
{
	float p[3];
	float t[3];
	int cn;
	int min, max;
	int x, y, z;    	   
	_drroi=loadRoi(_vtkImageRoi);

	/* Slice da estrarre */
    if(_sliceExtraction==0)
	{
	/* Caso Tutte */
		min = 0;
		max = _size[2];
	}
	else if(_sliceExtraction==1)
		{
		/* Caso Corrente */
		min = _currentSlice;
		max = _currentSlice + 1;
	}
	else{
		/* Caso Intervallo */
		min = _intervalMin;
		max = _intervalMax+1;
	}
	

	for(y=0; y<_size[1]; y++)
	{
		for(x=0; x<_size[0]; x++)
		{
			p[0] = x;
			p[1] = y;
			t[0] = p[0];
			t[1] = p[1];

			if(_verFlip){
				t[1] = _size[1] - t[1];
			}

			if(_horFlip){
				t[0] = _size[0] - t[0];
			}

			/* Calcolo dell'inclusione del punto P nella ROI */
			cn = cnInclusion(t);
			if(_typeExtraction==0 && cn==0 || _typeExtraction==1 && cn==1)
			{
				for(z=min; z<max; z++)
				{
					p[2] = z;
					/** Cancellazione dei pixel esterni all'estrazione */
					if(!_rgb) {
						ImageType::IndexType pixelIndex;
						pixelIndex[0]=p[0];
						pixelIndex[1]=p[1];
						pixelIndex[2]=p[2];
						_itkImageExtraction->SetPixel(pixelIndex,_pixelMin);
					}
					else {
						RGBImageType::IndexType rgbPixelIndex;
						rgbPixelIndex[0]=p[0];
						rgbPixelIndex[1]=p[1];
						rgbPixelIndex[2]=p[2];
						_itkRgbImageExtraction->SetPixel(rgbPixelIndex,_pixelMin);
					}
				}
			}
		}
	}

	if(!_rgb) {
		_vtkExtract = itkVtkImage(_itkImageExtraction);
		vtkImageShiftScale* scale = vtkImageShiftScale::New();
		scale->SetOutputScalarTypeToLong();
		scale->SetShift(0);
		scale->SetScale(1);
		scale->SetInput(itkVtkImage(_itkImageExtraction));
		_appCom->RemoveAllInputs();
		_appCom->AddInput(0, scale->GetOutput());
		_appCom->AddInput(0, scale->GetOutput());
		_appCom->AddInput(0, scale->GetOutput());
		_appCom->Update();
		_vtkExtract->DeepCopy(_appCom->GetOutput());
		scale->Delete();
	}
	else {
		_vtkExtract = itkVtkRgbImage(_itkRgbImageExtraction);
		vtkImageShiftScale* scale = vtkImageShiftScale::New();
		scale->SetOutputScalarTypeToLong();
		scale->SetShift(0);
		scale->SetScale(1);
		scale->SetInput(itkVtkRgbImage(_itkRgbImageExtraction));
		_appCom->RemoveAllInputs();
		_appCom->AddInput(0, scale->GetOutput());
		_appCom->Update();
		_vtkExtract->DeepCopy(_appCom->GetOutput());
		scale->Delete();
	}

	/* Cancella tutti i punti del contorno della ROI */
	_drroi->deleteAllPoints();
	if(_verFlip)
		computeFlipping(_vtkExtract,'y');
	if(_horFlip)
		computeFlipping(_vtkExtract,'x');

	return _vtkExtract;
}

// Filtro che consente di collegare dinamicamente le pipeline di ITK e VTK in formato scala di grigio
vtkImageData* roiManager::itkVtkImage(ImageType::Pointer itk)
{
	ConnectorType::Pointer _itkVtkConnector = ConnectorType::New();

	_itkVtkConnector->SetInput(itk);
    _itkVtkConnector->Update();
	_vtk->DeepCopy(_itkVtkConnector->GetOutput());
	return _vtk;
}

// Filtro che consente di collegare dinamicamente le pipeline di ITK e VTK in formato RGB
vtkImageData* roiManager::itkVtkRgbImage(RGBImageType::Pointer itk)
{
	RGBConnectorType::Pointer _itkVtkRgbConnector = RGBConnectorType::New();

	_itkVtkRgbConnector->SetInput(itk);
    _itkVtkRgbConnector->Update();
	_vtk->DeepCopy(_itkVtkRgbConnector->GetOutput());
	return _vtk;
}

// Converte le coordinate di un punto da pixel a mm 
float* roiManager::convertPixeltoMm(int x, int y)
{
	_mm[0] = x * _spacing[0];
	_mm[1] = y * _spacing[1];
	return (float*)_mm;
}

// Set della slice corrente 
void roiManager::setSliceRoi(int slice_corrente)
{
	loadRoi(_vtkImageRoi);
	if(_vtkExtract != _vtkImageRoi)
		loadRoi(_vtkExtract);
	if(!_rgb) _size = _itkImageExtraction->GetLargestPossibleRegion().GetSize();
	else _size = _itkRgbImageExtraction->GetLargestPossibleRegion().GetSize();
	if(_size[2]>1)
		_drroi->Set_slice(slice_corrente);
}

// Get della ROI in formato VTK 
vtkImageData* roiManager::getVtkRoi()
{
	_drroi = loadRoi(_vtkImageRoi);
	if(_vtkExtract != _vtkImageRoi)
		_drroi = loadRoi(_vtkExtract);
	return _drroi->GetOutput();
}

// Set della ROI in formato VTK 
void roiManager::setVtkRoi(vtkImageData* vtkImage)
{
	_vtkImageRoi->DeepCopy(vtkImage);
	_vtkExtract = _vtkImageRoi;
}


// Get del numero dei punti del contorno della ROI 
int roiManager::getNumPoints()
{
	_drroi=loadRoi(_vtkImageRoi);
	if(_vtkExtract!=_vtkImageRoi)
		_drroi=loadRoi(_vtkExtract);
	return _drroi->Get_numPoints();
}

// Seleziona tutti i punti del contorno della ROI 
void roiManager::selectAll() 
{
	loadRoi(_vtkImageRoi);
	if(_vtkExtract!=_vtkImageRoi)
		loadRoi(_vtkExtract);
	if(_drroi->Get_numPoints()!=0)
	{
		_drroi->selectAllPoints();
	}
}
// Cancella tutti i punti del contorno della ROI 
void roiManager::deleteAll() 
{
	loadRoi(_vtkImageRoi);
   	if(_vtkExtract!=_vtkImageRoi)
		loadRoi(_vtkExtract);
    _drroi->deleteAllPoints();
}

// Calcolo dell'angolo formato dalla ROI 
float roiManager::angle()
{
	return (_drroi->angle());
}

// Calcolo del volume di estrazione della ROI 
float roiManager::volumeExtraction()
{
	int min, max, numberSlice;
	float volume = _drroi->volume();

	/* Slice da estrarre */
	if(_sliceExtraction==0)
	{
	/* Caso Tutte */
		min = 0;
		max = _size[2];
	}
	else if(_sliceExtraction==1)
		{
		/* Caso Corrente */
		min = _currentSlice;
		max = _currentSlice + 1;
	}
	else{
		/* Caso Intervallo */
		min = _intervalMin;
		max = _intervalMax+1;
	}
	numberSlice = max - min;
	/* Tipo estrazione: esterno */
	if(_typeExtraction == 1)
		volume = ((_size[0] * _spacing[0] * _size[1] * _spacing[1]) * _spacing[2]) - volume;
	if (numberSlice>1)
    return (volume *numberSlice);
	else 
		return 0;
}

// Calcolo del volume della ROI 
float roiManager::volume()
{
	return (_drroi->volume());
}

//Calcolo dell'area della ROI
float roiManager::area()
{
	return (_drroi->area());
}

//Calcolo della lunghezza del contorno della ROI
float roiManager::length()
{
	return (_drroi->lengthTot());
}

// Carica la ROI: VTK->ROI 
imageDrawRoi* roiManager::loadRoi(vtkImageData* vtkImage)
{
	_drroi->SetInput(vtkImage);

	return _drroi;
}

// Set del colore dei punti del contorno della ROI 
void roiManager::setPointColor(double r, double g, double b)
{
	_drroi->Set_pointColor(r, g, b);
}

// Set del colore delle linee del contorno della ROI 
void roiManager::setLineColor(double r, double g, double b)
{
	_drroi->Set_lineColor(r, g, b);
}

// Set del colore dei punti selezionati del contorno della ROI 
void roiManager::setSelectedPointColor(double r, double g, double b)
{
	_drroi->Set_selectedPointColor(r, g, b);
}

// Set del raggio dei punti del contorno della ROI 
void roiManager::setPointRadius(double radius)
{
	_drroi->Set_pointRadius(radius);
}

void roiManager::OnButtonDown(short _interactionType, wxMouseEvent& event, double x, double y){
	_drroi = loadRoi(_vtkImageRoi);
   	if(_vtkExtract!=_vtkImageRoi){
		_drroi = loadRoi(_vtkExtract);
	}
	if(x<0 || x>=_size[0] || y<0 || y>=_size[1]) return;

	int number = _drroi->Get_numPoints();
	if (number==0) {
		_drroi->Set_closed(0);
		_state = INACTIVE_STATE;
		_drroi->setRectangle(0);
	}
	
	switch(_interactionType){
		case length2d:
			switch(_state){
				_drroi->setShapeToPolygon();
				case INACTIVE_STATE:
					if (event.LeftDown()){	
						_drroi->insertAfterSelectedPoint(x,y);
						_state = DRAWING_STATE;
					}
				break;
				case DRAWING_STATE:
					if (event.RightDown()){	
						if (number==2){
							_drroi->deselectAllPoints();
							_state = EDITING_STATE;
						}
					}
					else if (event.LeftDown()){	
						if (number<2)
							_drroi->insertAfterSelectedPoint(x,y);
					}
				break;
				case EDITING_STATE:
					if (event.LeftDown())
							_drroi->moveSelectedPoints(x,y);
					else if (event.RightDown()){
							_drroi->deselectAllPoints();
							_drroi->startSelectBox(x,y);						
					}
				break;			
			}
		break;
		case angle2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
					if (event.LeftDown()){	
						_drroi->insertAfterSelectedPoint(x,y);
						_state = DRAWING_STATE;
					}
				break;
				case DRAWING_STATE:
					if (event.LeftDown()){	
						_drroi->insertAfterSelectedPoint(x,y);
						_state = EDITING_STATE;
					}	
				break;
				case EDITING_STATE:
					if (event.LeftDown()){	
						if(number==3)
							_drroi->moveSelectedPoints(x,y);
						else
							_drroi->insertAfterSelectedPoint(x,y);
					}
					else if(event.RightDown()){
						_drroi->deselectAllPoints();
						_drroi->startSelectBox(x,y);						
					}			
				break;
			}
		break;
		case polygon2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
					if (event.LeftDown())
						_drroi->insertAfterSelectedPoint(x,y);
					else if (event.RightDown()){	
						_drroi->Set_closed(1);
						_state = EDITING_STATE;
					}
				break;
				case EDITING_STATE:
					if(event.LeftDown()){
						if(event.ShiftDown()){
							_drroi->insertPoint(x,y);
							break;
						}
						_drroi->moveSelectedPoints(x,y);
					}
					else if(event.RightDown()){
						_drroi->deselectAllPoints();
						_drroi->startSelectBox(x,y);
					}
				break;
			}
		break;
		case rectangle2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
					if (event.LeftDown()){	
						_state = DRAWING_STATE;
						_drroi->startRectangle(x,y);
					}
				break;
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.RightDown()){
						_drroi->deselectAllPoints();
						_drroi->startSelectBox(x,y);
					}
				break;
			}
		break;
		case pencil2d:
		_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.LeftDown()){
						if(event.ShiftDown()){
							_drroi->insertPoint(x,y);
							break;
						}
						_drroi->moveSelectedPoints(x,y);
					}
					else if(event.RightDown()){
						_drroi->deselectAllPoints();
						_drroi->startSelectBox(x,y);
					}
				break;
			}
		break;
		}
}

void roiManager::OnMotion(short _interactionType,wxMouseEvent& event, double x, double y){
	_drroi = loadRoi(_vtkImageRoi);
   	if(_vtkExtract!=_vtkImageRoi){
		_drroi = loadRoi(_vtkExtract);
	}
	if(x<0 || x>=_size[0] || y<0 || y>=_size[1]) return;

	int number = _drroi->Get_numPoints();
	if (number==0) {
		_drroi->Set_closed(0);
		_state = INACTIVE_STATE;
		_drroi->setRectangle(0);
	}
	
	switch(_interactionType){
		case length2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
					if(event.Dragging())
						_drroi->moveSelectedPoints(x,y);
				break;
				case EDITING_STATE:
					if(event.Dragging()){
						if(event.LeftIsDown())
							_drroi->moveSelectedPoints(x,y);
						else if(event.RightIsDown())
							_drroi->dragSelectBox(x,y);	
					}
				break;
			}
		break;
		case angle2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				break;
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.Dragging()){
						if(event.RightIsDown())
							_drroi->dragSelectBox(x,y);	
					}
				break;
			}
		break;
		case polygon2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.Dragging()){
						if(event.LeftIsDown())
							_drroi->moveSelectedPoints(x,y);
						else if(event.RightIsDown())
							_drroi->dragSelectBox(x,y);	
					}
				break;
			}
		break;
		case rectangle2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
					if(event.Dragging())
						if(event.LeftIsDown())
							_drroi->dragRectangle(x,y);	
				break;
				case EDITING_STATE:
					if(event.Dragging()){
						if(event.LeftIsDown()){
							if(_drroi->Get_numSelectedPoints() == number)
								_drroi->moveSelectedPoints(x,y);
							else
								_drroi->moveRectangle(x,y);
						}
						else if(event.RightIsDown())
								_drroi->dragSelectBox(x,y);
					}
				break;
			}
		break;
		case pencil2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
					if(event.Dragging())
						if(event.LeftIsDown())
							_drroi->insertAfterSelectedPoint(x,y);
				break;
				case EDITING_STATE:
					if(event.Dragging()){
						if(event.LeftIsDown())
							_drroi->moveSelectedPoints(x,y);	
						else if(event.RightIsDown())
							_drroi->dragSelectBox(x,y);	
					}
				break;
			}
		break;
	}
}

void roiManager::OnButtonUp(short _interactionType,wxMouseEvent& event, double x, double y){
	_drroi = loadRoi(_vtkImageRoi);
   	if(_vtkExtract!=_vtkImageRoi){
		_drroi = loadRoi(_vtkExtract);
	}
	if(x<0 || x>=_size[0] || y<0 || y>=_size[1]) return;

	int number = _drroi->Get_numPoints();
	if (number==0) {
		_drroi->Set_closed(0);
		_state = INACTIVE_STATE;
		_drroi->setRectangle(0);
	}
	
	switch(_interactionType){
		case length2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				break;
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.RightUp())
        				_drroi->endSelectBox(x,y);
				break;
			}
		break;
		case angle2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				break;
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.RightUp())
        				_drroi->endSelectBox(x,y);
				break;
			}
		break;
		case polygon2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
				break;
				case EDITING_STATE:
					if(event.RightUp())
        				_drroi->endSelectBox(x,y);
				break;
			}
		break;
		case rectangle2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				break;
				case DRAWING_STATE:
					if(event.LeftUp()){
						_drroi->endRectangle(x,y);
						_drroi->Set_closed(1);
						_state = EDITING_STATE;
					}
				break;
				case EDITING_STATE:
					if(event.RightUp())
        				_drroi->endSelectBox(x,y);
				break;
			}
		break;
		case pencil2d:
			_drroi->setShapeToPolygon();
			switch(_state){
				case INACTIVE_STATE:
				case DRAWING_STATE:
					if(event.LeftUp()){
						_drroi->Set_closed(1);
						_state = EDITING_STATE;
					}
				break;
				case EDITING_STATE:
					if(event.RightUp())
        				_drroi->endSelectBox(x,y);
				break;
			}
		break;
	}
}

void roiManager::computeFlipping(vtkImageData* image,char axis) 
{ 
	if (axis == 'x' || axis == 'y' || axis == 'z')
	{
		int intAxis = -1;
		switch (axis) {
			case 'x':
				intAxis = 0;
			break;
			case 'y':
				intAxis = 1;
			break;
			case 'z':
				intAxis = 2;
			default:
				intAxis = -1;
		}	
		
		if (intAxis >= 0)
		{
			vtkImageFlip *vtkFlip = vtkImageFlip::New();
			vtkFlip->SetInput(image);
			vtkFlip->SetInformationInput(image);
			vtkFlip->SetFilteredAxes(intAxis);
			vtkFlip->SetInterpolationModeToCubic();
			vtkFlip->InterpolateOn();
			vtkFlip->Update();	
			image->DeepCopy(vtkFlip->GetOutput());					
			vtkFlip->Delete();
		}
	}
}
