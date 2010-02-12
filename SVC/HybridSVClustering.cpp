#include "stdafx.h"
#include "HybridSVClustering.h"

namespace damina
{

	
	/**
	 * 	Constructor
	 * 
	 * 	@param	train	The input data set
	 * */
	HybridSVClustering::HybridSVClustering(DataSet *train): CCLSVClustering(train) {
		msvm = new MultiClassSVM();	
	}
	
	/**
	 * 	Constructor
	 * 	
	 * 	@param	p	The input data in libsvm format
	 * */
	HybridSVClustering::HybridSVClustering(struct svm_problem *p): CCLSVClustering(p) {
		msvm = new MultiClassSVM();		
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	train	The input data set
	 * */
	HybridSVClustering::HybridSVClustering(double C, DataSet *train): CCLSVClustering(C, train) {
		msvm = new MultiClassSVM();	
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	prob	The input data set in libsvm format
	 * */
	HybridSVClustering::HybridSVClustering(double C, struct svm_problem *p): CCLSVClustering(C, p) {
		msvm = new MultiClassSVM();	
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	w	The gaussian kernel width 
	 * 	@param	train	The input data set
	 * */
	HybridSVClustering::HybridSVClustering(double C, double w, DataSet *train): CCLSVClustering(C, w, train) {
		msvm = new MultiClassSVM();	
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	w	The gaussian kernel width 
	 * 	@param	prob	The input data set in libsvm format
	 * */
	HybridSVClustering::HybridSVClustering(double C, double w, struct svm_problem *p): CCLSVClustering(C, w, p) {
		msvm = new MultiClassSVM();	
	}
	
	
	/**
	 * 	Destructor
	 * 
	 * */
	HybridSVClustering::~HybridSVClustering()	{
		delete msvm; 
	}
	
	/**
	 * 	Run the clustering process.
	 * 
	 * 	In this case it runs the Step 2 of Support Vector Clustering, called "Cluster Labeling" by its authors.
	 * 
	 * 	The implementation reflects the solution proposed in 
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Cone Cluster Labeling for Support Vector Clustering," 
	 * 	in Proceedings of 6th SIAM Conference on Data Mining, 2006, pp. 484-488.</em>
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Selection and Fast Cluster Labeling 
	 * 	for Support Vector Clustering," Department of Computer Science, 
	 * 	University of Massachussets Lowell 2005.</em> 
	 * 
	 * 	namely, Cone Cluster Labeling but with some modifications which involve an SVM Classifier
	 * 
	 * */
	void HybridSVClustering::clusterize() {
		this->separateClusters();
		//this->experimentalSeparateClusters();
	}

	
	/*	PROTECTED METHODS FOLLOW	************************************************************************/
	
		
	/**
	 * 	Run the clustering process.
	 * 
	 * 	In this case it runs the Step 2 of Support Vector Clustering, called "Cluster Labeling" by its authors.
	 * 
	 * 	The implementation reflects the solution proposed in 
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Cone Cluster Labeling for Support Vector Clustering," 
	 * 	in Proceedings of 6th SIAM Conference on Data Mining, 2006, pp. 484-488.</em>
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Selection and Fast Cluster Labeling 
	 * 	for Support Vector Clustering," Department of Computer Science, 
	 * 	University of Massachussets Lowell 2005.</em> 
	 * 
	 * 	namely, Cone Cluster Labeling
	 * 
	 * */
	void HybridSVClustering::separateClusters() {
		
		// compute Zeta
		calculateZeta();
		
		int i,j;		//	to run over the SVs 
			
		int nSV = this->trainer->getModel()->l; // - this->trainer->getModel()->lbounded;
		int *SV_index = this->trainer->getModel()->SV_index;


		struct svm_node **allSV = this->trainer->getModel()->SV;
		
		/*	original dataset */
		struct svm_node **x = this->trainer->getProblem()->x;
		double dist;
		
		
		/* arithmetic mean of all betas */
		double betas_average = 0;
		double beta_max = -1;
		
		/*	adj matrix only for real SVs */
		UndirectedGraph adj(nSV);
		for (i = 0; i < nSV; i++) {

			for (j = 0; j < nSV; j++) {
				
				//	trivial: each node is connected to itself
				if (i == j) {
					add_edge(i, j, adj);
					continue;
				}
				
				// avoid to set the same edge more than once
				if (edge(i, j, adj).second) continue;
				
				
				//dist = eucDist->calculateDistance(x[SV_index[i]], x[SV_index[j]]);
				dist = eucDist->calculateDistance(allSV[i], allSV[j]);
				
				// 	zeta is the radius of the hyperspheres in data space centered in the SVs
				//	(all hyperspheres have the same radius)
				//	Two SVs are connected if their hyperspheres overlap, i.e. if the 
				//	distance between two SVs are at most 2*zeta (overlap in 1 point)
				if (dist <= 2*zeta) {		  
					add_edge(i, j, adj);
					add_edge(j, i, adj);
				}
				else {
					remove_edge(i, j, adj);
					remove_edge(j, i, adj);
				}
			}
		}
		
		
		// clustering SVs finding connected components of the adj matrix just built
		std::vector<int> svLabels(nSV);
		connected_components(adj, &svLabels[0]);
		
		
		// total points
		int N = this->trainer->getProblem()->l;
		
		// total labels
		//labels.resize(N);

		
		C_estimation = 10.0 * getSoftConstraint() / N;	
		

		classifyUnclusteredPoints(svLabels);
	}
	
	void HybridSVClustering::classifyUnclusteredPoints(std::vector<int> &lab) {

		int i;		//	to run over the SVs 
			
		int nSV = this->trainer->getModel()->l; // - this->trainer->getModel()->lbounded;
		int *SV_index = this->trainer->getModel()->SV_index;
		
		
		struct svm_node **allSV = this->trainer->getModel()->SV;
		/*	original dataset */
		struct svm_node **x = this->trainer->getProblem()->x;
		
		/*int nBSV = this->trainer->getModel()->lbounded;
		int *BSV_index = this->trainer->getModel()->BSV_index;
		double mindist, dist;
		int minsvindex;		
		for (int i = 0; i < nBSV; i++) {
			mindist = MAXFLOAT; 
			minsvindex = 0;
			for (int j = 0; j < nSV; j++) {
				dist = eucDist->calculateDistance(x[BSV_index[i]],x[SV_index[j]]);
				if (dist < mindist) {
					mindist = dist;
					minsvindex = j;
				}
			}
			labels[BSV_index[i]] = labels[minsvindex];
		}*/	


		int n_features = 0;
		struct svm_node *temp = x[0];
		while (temp->index != -1) {
			n_features++;
			temp++;
		}

		struct svm_problem * msvm_train = (struct svm_problem *)malloc(sizeof(struct svm_problem));
		
		msvm_train->l = nSV;
		msvm_train->y = (double *)malloc(sizeof(double) * nSV);
		msvm_train->x = (struct svm_node **) malloc(nSV * sizeof(struct svm_node*));
			
		for (i = 0; i < nSV; i++) {
			msvm_train->x[i] = (struct svm_node*) malloc((n_features + 1) * sizeof(struct svm_node));
			memcpy(msvm_train->x[i], allSV[i], sizeof(struct svm_node) * (n_features + 1));
			msvm_train->y[i] = lab[i];
		}

		/*for (i = 0; i < nSV; i++) {
			temp = msvm_train->x[i];
			while (temp->index != -1) {
				cout << temp->value << " ";
				temp++;
			}
			cout << ":" << msvm_train->y[i];
			cout << endl;
		}*/
		msvm->learn(msvm_train);
		
		

		struct svm_problem * msvm_test = (struct svm_problem *)malloc(sizeof(struct svm_problem));
		
		msvm_test->l = this->trainer->getProblem()->l;
		msvm_test->y = (double *)malloc(sizeof(double) * msvm_test->l);
		msvm_test->x = (struct svm_node **) malloc(msvm_test->l * sizeof(struct svm_node*));
		
		for (i = 0; i < msvm_test->l; i++) {
			msvm_test->x[i] = (struct svm_node*) malloc((n_features + 1) * sizeof(struct svm_node));
			memcpy(msvm_test->x[i], x[i], sizeof(struct svm_node) * (n_features + 1));
			msvm_test->y[i] = 0;
		}
		
		/*for (i = 0; i < msvm_test->l; i++) {
			temp = msvm_test->x[i];
			while (temp->index != -1) {
				cout << temp->value << " ";
				temp++;
			}
			cout << ":" << msvm_test->y[i];
			cout << endl;
		}*/
		
		msvm->classify(msvm_test);
		
		double *cl = msvm->getClassification();

		labels.resize(this->trainer->getProblem()->l);
		
		for (i = 0; i < this->trainer->getProblem()->l; i++) {
			labels[i] = (int)cl[i];
		}

	/*	int nBSV = this->trainer->getModel()->lbounded;
		int *BSV_index = this->trainer->getModel()->BSV_index;
		double mindist, dist;
		int minsvindex;		
		for (int i = 0; i < nBSV; i++) {
			mindist = MAXFLOAT; 
			minsvindex = 0;
			for (int j = 0; j < msvm_test->l; j++) {
				if (j == BSV_index[i]) continue;	//	ignore if the point is the current BSV
				if (beta(j, true) == 1) continue;	//	ignore all BSVs
				dist = eucDist->calculateDistance(x[BSV_index[i]],x[j]);
				if (dist < mindist) {
					mindist = dist;
					minsvindex = j;
				}
			}
			labels[BSV_index[i]] = labels[minsvindex];
		}	
	*/
		
		free(msvm_train);
		free(msvm_test);
	}
}
