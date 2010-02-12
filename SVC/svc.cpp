#include "stdafx.h"
/**
 * 	Support Vector Clustering Test and Benchmark
 * 
 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
 * 
 * */

//#include "getopt.h"	//	command line parsing
#include <fcntl.h>	//	file handling

#include <errno.h>	//	standard C error handling
//#include <err.h>	//	standard C error handling
#include <io.h>
#include <cstdio>	//	standard C I/O functions
#include <cstdlib>	//	standard C library functions
#include <cstring>	//	standard C string
#include <cmath>	//	standard C math functions

#include <iostream>	//	C++ I/O functions
#include <fstream>	//	C++ I/O functions
#include <string>	//	C++ string

#include <boost/timer.hpp>	//	Boost Library - Timer

#include "svm.h"				//	SVM Library
#include "LibsvmFileLoader.h"	//	File Loader
#include "SVClustering.h"		//	Classic Support Vector Clustering with Complete Graph Cluster Labeling
#include "CCLSVClustering.h"	//	SVC with Cone Cluster Labeling
#include "HybridSVClustering.h"	//	SVC with Hybrid Cone Cluster Labeling
#include "GKWGenerator.h"		//	Q Generator
#include "KernelMachines.h"		//	Kernel Machine utilities


#include "L1Distance.h"			//	L1 distance 
#include "L2Distance.h"			//	L2 distance


#define DEFAULT_KW				-1		//	default kernel width (no specified - rely on Kernel Width Generator)
#define	DEFAULT_C				1		//	default soft constraint value
#define	DEFAULT_ALG				0		//	default cluster labeling algorithm
#define	DEFAULT_RUNS			100000	//	default number of runs
#define	DEFAULT_L_OFFSET		1		//	default	label offset
#define	DEFAULT_VALID_THRESHOLD	2		//	default minimum number of elements per cluster
#define DEFAULT_SOFTENING		1		//	default softening constant for kernel width generation
#define DEFAULT_EDIST			2		//	default euclidean distance value (no specified - rely on SVC constructor)
#define DEFAULT_KERNEL			-1		//	default kernel type (no specified - rely on SVC constructor)

void printClusterClassAssignments(damina::SVClustering *, unsigned long int, int);

void printAllPoints(damina::SVClustering *, unsigned long int, ofstream &, int);

void countRightAndWrongClassificationsPerClass(damina::SVClustering *, vector<int> &, vector<int> &, vector<int> &, vector<int> &, int, int, vector<unsigned long int> &, unsigned long int);

void computeEvaluatingMeasreusPerClass(vector<int>, vector<int>, vector<int>, vector<int>, vector<double> &, vector<double> &, vector<double> &, vector<double> &, vector<double> &, double &, double &);
		
struct svc_options *parseCommandLineArgs(int, char **);

void usage(char **);

void printUsedCommandLine(int, char *[]);

/**
 *	The enumeration for Cluster Labeling algorithm types
 * */
enum {CCL = 0, CG = 1, HCCL = 2}; /* alg_type: cluster labeling algorithms types */


/**
 *	The enumeration for Euclidean Distance types
 * */
enum {L1 = 1, L2 = 2};	

/**
 *	The enumeration for the BSV clustering policy
 * */
enum {BSV_CLASSIC = 0, BSV_ENHANCED = 1};


/**
 * 	The structure encapsulating the SVC runing options
 * */
struct svc_options {
	unsigned long int runs;				//	number of runs
	unsigned long int alg_type;			//	algorithm type
	double	q;							//	gaussian kernel width
	double C;							//	soft margin constant
	char *filename;						//	data-set filename
	unsigned long int clusters;			//	number of clusters
	unsigned long int l_offset;			//	labels offset
	unsigned long int valid_threshold; 	// 	minimal clusters cardinality 
	double softening;					//	softening constant for kernel width generation
	char *detfile;						//	file to store the detailed cluster assignment
	unsigned long int e_dist;			//	euclidean distance type
	int kernel;							//	kernel type
	unsigned int bsv;						//	BSV clustering policy
};

/**
 *	The main flow for SVC benchmark
 * */
