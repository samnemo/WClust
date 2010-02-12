// $Id: BatchUtils.h,v 1.7 2009/06/25 04:03:21 samn Exp $ 
#ifndef BATCH_UTILS
#define BATCH_UTILS

#include <string>
#include <vector>

#define     CLUST_USER      0
#define     CLUST_ORIG      1
#define     CLUST_KM        2
#define     CLUST_INFO      3
#define     CLUST_AP        4
#define     CLUST_KK        5
#define     CLUST_FL        6
#define     CLUST_MAX       7

#define     BATCH_OFF			0
#define		BATCH_QUAL_USER		1
#define		BATCH_TSA			2
#define		BATCH_AUTO_CLUST	3
#define		BATCH_QUAL_ORIG		4

static const char* CStrClustNames[7] = {
	"Your new",
	"Original",
	"K-means",
	"Entropy-reduction",
	"Affinity propagation",
	"KlustaKwik",
	"F.L.A.M.E."
};

//batch record for processing cluster quality measurement & time-stamp adjustment batches
struct BatchRec
{
	std::string strBPF;        //input .bpf file path
	std::string strCL;         //input .cl file path
	int iTetrode;              //tetrode to load points from
	double dPrct;              //fraction of points to load from bpf
	std::string outCL;         //output .cl file path
	std::string outBPF;
	std::vector<double> vRatings; //cluster ratings -- 0==unknown,1==poor,2==fair,3==good
	                              //                     1.5==poor-fair,2.5==fair-good
	bool bReadyToRun;			//true iff loaded bpf,cl file successfully

	bool bAutoC;				//whether batch is autoclustering
	int iMinClust;				//min # of clusters
	int iMaxClust;				//max # of clusters
	int iDBIters;				//# of iterations to check optimal # of clusters (for k-means)
	int iIters;					//# of iterations
	int iMaxKKClust;			//max possible # of clusters (for klustakwik)
	int iACType;				//clustering algorithm to use
};

bool ParseQBatchLines(std::vector<std::string>& vstr, std::vector<BatchRec>& vqb,int& iBatchMode);
bool ParseTSBatchLines(std::vector<std::string>& vstr, std::vector<BatchRec>& vb);
bool ParseACBatchLines(std::vector<std::string>& vstr, std::vector<BatchRec>& vb);

#endif
