#include "stdafx.h"
#include "CCLSVClustering.h"
#include "LibsvmUtils.h"

namespace damina
{

	
	/**
	 * 	Constructor
	 * 
	 * 	@param	train	The input data set
	 * */
	CCLSVClustering::CCLSVClustering(DataSet *train): SVClustering(train) {
	}
	
	/**
	 * 	Constructor
	 * 	
	 * 	@param	p	The input data in libsvm format
	 * */
	CCLSVClustering::CCLSVClustering(struct svm_problem *p): SVClustering(p) {
		
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	train	The input data set
	 * */
	CCLSVClustering::CCLSVClustering(double C, DataSet *train): SVClustering(C, train) {
		
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	prob	The input data set in libsvm format
	 * */
	CCLSVClustering::CCLSVClustering(double C, struct svm_problem *p): SVClustering(C, p) {
		
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	w	The gaussian kernel width 
	 * 	@param	train	The input data set
	 * */
	CCLSVClustering::CCLSVClustering(double C, double w, DataSet *train): SVClustering(C, w, train) {
		
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	w	The gaussian kernel width 
	 * 	@param	prob	The input data set in libsvm format
	 * */
	CCLSVClustering::CCLSVClustering(double C, double w, struct svm_problem *p): SVClustering(C, w, p) {
		
	}
	
	
	/**
	 * 	Destructor
	 * 
	 * */
	CCLSVClustering::~CCLSVClustering()	{
		 
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
	 * 	namely, Cone Cluster Labeling
	 * 
	 * */
	void CCLSVClustering::clusterize() {
		this->separateClusters();
		//this->experimentalSeparateClusters();
	}

	
	/*	PRIVATE METHODS FOLLOW	************************************************************************/
	
	/**
	 * 	Calculate the Zeta, i.e. the radius of the SV-centered spheres in data space, 
	 * 	corresponding to Support Vector Cones in features space, as described in
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Cone Cluster Labeling for Support Vector Clustering," 
	 * 	in Proceedings of 6th SIAM Conference on Data Mining, 2006, pp. 484-488.</em> 
	 *  
	 * */
	void CCLSVClustering::calculateZeta() {
		double R = getSphereRadius();
		double q = getKernelWidth();
		
		//double sqrt_1_R = sqrt(1 - R*R);
		double sqrt_1_R = sqrt(1 - R);
		double ln = log(sqrt_1_R);

		this->zeta = sqrt(-(ln/q));
		//this->zeta = -(ln/q);
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
	void CCLSVClustering::separateClusters() {
		
		// compute Zeta
		calculateZeta();
		
		int i,j;		//	to run over the SVs 
			
		int nSV = this->trainer->getModel()->l - this->trainer->getModel()->lbounded;
		int *SV_index = this->trainer->getModel()->SV_index;

		int nBSV = this->trainer->getModel()->lbounded;	
		int *BSV_index = this->trainer->getModel()->BSV_index;

		
		/*	original dataset */
		struct svm_node **x = this->trainer->getProblem()->x;
		double dist;
		
		
		/* arithmetic mean of all betas */
		double betas_average = 0;
		double beta_max = -1;
		
		/*	adj matrix only for real SVs */
		UndirectedGraph adj(nSV);
		for (i = 0; i < nSV; i++) {
			betas_average += beta(i, false);							// 	sum all betas
			if (beta(i, false) > beta_max) beta_max = beta(i, false);	//	max beta

			for (j = 0; j < nSV; j++) {
				
				//	trivial: each node is connected to itself
				if (i == j) {
					add_edge(i, j, adj);
					continue;
				}
				
				// avoid to set the same edge more than once
				if (edge(i, j, adj).second) continue;
				
				
				dist = eucDist->calculateDistance(x[SV_index[i]], x[SV_index[j]]);
				
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
		betas_average = betas_average / nSV;	//	divide the sum of all betas by numer of betas => arithmetic mean
		
		
		// clustering SVs finding connected components of the adj matrix just built
		std::vector<int> svLabels(nSV);
		connected_components(adj, &svLabels[0]);
		
		
		// total points
		int N = this->trainer->getProblem()->l;
		
		// total labels
		labels.resize(N);

		double mindist;
		int minsvindex;
		
		C_estimation = 10.0 * getSoftConstraint() / N;	




		// clustering points other than SVs
		// for each point in the input data set 
		for (i = 0; i < N; i++) {
			mindist = MAXFLOAT; 
			minsvindex = -1;
			for (j = 0; j < nSV; j++) {	// we find the nearest SV
			
				if (i == SV_index[j]) {	//	trivial if the point is the current SV
					minsvindex = j;
					break;
				}
				
				dist = eucDist->calculateDistance(x[SV_index[j]], x[i]);
				if (dist < mindist) {	//	if the distance between a point and an SV is less or equal than zeta
										//	and it is also less than the last distance found 
				//if ((dist < mindist) && (dist <= zeta)) {
					mindist = dist;
					minsvindex = j;
				}  
			}

			labels[i] = svLabels[minsvindex];
			
			/*if (minsvindex == -1) {
				mindist = MAXFLOAT; 
				minsvindex = -1;
				for (j = 0; j < N; j++) {
					if (i != j) {
						dist = eucDist->calculateDistance(x[j], x[i]);
						if (dist < mindist) {
							mindist = dist;
							minsvindex = j;
						}
					}	
				}
				labels[i] = labels[minsvindex];
			}
			else {
				labels[i] = svLabels[minsvindex];
			}*/
		}

		/*for (i = 0; i < nSV; i++) {
			cout << "T " << labels[SV_index[i]] << " ";
			LibsvmUtils::printPoint(x[SV_index[i]]);	
		}

		for (i = 0; i < nBSV; i++) {
			cout << "T " << labels[BSV_index[i]] << " ";
			LibsvmUtils::printPoint(x[BSV_index[i]]);	
		}*/
	
		if (usingSpecialClusteringPolicyForBSV()) {
				// try to cluster BSV points
				int nBSV = this->trainer->getModel()->lbounded;
				int *BSV_index = this->trainer->getModel()->BSV_index;
				
				for (i = 0; i < nBSV; i++) {
					mindist = MAXFLOAT; 
					minsvindex = 0;
					for (j = 0; j < N; j++) {
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
		}
	}
	
	
	void CCLSVClustering::experimentalSeparateClusters() {
		// compute Zeta
		calculateZeta();

		double ker_zeta = exp(-1 * getKernelWidth() * zeta);
		
		int i,j;		//	to run over the SVs 
			
		int nSV = this->trainer->getModel()->l - this->trainer->getModel()->lbounded;
		int *SV_index = this->trainer->getModel()->SV_index;
		
		
		/*	original dataset */
		struct svm_node **x = this->trainer->getProblem()->x;
		double dist;
		
		
		/* arithmetic mean of all betas */
		double betas_average = 0;
		double beta_max = -1;
		
		/*	adj matrix only for real SVs */
		UndirectedGraph adj(nSV);
		for (i = 0; i < nSV; i++) {
			betas_average += beta(i, false);							// 	sum all betas
			if (beta(i, false) > beta_max) beta_max = beta(i, false);	//	max beta

			for (j = 0; j < nSV; j++) {
				
				//	trivial: each node is connected to itself
				if (i == j) {
					add_edge(i, j, adj);
					continue;
				}
				
				// avoid to set the same edge more than once
				if (edge(i, j, adj).second) continue;
				
				
				//dist = eucDist->calculateDistance(x[SV_index[i]], x[SV_index[j]]);
				dist = 2 - 2 * kernelfunction(x[SV_index[i]], x[SV_index[j]], *(getParameters()));

				// 	zeta is the radius of the hyperspheres in data space centered in the SVs
				//	(all hyperspheres have the same radius)
				//	Two SVs are connected if their hyperspheres overlap, i.e. if the 
				//	distance between two SVs are at most 2*zeta (overlap in 1 point)
				if (dist <=  2 * ker_zeta) {		  
					add_edge(i, j, adj);
					add_edge(j, i, adj);
				}
				else {
					remove_edge(i, j, adj);
					remove_edge(j, i, adj);
				}
			}
		}
		betas_average = betas_average / nSV;	//	divide the sum of all betas by numer of betas => arithmetic mean
		
		
		// clustering SVs finding connected components of the adj matrix just built
		std::vector<int> svLabels(nSV);
		connected_components(adj, &svLabels[0]);
		
		
		// total points
		int N = this->trainer->getProblem()->l;
		
		// total labels
		labels.resize(N);

		double mindist;
		int minsvindex;
		
		C_estimation = 10.0 * getSoftConstraint() / N;	

		// clustering points other than SVs
		// for each point in the input data set 
		for (i = 0; i < N; i++) {
			mindist = MAXFLOAT; 
			minsvindex = -1;
			for (j = 0; j < nSV; j++) {	// we find the nearest SV
			
				if (i == SV_index[j]) {	//	trivial if the point is the current SV
					minsvindex = j;
					break;
				}
				
				//dist = eucDist->calculateDistance(x[SV_index[j]], x[i]);

				dist = 2 - 2 * kernelfunction(x[SV_index[j]], x[i], *(getParameters()));

				//if (dist < mindist) {	//	if the distance between a point and an SV is less or equal than zeta
										//	and it is also less than the last distance found 
				if ((dist < mindist) && (dist <= ker_zeta)) {
					mindist = dist;
					minsvindex = j;
				}  
			}

			//labels[i] = svLabels[minsvindex];
			if (minsvindex == -1) {
				mindist = MAXFLOAT; 
				minsvindex = -1;
				for (j = 0; j < N; j++) {
					if (i != j) {
						dist = 2 - 2 * kernelfunction(x[i], x[j], *(getParameters()));
						//dist = eucDist->calculateDistance(x[j], x[i]);
						if (dist < mindist) {
							mindist = dist;
							minsvindex = j;
						}
					}	
				}
				labels[i] = labels[minsvindex];
			}
			else {
				labels[i] = svLabels[minsvindex];
			}
		}

		if (usingSpecialClusteringPolicyForBSV()) {
				// try to cluster BSV points
				int nBSV = this->trainer->getModel()->lbounded;
				int *BSV_index = this->trainer->getModel()->BSV_index;
				
				for (i = 0; i < nBSV; i++) {
					mindist = MAXFLOAT; 
					minsvindex = 0;
					for (j = 0; j < N; j++) {
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
		}
	
	}
}