#ifdef DONT
int main(int argc, char *argv[]) {

	using namespace std;
	using namespace damina;
	using namespace boost;
	
	struct svc_options *opt;
	
	opt = parseCommandLineArgs(argc, argv);	// 	parse command line to get parameters
	if (opt == NULL) {						//	parsing fails
		usage(argv);						//	show usage
		exit(1);
	}

	/**
	 * 	Load data file in LibSVM format
	 * */
	LibsvmFileLoader *fl;
	string datafile(opt->filename);
	struct svm_problem *p = NULL;
	cout << "==================================" << endl;
	cout << "Loading file" << endl;
	cout << "==================================" << endl;
	fl = new LibsvmFileLoader(datafile);
	timer tfl;
	p = fl->load(false);
	cout << "File loaded in " << tfl.elapsed() << " secs" << endl;
	cout << "Number of objects: " << p->l << endl << endl;
	delete fl;

	
	/**
	 * 	New SVC instance
	 * */	
	SVClustering *svc;			//	Support Vector Clustering Engine
	switch (opt->alg_type) {
		case CG: svc = new SVClustering(opt->C, p); break;		//	classic one
		case CCL: svc = new CCLSVClustering(opt->C, p); break;	//	with CCL
		case HCCL: svc = new HybridSVClustering(opt->C, p); break;	//	with Hybrid CCL
		default:
			svc = new CCLSVClustering(opt->C, p); break;		//	default is CCL
	}

	if (opt->bsv) {
		svc->useSpecialClusteringPolicyForBSV();
	}
	else {
		svc->useClassicClusteringPolicyForBSV();
	}
		
	/**
	 *	Change kernel type, in requested
	 * */
	if (opt->kernel != DEFAULT_KERNEL) {
		svc->setKernelType(opt->kernel);
	}

	/**
	 * Kernel generator instance
	 * **/	
	GKWGenerator *kwg;
	if (opt->q == -1) {	//	no kernel width value provided on command line	
		kwg = new GKWGenerator(svc, 0.000000001);
	}
	else {	//	kernel width value provided on command line
		kwg = new GKWGenerator(svc, 0.000000001, opt->q);
	}

	kwg->setGrowthSoftening(opt->softening);

	/**
	 *	Change Euclidean Distance, if requested
	 * */
	switch (opt->e_dist) {
		case L1: { svc->setEuclideanDistance(new L1Distance()); kwg->setEuclideanDistance(new L1Distance()); } break;
		case L2: { svc->setEuclideanDistance(new L2Distance()); kwg->setEuclideanDistance(new L2Distance()); } break;
		default: ;
	}

	bool target = false;
	unsigned long int invalid = 0, runs = 0;
	double q, prev_q = -1, prev_C = -1;
	int nSV = 0; //	nr of support vectors

	
	unsigned long int nrClusters, 
				  // nrNonSingletonClusters;
				  nrValidClusters;
	
	timer partial,	//	timer for intermediate steps
			total;	//	timer for overall time measurement 
	
	vector<unsigned long int> ppc;
	vector<int> tp, fp, tn, fn;
	vector<double> precision, recall, f1, contamination, specificity;
	double ma, accuracy, overall_time = 0;

	//dump file
	ofstream *dump_stream;
	if (opt->detfile != NULL) {
		dump_stream = new ofstream(opt->detfile);
	}
	
	while (true) {
		prev_q = q;
		prev_C = svc->getSoftConstraintEstimate();
		
		runs++;
		if (runs > opt->runs) break;

		q = kwg->getNextKernelWidthValue();
		if (q == -1) {
			cout << "No more valid kernel width values" << endl << endl;
			break;
		}
		
		if (opt->runs > 1) {
			cout << "==================================" << endl;
			cout << " Finding Kernel Width # " << runs << endl;
			cout << "==================================" << endl;
		}

		svc->setKernelWidth(q);
		
		/**
		 * 	Perform clustering
		 * */
		total.restart();				//	start total time counter
		partial.restart();				//	start domain description process time counter

		svc->learn();					//	domain description process
		cout << "radius = " << svc->getSphereRadius() << endl;
		cout << endl << "Domain description time taken: ";
		cout << partial.elapsed() << " seconds " << endl;		//	stop domain description process time counter

		partial.restart();				//	start clustering process time counter

		svc->clusterize();				//	cluster labeling process
		cout << "Clustering time taken: ";
		cout << partial.elapsed() << " seconds " << endl;	//	stop clustering process time counter

		cout << "Total time taken: ";
		overall_time += total.elapsed();
		cout << total.elapsed() << " seconds " << endl << endl;		//	stop total time counter
		
		
		nrClusters = svc->getNumberOfClusters();
		// nrNonSingletonClusters = svc->getNumberOfNonSingletonClusters();
		nrValidClusters = svc->getNumberOfValidClusters(opt->valid_threshold);

		cout << "Soft Constraint Estimate: " << svc->getSoftConstraintEstimate() << endl;	
		cout << "Current Soft Constraint: " << svc->getSoftConstraint() << endl;
		cout << "Kernel Width: " << q << endl;
		cout << "Number of clusters: " << nrClusters << endl;
		cout << "Number of valid clusters: " << nrValidClusters << endl;
		cout << "Number of non valid clusters (cardinality < " << opt->valid_threshold << "): " << nrClusters - nrValidClusters << endl;
		
		cout << "Points per cluster: " << endl;
	
		ppc.clear();	
		ppc = svc->getPointPerCluster();
	
		for (unsigned long int i = 0; i < ppc.size(); i++) {
			if (ppc[i] >= opt->valid_threshold) {
				cout << "\tCluster " << i << ": " << ppc[i] << endl;
			}
		}
		cout << endl << endl;

		if (opt->detfile != NULL) {
			printAllPoints(svc, opt->l_offset, *dump_stream, runs);
		}

		countRightAndWrongClassificationsPerClass(svc, tp, fp, tn, fn, opt->clusters, opt->l_offset, ppc, opt->valid_threshold);
		computeEvaluatingMeasreusPerClass(tp, fp, tn, fn, contamination, specificity, precision, recall, f1, ma, accuracy);
		
		for (unsigned long int i = 0; i < tp.size(); i++) {
			cout << "Class " << i << endl << "\tTP: " << tp[i] << "\tFP: " << fp[i] << endl << "\tFN: " << fn[i] << "\tTN: " << tn[i] << endl << endl; 
			cout << "Precision: " << precision[i] << " - Recall: " << recall[i] << " - F1: " << f1[i] << endl;
			cout << "Completeness: " << recall[i] << " - Contamination: " << contamination[i] << endl;
			cout << "Sensitivity: " << recall[i]  << " - Specificity: " << specificity[i] << endl << endl;
		}
		
		cout << endl;
		printClusterClassAssignments(svc, opt->l_offset, opt->clusters);
		cout << endl;
		
		if (opt->clusters > 2) {
			cout << "Macroaveraging: " << ma << endl; 
		}
		
		cout << "Accuracy: " << accuracy << endl << endl;

		// check stop criteria
		if (target) {
			if (nrValidClusters != opt->clusters) {
				break;
			}
			else {
				if (nrClusters - nrValidClusters > invalid) {
					break;
				}
			}
		}
		else {
			if (nrValidClusters == opt->clusters) {
				target = true;
				invalid = nrClusters - nrValidClusters;
			}
		}

	 	nSV = svc->getModel()->l - svc->getModel()->lbounded;
		
		if (nSV >= ceil((50.0 * p->l) / 100)) {
			cout << "Too much support vectors. Exiting..." << endl << endl;
			break;
		}
	}

	if (opt->detfile != NULL) {
		dump_stream->close();
	}

	cout << "==================================" << endl;
	cout << "Clustering process finished" << endl;
	cout << "==================================" << endl;
	if (opt->runs > 1) {
		cout << "Check out the last run and second last run." << endl;
		cout << "Kernel Width Found: " << prev_q << endl;
	}   
	cout << "Soft Constraint Estimate: " << prev_C << endl;
    cout << "Overall time elapsed: " << overall_time << " secs" << endl;
//	cout << "Memory used: " << sizeof(svc) + sizeof(kwg) << endl;
	cout << "Original command line: "; 
	printUsedCommandLine(argc, argv);

	delete svc;
	delete kwg;
	free(opt);
	
	return 0;
}
#endif

