/**
 * \file itkDaub.h
 * \brief Questo file contiene le definizioni dei filtri che usiamo nell'analisi wavelet con 
 * le funzioni Daubechies
 * \author ICAR-CNR Napoli
 */

#ifndef _itkDaub_h_
#define _itkDaub_h_

#if !defined(M_SQRT2)
#define M_SQRT2     1.41421356237309504880168872420969808
#endif

#ifndef SQRT3
#define SQRT3 1.73205080756887729352745
#endif

#define N_ELEM(st) (sizeof(st)/sizeof((st)[0]))


/* 
* Definiamo una struttura generica atta a contenere i coefficienti dei filtri 
* e altre informazioni di supporto per la trasformazione
*
*/

class waveletfilter {
public:
	double *cH;
	int nH;
	int nHtilde;
	double *cHtilde;
	int offH, offG, offHtilde, offGtilde;
} ;


/*
 *	-----------------------------------------------------------------
 *	Filtri di Daubechies 
 *
 *	
 */


double cHDaubechies_4[] = {
	M_SQRT2 * (1 + SQRT3) / 8.0,
	M_SQRT2 * (3 + SQRT3) / 8.0,
	M_SQRT2 * (3 - SQRT3) / 8.0,
	M_SQRT2 * (1 - SQRT3) / 8.0
};


double cHDaubechies_6[] = {
	 0.332670552950,
	 0.806891509311,
	 0.459877502118,
	-0.135011020010,
	-0.085441273882,
	 0.035226291882
};

double cHDaubechies_8[] = {
	 0.230377813309,
     0.714846570553,
	 0.6308807667930,
	-0.027983769417,
	-0.187034811719,
	 0.030841381836,
	 0.032883011667,
	-0.010597401785
};


double cHDaubechies_10[] = {
	 0.1601023979741929,
	 0.6038292697971895,
	 0.7243085284377726,
	 0.1384281459013203,
	-0.2422948870663823,
	-0.0322448695846381,
	 0.0775714938400459,
	-0.0062414902127983,
	-0.0125807519990820,
	 0.0033357252854738
};

double cHDaubechies_12[] = {
	 0.1115407433501095,
	 0.4946238903984533,
	 0.7511339080210959,
	 0.3152503517091982,
	-0.2262646939654400,
	-0.1297668675672625,
	 0.0975016055873225,
	 0.0275228655303053,
	-0.0315820393184862,
	 0.0005538422011614,
	 0.0047772575119455,
	-0.0010773010853085
};

double cHDaubechies_20[] = {
	 0.026670057901,
	 0.188176800078,
	 0.527201188932,
	 0.688459039454,
	 0.281172343661,
	-0.249846424327,
	-0.195946274377,
	 0.127369340336,
	 0.093057364604,
	-0.071394147166,
	-0.029457536822,
	 0.033212674059,
	 0.003606553567,
	-0.010733175483,
	 0.001395351747,
	 0.001992405295,
	-0.000685856695,
	-0.000116466855,
	 0.000093588670,
	-0.000013264203
};

#endif _itkDaub_h_