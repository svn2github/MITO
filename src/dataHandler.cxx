/**
 * \file dataHandler.cxx
 * \brief File per la gestione dei dati appData
 * \author ICAR-CNR Napoli
 */

#include "dataHandler.h"

dataHandler::dataHandler() {
	_idDataCounter = 1;
}


dataHandler::~dataHandler() {
	deleteAllData();
}


unsigned int dataHandler::newData(appData *appData, unsigned int idViewer, bool isReference) {
	dataItem *data = new dataItem();
		
	data->appData = appData;
	data->idViewer = idViewer;
	data->isReference = isReference;
	if (!data->isReference)
		data->appData->setIdData(_idDataCounter);

	_dataList.push_back(data);
	_idDataCounter++;
	
	return _idDataCounter - 1;
}


appData* dataHandler::getData(unsigned int idData) {
	dataItem *data = getDataItem(idData);
	if (data)
		return getDataItem(idData)->appData;
	else
		return NULL;
}


dataHandler::dataItem* dataHandler::getDataItem(unsigned int idData) {
	list<dataItem*>::iterator iteList = _dataList.begin();
    dataItem *data;

	while (iteList != _dataList.end()) {
		data = *iteList;
		if (data->appData->getIdData() == idData)
			return data;
		iteList++;
	}
	
	return NULL;
}


void dataHandler::deleteAllData() {
	list<dataItem*>::iterator iteList = _dataList.begin();
    dataItem *data;

	while (iteList != _dataList.end()) {
		data = *iteList;
		if (data) {
			if (!data->isReference)
				delete data->appData;
			delete data;
		}
		iteList++;
	}
	_dataList.clear();
}


void dataHandler::deleteData(unsigned int idData) {
	list<dataItem*>::iterator iteList = _dataList.begin();
	dataItem *data;

	while (iteList != _dataList.end()) {
		data = *iteList;
		if (data) {
			if (data->appData->getIdData() == idData && data->isReference) {
				_dataList.remove(data);
				delete data;
				iteList = _dataList.begin();
				iteList--;
			}
		}
		iteList++;
	}
	data = getDataItem(idData);
	if (data) {
		_dataList.remove(data);
		delete data->appData;
		delete data;
	}
}


list<unsigned int>* dataHandler::getIdViewerByIdData(unsigned int idData) {
	if (idData) {
		list<dataItem*>::iterator iteList = _dataList.begin();
		list<unsigned int>* viewerList = new list<unsigned int>;
		dataItem *data;

		while (iteList != _dataList.end()) {
			data = *iteList;
			if (data) {
				if (data->appData->getIdData() == idData)
					viewerList->push_back(data->idViewer);
			}
			iteList++;
		}
		
		return viewerList;
	}
	else return NULL;
}


list<unsigned int>* dataHandler::getIdDataByIdViewer(unsigned int idViewer) {
	if (idViewer) {
		list<dataItem*>::iterator iteList = _dataList.begin();
		list<unsigned int> *viewerList = new list<unsigned int>;
		dataItem *data;

		while (iteList != _dataList.end()) {
			data = *iteList;
			if(data) {
				if (data->idViewer == idViewer)
					viewerList->push_back(data->appData->getIdData());
			}
			iteList++;
		}
		return viewerList;
	}
	else return NULL;
}
//è solo per prova, è presa dalla vecchia versione: non si deve usare!
list < unsigned int > *dataHandler::getAllIdData() {
	list < dataItem* >::iterator iteList = _dataList.begin();
	list < unsigned int > *o = new list <unsigned int >;
	dataItem *d;

	while (iteList != _dataList.end()) {
		d = *iteList;
		if (d) {
			o->push_back(d->appData->getIdData());
		}
		iteList++;
	}

	return o;
}