/**
 *	For each cluter print how many points of each class there are 
 *
 * 	e.g.: Cluster 0: 10 points from class 0, 2 points from class 1, 1 poit from class 2
 *
 *	@param	clust	The involved Support Vector Clustering engine
 *	@param	classOffset	The label of the first class in the original classification	
 *	@param	nrClasses The number of original classes
 * */
void printClusterClassAssignments(damina::SVClustering *clust, unsigned long int classOffset, int nrClasses) {

	std::vector<int> labels = clust->getClustersAssignment();		//	clustering result
	std::vector<int> orig_c = clust->getOriginalClasses((int)classOffset);		//	original classification

	int nrClusters = clust->getNumberOfClusters();
	int k, j ;	
	int count;
	
	for (j = 0; j < nrClusters; j++) {
		cout << "Cluster " << j << ": ";
		for (k = 0; k < nrClasses; k++) {
		   	count =	0;
			for (unsigned long int i = 0; i < labels.size(); i++) {
				if ((labels[i] == j) && (orig_c[i] == k)) count++;
			}
			cout << count << " points from Class " << k << ", ";
		}
		cout << endl;
	}
}


/**
 *	Print all points with their original classes and cluster assigned
 *
 *	@param	clust	The involved Support Vector Clustering engine
 *	@param	classOffset	The label of the first class in the original classification	
 *	@param	stream	The stream to write to
 * */
