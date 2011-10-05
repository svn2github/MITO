/**
 * \file anonymizeFilter.h
 * \brief Filtro per l'anonimizzazione di immagini DICOM
 * \author ICAR-CNR Napoli
 */

#ifndef _anonymizeFilter_h_
#define _anonymizeFilter_h_

#include "itkVtkFilter.h"

// Used DICOM Tags
#define STUDY_DATE "0008|0020"
#define SERIES_DATE "0008|0021"
#define ACQUISITION_DATE "0008|0022"
#define IMAGE_DATE "0008|0023"
#define STUDY_TIME "0008|0030"
#define SERIES_TIME "0008|0031"
#define IMAGE_TIME "0008|0033"
#define ACCESSION_NUMBER "0008|0050"
#define INSTITUTION_NAME "0008|0080"
#define REFERRING_PHYSICIAN "0008|0090"
#define PHYSICIAN_OF_RECORD "0008|1048"
#define PERFORMING_PHYSICIAN "0008|1050"
#define PATIENT_NAME "0010|0010"
#define PATIENT_ID "0010|0020"
#define PATIENT_BIRTHDATE "0010|0030"
#define PATIENT_SEX "0010|0040"
#define PATIENT_AGE "0010|1010"
#define PATIENT_WEIGHT "0010|1030"
#define TRIAL_SPONSOR_NAME "0012|0010"
#define TRIAL_PROTOCOL_ID "0012|0020"
#define TRIAL_PROTOCOL_NAME "0012|0021"
#define TRIAL_SITE_ID "0012|0030"
#define TRIAL_SITE_NAME "0012|0031"
#define TRIAL_SUBJECT_ID "0012|0040"
#define TRIAL_SUBJECT_READING_ID "0012|0042"
#define TRIAL_TIME_POINT_ID "0012|0050"
#define TRIAL_TIME_POINT_DESCRIPTION "0012|0051"
#define TRIAL_COORDINATING_CENTER_NAME "0012|0060"
#define RESCALE_INTERCEPT "0028|1052"


/**
* \brief Struttura contenente i campi da anonimizzare
*
* La struttura anonymizeData contiene una lista di dati
* che è possibile anonimizzare
*
*/
struct anonymizeData {
	string studyDate;
	string seriesDate;
	string acquisitionDate;
	string imageDate;
	string studyTime;
	string seriesTime;
	string imageTime;
	string accessionNumber;
	string institutionName;
	string referringPhysician;
	string physicianOfRecord;
	string performingPhysician;
	string patientName;
	string patientId;
	string patientBirthdate;
	string patientSex;
	string patientAge;
	string patientWeight;
	string trialSponsorName;
	string trialProtocolId;
	string trialProtocolName;
	string trialSiteId;
	string trialSiteName;
	string trialSubjectId;
	string trialSubjectReadingId;
	string trialTimePointId;
	string trialTimePointDescription;
	string trialCoordinatingCenterName;
};


/**
 * \class anonymizeFilter
 * \brief Classe per l'anonimizzazione di immagini DICOM
 *
 * Questa classe si occupa di anonimizzare alcuni campi dell'header
 * delle immagini o delle serie DICOM 
 */
class anonymizeFilter {

private:
	/** 
	 * \var bool _rgb
	 * \brief Indica se il formato di un'immagine è RGB o scala di grigi
	 */	
	bool _rgb;

public:

	/** Costruttore con parametro rgb */
	anonymizeFilter(bool rgb);

	/**
	 * \fn void anonymizeImage(string inputFilename, string outputFilename, anonymizeData* aData)
	 * \brief Anonimizza un'immagine DICOM
	 * \param inputFilename Nome del file origine
	 * \param outputFilename Nome del file destinazione
	 * \param aData Insieme di campi da anonimizzare
	 */
	void anonymizeImage(string inputFilename, string outputFilename, anonymizeData* aData);

	/**
	 * \fn void anonymizeSeries(string inputDirectory, string outputDirectory, anonymizeData* aData)
	 * \brief Anonimizza una serie DICOM
	 * \param inputDirectory Nome della directory origine
	 * \param outputDirectory Nome della directory destinazione
	 * \param aData Insieme di campi da anonimizzare
	 */
	void anonymizeSeries(string inputDirectory, string outputDirectory, anonymizeData* aData);

	/**
	 * \fn bool getRgb()
	 * \brief Ottiene il valore della variabile _rgb
	 * \return Valore della variabile _rgb
	 */
	inline bool getRgb() {
		return _rgb;
	}

	/**
	 * \fn void setRgb(bool rgb)
	 * \brief Setta il valore della variabile _rgb
	 * \param rgb Valore da assegnare alla variabile _rgb
	 */
	inline void setRgb(bool rgb) {
		_rgb = rgb;
	}
};

#endif _anonymizeFilter_h_