void printAllPoints(damina::SVClustering *clust, unsigned long int classOffset, ofstream &out, int runs) {
	
	out << "================" << endl;
	out << "Dumping run #" << runs << endl;
	out << "================" << endl;
	out << endl;

	std::vector<int> labels = clust->getClustersAssignment();		//	clustering result
	std::vector<int> orig_c = clust->getOriginalClasses((int)classOffset);		//	original classification

	for (unsigned long int i = 0; i < labels.size(); i++) {
		out << "Point #" << i << " - " << "Cluster: " << labels[i] << ", Class: " << orig_c[i] << endl;
	}
	out << endl;
}

/**
 *	For all classes computes the number of true/false positives and true/false negatives
 *
 *	@param	clust	The involved Support Vector Clustering engine
 *	@param	tp		The true positives vector for all classes
 *	@param	fp		The flase positives vector for all classes
 *	@param	tn		The true negatives vector for all classes
 *	@param	fp		The flase negatives vector for all classes
 *	@param	nrClasses	The number of the original classes
 *	@param	classOffset	The label of the first class in the original classification
 * */
void countRightAndWrongClassificationsPerClass(damina::SVClustering *clust, vector<int> &tp, vector<int> &fp, vector<int> &tn, vector<int> &fn, int nrClasses, int classOffset, vector<unsigned long int> &ppc, unsigned long int valid_t) {
	
	std::vector<int> labels = clust->getClustersAssignment();		//	clustering result
	std::vector<int> orig_c = clust->getOriginalClasses(classOffset);		//	original classification
	
	int datasetDimension = clust->getProblem()->l;

	tp.clear();	
	fp.clear();	
	tn.clear();	
	fn.clear();	
	
	tp.resize(nrClasses);
	fp.resize(nrClasses);
	tn.resize(nrClasses);
	fn.resize(nrClasses);	
	
	for (unsigned long int i = 0; i < labels.size(); i++) {
		if (orig_c[i] >= nrClasses) continue;
		
		if ((ppc[labels[i]] >= valid_t) && (labels[i] == orig_c[i])) {
			tp[orig_c[i]]++;
		}
		else {
			fp[labels[i]]++;
			fn[orig_c[i]]++;
		}
	}
	
	for (int i = 0; i < nrClasses; i++) {
		tn[i] = datasetDimension - tp[i] - fp[i] - fn[i];
	}
}


/**
 *	For all classes computes the precision/recall/f1 values
 *
 *	@param	tp		The true positives vector for all classes
 *	@param	fp		The flase positives vector for all classes
 *	@param	tn		The true negatives vector for all classes
 *	@param	fp		The flase negatives vector for all classes
 *	@param	contamination	The contamination value for all classes
 *	@param	precision	The precision value for all classes
 *	@param	recall	The recall value for all classes
 *	@param	f1	The f1 value for all classes 
 * */
void computeEvaluatingMeasreusPerClass(vector<int> tp, vector<int> fp, vector<int> tn, vector<int> fn, 
			vector<double> &contamination, vector<double> &specificity, 
			vector<double> &precision, vector<double> &recall, vector<double> &f1, double &ma, double &accuracy) {

	unsigned long int nrClasses = tp.size();
	int datasedDim = tp[0] + fp[0] + tn[0] + fn[0];
	
	precision.clear();
	recall.clear();
	f1.clear();
	
	contamination.resize(nrClasses);
	precision.resize(nrClasses);
	recall.resize(nrClasses);
	f1.resize(nrClasses);
	specificity.resize(nrClasses);
	
	ma = 0;
	accuracy = 0;
	
	for (unsigned long int i = 0; i < nrClasses ; i++) {
		accuracy += tp[i];
		precision[i] = ((double)tp[i] / (double)(tp[i] + fp[i])) * 100;
		recall[i] = ((double)tp[i] / (double)(tp[i] + fn[i])) * 100;
		specificity[i] = ((double)tn[i] / (double)(tn[i] + fp[i])) * 100;
		contamination[i] = (double)fp[i] / ((double)tp[i] + (double)fp[i]) * 100;
		if (precision[i] + recall[i] == 0) {
			f1[i] = -1; 
		}
		else {
			f1[i] = (2 * (precision[i] * recall[i])) / (precision[i] + recall[i]);
			ma += f1[i];
		}

	}
		
	ma = ma / (double) nrClasses;
	accuracy = accuracy / (double)datasedDim * 100;
}


/**
 * 	Parse the command line arguments and return the options for the SVC running
 * 
 * 	
 *	@param	argc	The number of arguments
 * 	@param	argv	The arguments
 * 
 * 	@return	The options 	
 * */
#ifdef DONT
struct svc_options *parseCommandLineArgs(int argc, char **argv) {

	using namespace damina;
	
	struct svc_options *opt; 
	opt = (struct svc_options *)malloc(sizeof(struct svc_options));
	
	opt->filename = (char *)malloc(2 * sizeof(char));
	strcpy(opt->filename, "");
	
	opt->q = DEFAULT_KW;
	opt->C = DEFAULT_C;
	opt->alg_type = DEFAULT_ALG;
	opt->runs = DEFAULT_RUNS;
	opt->l_offset = DEFAULT_L_OFFSET;
	opt->valid_threshold = DEFAULT_VALID_THRESHOLD;
	opt->softening = DEFAULT_SOFTENING;
	opt->detfile = NULL;
	opt->e_dist = DEFAULT_EDIST;
	opt->kernel = DEFAULT_KERNEL;
	opt->bsv = BSV_CLASSIC;

	bool file_f = true;
	bool cluster_f = false;
	
	int ch;
	
	/* options descriptor */
	static struct option longopts[] = {
	        { "kernel-width", 	required_argument,	NULL, 	'q' },
	        { "soft-margin",	required_argument,	NULL,	'C' },
	        { "num-clusters",	required_argument,	NULL,	'c' },
	        { "labeling-alg",	required_argument,	NULL,	'l' },
	        { "dataset-file",	required_argument,	NULL,	'f' },
	        { "runs",			required_argument,	NULL,	'r' },
	        { "labels-offset",	required_argument,	NULL,	'o' },
	        { "valid-threshold",required_argument,	NULL,	't' },
	        { "softening",		required_argument,	NULL,	's' },
	        { "dump-details",	required_argument,	NULL,	'D' },
	        { "euclidean-distance",	required_argument,	NULL,	'e' },
	        { "kernel-type",	required_argument,	NULL,	'k' },
	        { "bsv-clustering",	required_argument,	NULL,	'b' }

	};

	while ((ch = getopt_long(argc, argv, "q:C:c:l:f:r:o:t:s:D:e:k:b:", longopts, NULL)) != -1) {
		switch (ch) {
				
			case 'b': {
					opt->bsv = (int)strtol(optarg, (char**)NULL, 10);
					//if ((errno == EINVAL) || (opt->bsv != (int)BSV_CLASSIC) || (opt->bsv != (int)BSV_ENHANCED))
					//	err(1, "%s is an invalid value for BSV Clustering Policy. It must be an inter value in [%d,%d].", optarg, BSV_CLASSIC, BSV_ENHANCED);
					break;
			}


			case 'k': {
					opt->kernel = (int)strtol(optarg, (char**)NULL, 10);
					//if ((errno == EINVAL) || (opt->kernel != KernelType::gaussian) || (opt->kernel != KernelType::laplacian))
					//	err(1, "%s is an invalid value for Kernel Type. It must be %d or %d.", optarg, KernelType::gaussian, KernelType::laplacian);
					break;
			}

			case 'e': {
					opt->e_dist = (int)strtol(optarg, (char**)NULL, 10);
					if ((errno == EINVAL) || (opt->e_dist < 1) || (opt->e_dist > 2))
						fprintf(stderr, "%s is an invalid value for Euclidean Distance type. It must be an inter value in [1,2].", optarg);
					break;
			}

			case 'q': {
					opt->q = (double)strtod(optarg, (char**)NULL);
					if ((errno == EINVAL) || (opt->q <= 0))
						fprintf(stderr, "%s is an invalid value for Kernel Width. It must be a real value greater than 0.", optarg);
					break;
			}
			
			case 'C': {
					opt->C = (double)strtod(optarg, (char**)NULL);
					if ((errno == EINVAL) || (opt->C <= 0))
						fprintf(stderr, "%s is an invalid value for Soft Margin Constant. It must be a real value greater than 0.", optarg);
					break;
			}
			
			case 'c': {
					opt->clusters = (int)strtol(optarg, (char**)NULL, 10);
					if ((errno == EINVAL) || (opt->clusters <= 0))
						fprintf(stderr, "%s is an invalid value for Number of Clusters. It must be an integer value greater than 0.", optarg);
					
					cluster_f = true;
					break;
			}
			
			case 'r': {
					opt->runs = (int)strtol(optarg, (char**)NULL, 10);
					if ((errno == EINVAL) || (opt->runs <= 0))
						fprintf(stderr, "%s is an invalid value for Number of Runs. It must be an integer value greater than 0.", optarg);
					break;
			}
			
			case 's': {
					opt->softening = (double)strtod(optarg, (char**)NULL);
					if ((errno == EINVAL) || (opt->softening <= 0))
						fprintf(stderr, "%s is an invalid value for the Softening Constant. It must be an real value greater than 0.", optarg);
					break;
			}
		
			case 't': {
					opt->valid_threshold = (int)strtol(optarg, (char**)NULL, 10);
					if ((errno == EINVAL) || (opt->valid_threshold <= 0))
						fprintf(stderr, "%s is an invalid value for Minimum Clusters Cardinality. It must be an integer value greater than 0.", optarg);
					break;
			}

			case 'o': {
					opt->l_offset = (int)strtol(optarg, (char**)NULL, 10);
					if ((errno == EINVAL) || (opt->l_offset < 0))
						fprintf(stderr, "%s is an invalid value for Labels Offset. It must be an integer value greater than or equal to 0.", optarg);
					break;
			}
			
			case 'l': {
					opt->alg_type = (int)strtol(optarg, (char**)NULL, 10);
					if ((errno == EINVAL) || (opt->alg_type > 2) || (opt->alg_type < 0))
						fprintf(stderr, "%s is an invalid value for Cluster Labeling algorithm. It must be 0, 1 or 2.", optarg);
					break;
			}
			
			case 'f': {
					if ((open(optarg, O_RDONLY, 0)) == -1) {
                    	fprintf(stderr, "Unable to open %s", optarg);
                    	break;
					}			

					free(opt->filename);	
					opt->filename = (char *)malloc((strlen(optarg) + 1) * sizeof(char));
					strcpy(opt->filename, optarg);
											
					file_f = true;
					break;
			}
			
			case 'D': {
					free(opt->detfile);
					opt->detfile = (char *)malloc((strlen(optarg) + 1) * sizeof(char));
					strcpy(opt->detfile, optarg);
					break;
			}
			
			default: {
				usage(argv);
			}
		}
	}
	argc -= optind;
   	argv += optind;	

	if ((file_f == false) || (cluster_f == false)) {
		return NULL;
	}
	return opt;
}

/**
 *	Print the usage on the standard error
 *
 *	@param	argv	The argv parameter passed on the comand line
 * */
void usage(char **argv) {
	using namespace damina;
	
	cerr << endl << "Usage: " << argv[0] << " -q (0,+inf) -C (0,+inf) -b [" << BSV_CLASSIC << "," << BSV_ENHANCED << "] -k" << KernelType::gaussian << "," << KernelType::laplacian << " -e [1,2] -c [1, N] -l [0-2] -f <dataset-filename> -D <detail-filename> -r [1, N] -o [0, N] -t [1,N] -s (0,1]" << endl << endl;
	cerr << "\t -q: the kernel width value must be a real value greater than zero - DEFAULT: auto" << endl; 
	cerr <<	"\t -C: the soft margin constant must be a real value greater than zero - DEFAULT: " << DEFAULT_C << endl;
	cerr <<	"\t -b: the BSV Clustering Policy - DEFAULT: " << BSV_ENHANCED << endl;
		cerr << "\t\t " << BSV_CLASSIC << " for classic policy" << endl;
		cerr << "\t\t " << BSV_ENHANCED << " for enhanced policy (aim to get better results)" << endl;
	cerr <<	"\t -k: the Kernel Type - DEFAULT: " << KernelType::gaussian << endl;
		cerr << "\t\t " << KernelType::gaussian << " for Gaussian Kernel (K(x,y) = exp(-gamma * ||x-y||^2))" << endl;
		cerr << "\t\t " << KernelType::laplacian << " for Laplacian Kernel (K(x,y) = exp(-gamma*|x-y|_1)) " << endl;
	cerr <<	"\t -e: the Euclidean Measure to use in vector-distance measurement - DEFAULT: " << L2 << endl;
		cerr << "\t\t " << L1 << " for L1 distance (sum |x_i - y_i|) " << endl;
		cerr << "\t\t " << L2 << " for L2 distance (sum sqrt(||x_i - y_i||^2)) " << endl;
	cerr << "\t -l: the cluster labeling algorithm type - DEFAULT: " << DEFAULT_ALG << endl;
		cerr << "\t\t " << HCCL << " for Hybrid Cone Cluster Labeling" << endl;
		cerr << "\t\t " << CCL << " for Cone Cluster Labeling" << endl;
		cerr << "\t\t " << CG << " for Complete Graph Cluster Labeling" << endl;
	cerr << "\t -c: the number of cluster to request must be an integer value greater than zero" << endl;
	cerr << "\t -f: the filename of the dataset file, in LIBSVM format" << endl;
	cerr << "\t -D: the filename of the file to store detailed clusters assignment" << endl;
	cerr << "\t -r: the number of runs to perform - DEFAULT: " << DEFAULT_RUNS	<< endl;
	cerr << "\t -o: the labels offset (i.e. the label of the first class) - DEFAULT: " << DEFAULT_L_OFFSET << endl;
	cerr << "\t -s: the softening constant for kernel width generation - DEFAULT: " << DEFAULT_SOFTENING << endl;
	cerr << "\t -t: the minimum clusters cardinality - DEFAULT: " << DEFAULT_VALID_THRESHOLD << endl << endl;
}

/**
 *	Print the whole command line used to invoke the program
 *
 *	@param	opt	The result of the command line parsing
 * */
void printUsedCommandLine(int argc, char *argv[]) {
	cout << argv[0] << " ";

	for (int i = 1; i <= argc; i++) {
		cout << argv[i] << " ";
	}
	cout << endl << endl;
}
#endif