#include "stdafx.h"
#include "SVClustering.h"
#include "L2Distance.h"

namespace damina
{

	/**
	 * 	Constructor
	 * 
	 * 	@param	train	The input data set
	 * */
	SVClustering::SVClustering(DataSet *train) {
		this->eucDist = new L2Distance();
		this->trainer = new OneClassSVM();
		this->trainer->setKernel(KernelType::gaussian);
		this->trainer->setCacheSize(100);
		this->trainer->setTolerance(0.00000001);
		this->trainer->setTrainingSet(train);
		this->trainer->setKernelWidth(initialKernelWidth());
		this->quadratic = NULL;
		this->clusterizeBSV = true;
	}
	
	/**
	 * 	Constructor
	 * 	
	 * 	@param	p	The input data in libsvm format
	 * */
	SVClustering::SVClustering(struct svm_problem *p) {
		this->eucDist = new L2Distance();
		this->trainer = new OneClassSVM();
		this->trainer->setKernel(KernelType::gaussian);
		this->trainer->setCacheSize(100);
		this->trainer->setTolerance(0.00000001);
		this->trainer->setProblem(p);
		this->trainer->setKernelWidth(initialKernelWidth());
		this->quadratic = NULL;
		this->clusterizeBSV = true;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	train	The input data set
	 * */
	SVClustering::SVClustering(double C, DataSet *train) {
		this->eucDist = new L2Distance();
		this->trainer = new OneClassSVM();
		this->trainer->setKernel(KernelType::gaussian);
		this->trainer->setCacheSize(100);
		this->trainer->setTolerance(0.00000001);
		
		this->trainer->setTrainingSet(train);
		
		this->trainer->setKernelWidth(initialKernelWidth());

		
		this->trainer->setNU((1 / (train->getSize() * C)));
		this->C = C;
		this->quadratic = NULL;
		this->clusterizeBSV = true;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	prob	The input data set in libsvm format
	 * */
	SVClustering::SVClustering(double C, struct svm_problem *prob) {
		this->eucDist = new L2Distance();
		this->trainer = new OneClassSVM();
		this->trainer->setKernel(KernelType::gaussian);
		this->trainer->setCacheSize(100);
		this->trainer->setTolerance(0.00000001);
		
		this->trainer->setProblem(prob);
		
		this->trainer->setKernelWidth(initialKernelWidth());
		
		this->trainer->setNU((1 / (prob->l * C)));
		this->C = C;
		this->quadratic = NULL;
		this->clusterizeBSV = true;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	w	The gaussian kernel width 
	 * 	@param	train	The input data set
	 * */
	SVClustering::SVClustering(double C, double w, DataSet *train) {
		this->eucDist = new L2Distance();
		this->trainer = new OneClassSVM();
		this->trainer->setKernel(KernelType::gaussian);
		this->trainer->setCacheSize(100);
		this->trainer->setTolerance(0.00000001);
		
		this->trainer->setTrainingSet(train);
		
		this->trainer->setNU((1 / (train->getSize() * C)));
		this->C = C;
		
		this->trainer->setKernelWidth(w);
		this->quadratic = NULL;
		this->clusterizeBSV = true;
	}
	
	/**
	 * 	Constructor
	 * 
	 * 	@param	C	The soft margin constant
	 * 	@param	w	The gaussian kernel width 
	 * 	@param	prob	The input data set in libsvm format
	 * */
	SVClustering::SVClustering(double C, double w, struct svm_problem *prob) {
		this->eucDist = new L2Distance();
		this->trainer = new OneClassSVM();
		this->trainer->setKernel(KernelType::gaussian);
		this->trainer->setCacheSize(100);
		this->trainer->setTolerance(0.00000001);
		
		this->trainer->setProblem(prob);
		
		this->trainer->setNU((1 / (prob->l * C)));
		this->C = C;
		
		this->trainer->setKernelWidth(w);
		this->quadratic = NULL;
		this->clusterizeBSV = true;
	}
		
	/**
	 * 	Destructor
	 * 
	 * */
	SVClustering::~SVClustering() {
		delete trainer;
		delete eucDist;
		// if (quadratic != NULL) free(quadratic);
	}

	/**
	 * 	The learning process. Here we find the Hypersphere in Feature Space
	 * */
	void SVClustering::learn() {
		free(quadratic);
		quadratic = NULL;
		this->numberOfClusters = -1;
		this->pointPerCluster.clear();
		this->labels.clear();
		this->trainer->learn();
	}

	/**
	 * 	The learning process. Here we find the Hypersphere in Feature Space
	 * 
	 * 	@param	train	The input data set
	 * */	
	void SVClustering::learn(DataSet *train) {
		free(quadratic);
		quadratic = NULL;
		this->numberOfClusters = -1;
		this->pointPerCluster.clear();
		this->labels.clear();
		this->trainer->learn(train);
	}
	
	/**
	 * 	The learning process. Here we find the Hypersphere in Feature Space
	 * 
	 * 	@param	p	The input data set in libsvm format
	 * */	
	void SVClustering::learn(struct svm_problem *p) {
		free(quadratic);
		quadratic = NULL;
		this->numberOfClusters = -1;
		this->pointPerCluster.clear();
		this->labels.clear();
		this->trainer->learn(p);
	}
	
		
	
	/**
	 * 	Returns the current value of the sphere radius
	 * 
	 * 	@return The value of the current sphere radius
	 * */
	double SVClustering::getSphereRadius() {
		calculateSphereRadius();
		return sphereRadius;
	}
	
	/**
	 * 	Set the input data set
	 * 
	 * 	@param	t	The input data set
	 * */
	void SVClustering::setTrainingSet(DataSet *t) {
		this->trainer->setTrainingSet(t);
	}
	
	/**
	 * 	Returns the current input data set
	 * 
	 * 	@return	The current input data set	
	 * */
	DataSet *SVClustering::getTrainingSet() {
		return this->trainer->getTrainingSet();
	}
	
	
	/**
	 * 	Set the input data set in libsvm format
	 * 
	 * 	@param	p	The input data set
	 * */
	void SVClustering::setProblem(struct svm_problem *p) {
		this->trainer->setProblem(p);
	}
	
	/**
	 * 	Returns the current input data set in the libsvm format
	 * 
	 * 	@return	The current input data set
	 * */
	struct svm_problem *SVClustering::getProblem() {
		return this->trainer->getProblem();
	}
	
	
	/**
	 * 	Returns the trained model
	 * 
	 * 	@return The trained model, with objective function, lagrangians, etc.
	 * */
	struct svm_model *SVClustering::getModel() {
		return this->trainer->getModel();
	}

	/**
	 *	Returns the SVM paramters
     * 
     *  @return The whole set of SVM parameters
	 * */
	struct svm_parameter *SVClustering::getParameters() {
		return trainer->getParameters();
	}
	
	/**
	 * 	Computes the initial gaussian kernel width, as proposed in
	 * 	
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, 
	 * 	"Support Vector Clustering," Journal of Machine 
	 * 	Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 * 	At this value, the soft margin constraint (C) have to be 1 (no outliers needed)
	 * 	and it is expected to yield a clustering result of one cluster.
	 * 
	 * */
	double SVClustering::initialKernelWidth() {
		struct svm_problem *p = this->trainer->getProblem();
		int N = this->trainer->getProblem()->l;
		
		double max = eucDist->calculateDistance(p->x[0], p->x[1]);
		double dist;
		
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				dist = eucDist->calculateDistance(p->x[i], p->x[j]);
				if (dist > max) {
					max = dist;
				}
			}
		}
		
		this->trainer->setKernelWidth(1/max);
		
		return (1/max);
	}
	
	
	/**
	 * 	Set the new width for Gaussian Kernel
	 * 
	 * 	@param	w	The kernel widht to set
	 * */
	void SVClustering::setKernelWidth(double w) {
		this->trainer->setKernelWidth(w);
	}
	
	
	/**
	 * 	Returns the current value of the Kernel Width
	 * 
	 * 	@return The current value of the Gaussian Kernel Width
	 * */
	double SVClustering::getKernelWidth() {
		return this->trainer->getKernelWidth();
	}

	/**
	 * 	Set the kernel type for the SVM.
	 * 	The values representing the Kernel Types are available in the 
	 * 	KernelType class as public static attributes.
	 * 
	 * 	@see KernelType
	 * 	
	 * 	@param	k	The integer value representing the Kernel Type
	 * */

	void SVClustering::setKernelType(int k) {
		this->trainer->setKernel(k);
	}	
	
	/**
	 * 	Returns the current kernel type
	 * 
	 * 	@return	The kernel type
	 * */

	int SVClustering::getKernelType() {
		return this->trainer->getKernel();
	}
		
	/**
	 * 	Set the new value for the Soft Margin Constant
	 * 
	 * 	@param	C	The soft margin constant value
	 * */
	void SVClustering::setSoftConstraint(double C) {
		this->C = C;
		
		/*	The OneClass SVM needs the NU param to be set in function of C param */
		this->trainer->setNU((1 / (this->trainer->getProblem()->l * C)));	
	}
	
	/**
	 * 	Returns the current value of the Soft Margin Constraint
	 * 
	 * 	@return	The value of current Soft Margin Constraint 
	 * */
	double SVClustering::getSoftConstraint() {
		return this->C;
	}


	/**
	 *	Return the estimate of a more appropriate Soft Margin parameter
	 *
	 *	@return The estimate of C parameter
	 * */
	double SVClustering::getSoftConstraintEstimate() {
		return C_estimation;
	}
	
	
	/**
	 * 	Run the clustering process.
	 * 
	 * 	In this case it runs the Step 2 of Support Vector Clustering, called "Cluster Labeling" by its authors.
	 * 
	 * 	The implementation reflects the first solution proposed in 
	 * 
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, 
	 * 	"Support Vector Clustering," Journal of Machine 
	 * 	Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 * 	namely, Complete Graph Cluster Labeling.
	 * 
	 * */
	void SVClustering::clusterize() {
		this->separateClusters();
	}
	
	/**
	 * 	Returns the clustering assignments
	 * 
	 * 	In the i-th position you can find the cluster wich contains the i-th input data point
	 * 
	 * 	@return	A vector encapsulating the clustering assignments
	 * */
	std::vector<int>& SVClustering::getClustersAssignment() {
		return labels;
	}
	
	/**
	 * 	Returns the number of points per cluster
	 * 
	 * 	The i-th position report the number of points for the i-th cluster
	 * 
	 * 	@return	A vector encapsulating the points per cluster count
	 * */
	std::vector<unsigned long int>& SVClustering::getPointPerCluster() {
		if (numberOfClusters > -1) {
			return pointPerCluster;
		}
		
		// calculating points per cluster and number of clusters detected
		
		// theoretically impossible
   		// here to detect some programming errors
   		if (labels.size() == 0) {
   			this->numberOfClusters = 0;
			return pointPerCluster;
   		}
		
   		std::vector<unsigned long int>::size_type idx;
		this->pointPerCluster.clear();
   		this->pointPerCluster.push_back(0);
   		this->numberOfClusters = 1;
    	for (idx = 0; idx < labels.size(); idx++) {
			if ((int)(pointPerCluster.size() - 1) < labels[idx]) {
    			this->numberOfClusters++;
    			this->pointPerCluster.push_back(1);
    		}
    		else {
    			this->pointPerCluster[labels[idx]]++;
    		}
    	}
			
		return pointPerCluster;
	}
	
	/**
	 * 	Returns the number of clusters found
	 * 
	 * 	@return 	An integer value which is the number of clusters found
	 * */
	unsigned long int SVClustering::getNumberOfClusters() {
		
		if (numberOfClusters > -1)
			return numberOfClusters;
		
		// calculating points per cluster and number of clusters detected
		
		// theoretically impossible
   		// here to detect some programming errors
   		if (labels.size() == 0) {
   			this->numberOfClusters = 0;
   			return numberOfClusters;
   		}
		
   		
   		std::vector<int>::size_type idx;
   		this->pointPerCluster.push_back(0);
   		this->numberOfClusters = 1;
    	for (idx = 0; idx < labels.size(); idx++) {
			if ((int)(pointPerCluster.size() - 1) < labels[idx]) {
    			this->numberOfClusters++;
    			this->pointPerCluster.push_back(1);
    		}
    		else {
    			this->pointPerCluster[labels[idx]]++;
    		}
    	}

		return numberOfClusters;
	}
	
	/**
	 * 	Returns the number of clusters containing more than one point
	 * 
	 * 	@return 	An integer value which is the number of non-singleton clusters
	 * */
	unsigned long int SVClustering::getNumberOfNonSingletonClusters() {
		return getNumberOfValidClusters(2);
	}

	/**
	 *	Returns the number of clusters with a number of elements greater than or equal to a certain threshold
	 *
	 *	@param	threshold	An integer value representing the minimum number of elements for a valid cluster
	 *	@return	An integer value which is the number of clusters requested
	 * */
	unsigned long int SVClustering::getNumberOfValidClusters(unsigned long int threshold) {
		getPointPerCluster();
		unsigned long int counter = 0;
		for (unsigned long int i = 0; i < pointPerCluster.size(); i++) {
			if (pointPerCluster[i] >= threshold) {
				counter++;
			}
		}
		return counter;
	}
		
	/**
	 * 	Returns the original classes assignments for the points in input
	 * 
	 * 	This could be contains the right assignments, but could be not too.
	 * 
	 * 	It depends on the input data file.
	 * 	If the input data file reports the right class for each point, then 
	 * 	you can find them here in this attribute. Otherwise not.
	 * 
	 *	@param	classOffset	The label of the first class in the original classification
	 *
	 * 	@return	The original classes assignments for the points in input
	 * */
	std::vector<int>& SVClustering::getOriginalClasses(int classOffset) {
		double *classes = this->trainer->getProblem()->y;
		unsigned long int i, N = this->trainer->getProblem()->l;
		
		originalClasses.resize(N);
		
		for (i = 0; i < N; i++) {
			originalClasses[i] = (int)classes[i] - classOffset;
		}
		
		return originalClasses;
	}
	

	/**
     *	Set a vector-distance measure valid for the Euclidean Space
	 *  
	 *  @param	ed	The instance of the distance
	 * */
	void SVClustering::setEuclideanDistance(EuclideanDistance *ed) {
		eucDist = ed;
	}	


	/**
	 *	Use a special clustering policy to clusterize BSVs
	 *	
	 * */
	void SVClustering::useSpecialClusteringPolicyForBSV() {
		this->clusterizeBSV = true;
	}
	
	/**
	 *	Use the classic clustering policy to clusterize BSVs
	 *	
	 * */
	void SVClustering::useClassicClusteringPolicyForBSV() {
		this->clusterizeBSV = false;
	}

	/**
	 *	Return true if the SVC is using the special policy to clusterize BSV
	 *	
	 *  @return	True if using special policy for BSV
	 * */
	bool SVClustering::usingSpecialClusteringPolicyForBSV() {
		return this->clusterizeBSV;
	}

	
	/*	PROTECTED METHODS FOLLOW	************************************************************************/
	
	
	/**
	 * 	Returns the i-th lagrangian multiplier.
	 * 
	 * 	The One Class SVM solves the Quadratic Programming problem and computes the lagrangians "beta".
	 * 	This method returns the i-th beta, multiplied by C, because LibSVM solves a scaled version
	 * 	of the One Class SVM problem.
	 * 	The betas equal to zero (non-support vector ones) are not in the solution.
	 * 
	 * 	@param	i	The index of the lagrangian
	 * 
	 * 	@return	The value of the i-th beta 
	 * */
	 double SVClustering::beta(int i) {
		return this->trainer->getModel()->sv_coef[0][i] * C; 
	 }
	 
	 	 
	 /**
	 * 	Returns the i-th lagrangian multiplier.
	 * 
	 * 	The One Class SVM solves the Quadratic Programming problem and computes the lagrangians "beta".
	 *	LibSVM solves a scaled version of the One Class SVM problem, so this method returns either the
	 * 	scaled value of a beta or the normal one, depeding on the 'scaled' param.
	 * 
	 * 	To obtain the normal value of a lagrangian, we multiply it by C.
	 * 
	 * 	The betas equal to zero (non-support vector ones) are not in the solution.
	 *	 
	 * 	@param	i		The index of the lagrangian
	 * 	@param	scaled	True if you want the scaled version, false otherwise
	 * 
	 * 	@return	The value of the i-th beta (scaled or not, depending on 'scaled' param)
	 * */
	double SVClustering::beta(int i, bool scaled) {
		if (scaled) {
			return this->trainer->getModel()->sv_coef[0][i];
		}
		else {
			return this->trainer->getModel()->sv_coef[0][i] * getSoftConstraint();
		}
	}
	
	 
	/**
	 * 	Returns the rho value of the decision function computed by One Class SVM.
	 * 
	 * 	The One Class SVM solves the Quadratic Programming problem and computes the rho value.
	 * 	This method returns such a value, multiplied by C, because LibSVM solves a scaled version
	 * 	of the One Class SVM problem
	 * 
	 * 	@return	The rho value of One Class SVM decision function
	 * */
	 double SVClustering::rho() {
	 	return this->trainer->getModel()->rho[0] * C;
	 }

	/**
	 * 	Returns the rho value of the decision function computed by One Class SVM.
	 * 
	 * 	The One Class SVM solves the Quadratic Programming problem and computes the rho value.
	 *	LibSVM solves a scaled version of the One Class SVM problem, so this method returns either the
	 * 	scaled value of rho or the normal one, depeding on the 'scaled' param.
	 * 	
	 * 	@param	scaled	True if you want the scaled version, false otherwise
	 *  
	 * 	@return	The rho value of One Class SVM decision function
	 * */
	 double SVClustering::rho(bool scaled) {
	 	if (scaled) {
	 		return this->trainer->getModel()->rho[0];
	 	}
	 	else {
	 		return this->trainer->getModel()->rho[0] * getSoftConstraint();
	 	}
	 }
	 
	
	/**
	 * 	Calculate the quadratic part of the point distance equation. 
	 * 
	 * 	
	 * 	Reference:
	 * 	 
	 *  <em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, "Support Vector Clustering," 
	 * 	Journal of Machine Learning Research, vol. 2, pp. 125-137, 2001.</em> 
	 * 
	 *	The Equation nr. 13 in the paper above is the distance of a point from the center of Sphere
	 * 
	 * 	In that equation, the quadratic part (sum over i, j beta_i beta_j K(x_i, x_j))
	 * 	is the same for all points, so we compute it once. 
	 * 
	 * 	Furthermore, we sum over support vectors only (including Bounded SVs), 
	 * 	because the Betas (lagrangian multipliers) are zero for non-SVs
	 * 		
	 */
	void SVClustering::calculateQuadraticPartOfDistanceFromCenter() {
		
		if (quadratic != NULL) {
			return;
		}
		
		struct svm_node **SV = this->trainer->getModel()->SV;
		int nSV = this->trainer->getModel()->l;
		double tmp = 0;
		
		
		for (int i = 0; i < nSV; i++) {
			for (int j = 0; j < nSV; j++) {
				tmp += beta(i) * beta(j) * kernelfunction(SV[i], SV[j], this->trainer->getModel()->param);
			}
		}
		
		this->quadratic = (double *)malloc(sizeof(double));
		*(this->quadratic) = tmp;
	} 
	 
	/**
	 * 	Calculate the distance of a point from the center of the sphere. 
	 * 
	 * 	
	 * 	Reference:
	 * 	 
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, "Support Vector Clustering," 
	 * 	Journal of Machine Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 *	The Equation nr. 13 in the paper above is the distance of a point from the center of Sphere
	 * 
	 * 
	 * 	We sum over support vectors only (including Bounded SVs), 
	 * 	because the Betas (lagrangian multipliers) are zero for non-SVs.
	 * 
	 * 	Furthermore, we replace K(x,x) with 1, because with the Gaussian Kernel, 
	 * 	K(x,x) = 1 for all x. 
	 * 
	 * 	@see calculateQuadraticPartOfDistanceFromCenter()
	 * 
	 * 	@param	x	The point
	 */
	double SVClustering::calculateDistanceFromCenter(struct svm_node *x) {
		double sum = 0;
		
		struct svm_node **SV = this->trainer->getModel()->SV;
		int nSV = this->trainer->getModel()->l;
		
		
		for (int i = 0; i < nSV; i++) {
			sum += beta(i) * kernelfunction(SV[i], x, this->trainer->getModel()->param);
		}
		
		
		calculateQuadraticPartOfDistanceFromCenter();
		
		// 1 is because (in case of RBF kernel used here) K(x,x) is equal to 1 
		// (therotically provable and pratically tested with libsvm) 
		//return sqrt(1 - (2 * sum) + *(this->quadratic));
		return 1 - (2 * sum) + *(this->quadratic);
	}
	
	/**
	 * 	Calculate the sphere radius 
	 * 
	 * 	
	 * 	Reference:
	 * 	 
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, "Support Vector Clustering," 
	 * 	Journal of Machine Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 *  Equation nr. 14 in the paper above is radius of the Sphere.
	 * 
	 * 	As suggested in
	 * 
	 * 	<em>J. Yang, V. Estivill-Castro, and S. K. Chalup, "Support vector clustering through 
	 * 	proximity graph modelling," in Neural Information Processing, 2002. ICONIP Ô02. 
	 * 	Proceedings of the 9th International Conference on, 2002, pp. 898-903.</em> 
	 * 
	 * 	we can use the average among SVs' distances from center to calculate the radius.
	 * 	
	 * 	However, theoretically, all the SVs must have the same distance from center, so 
	 * 	we have tested and found that using the distance of just one SV is a good approximation
	 * 	as much as to compute the aforementioned average.
	 * 
	 * 	This solution avoids to compute the distance for all SVs. 
	 * 
	 * */
	void SVClustering::calculateSphereRadius() {
		
		// first tecnique
		//
		// assuming SVs have all the same distanace from center (theoretically a correct assumption)
		// to avoid a check on the distance of all SVs
		//
		this->sphereRadius = this->calculateDistanceFromCenter(this->trainer->getModel()->SV[0]);
		
		//if (this->sphereRadius >= 1) {
		//	free(quadratic);
		//	this->sphereRadius = this->calculateDistanceFromCenter(this->trainer->getModel()->SV[0]);
		//}

		// second tecnique
		//
		// we can use the average among SVs' distances to calculate the radius
		
		//int nSV = this->trainer->getModel()->l;
		//this->sphereRadius = 0;
		//for (int i = 0; i < nSV; i++) {
		//	this->sphereRadius += this->calculateDistanceFromCenter(this->trainer->getModel()->SV[i]);			
		//}
		//this->sphereRadius = this->sphereRadius / nSV;

		//	third tecnique
		//	Using One Class SVM, Radius = sqrt(1 - rho/2)
		//sphereRadius = sqrt(1 - (rho()/2));
	}

	
	/**
	 *	Samples a point on the path between two points
	 * 
	 * 	@param	start	The first point
	 * 	@param	stop	The second point
	 * 	@param	interval	The interval of sampling for find out the point on the path 
	 * */
	struct svm_node *SVClustering::pointOnThePath(struct svm_node *start, struct svm_node *stop, double interval) {
		int start_dim = 0, stop_dim = 0;
		struct svm_node *t, *z;
		
		t = start;
		while (t->index != -1) {
			start_dim++;
			t++;
		}
		
		t = stop;
		while (t->index != -1) {
			stop_dim++;
			t++;
		}
		
		start_dim++;
		stop_dim++;
		
		
		if (start_dim >= stop_dim) {
			z = (struct svm_node *) malloc(sizeof(struct svm_node) * start_dim);
		}
		else {
			z = (struct svm_node *) malloc(sizeof(struct svm_node) * stop_dim);
		}

		t = z;
		
		struct svm_node *x = start;
		struct svm_node *y = stop;
				
		while(x->index != -1 && y->index !=-1) {
			if (x->index == y->index) {
				z->index = x->index;
				z->value = ((y->value - x->value) * interval) + x->value;
				x++;
				y++;
				z++;
			}
			else {
				if (x->index > y->index) {	
					z->index = y->index;
					z->value = y->value * interval; // (y->value - 0) * interval + 0 
					++y;
					++z;
				}
				else {
					z->index = x->index;
					z->value = -(x->value * interval) + x->value;  // (0 - x->value) * interval + x->value 
					++x;
					++z;
				}
			}
		}

		while(x->index != -1) {
			z->index = x->index;
			z->value = -(x->value * interval) + x->value;  // (0 - x->value) * interval + x->value
			++x;
			++z;
		}

		while(y->index != -1) {
			z->index = y->index;
			z->value = y->value * interval; // (y->value - 0) * interval + 0 
			++y;
			++z;
		}
		
		z->index = -1;
		z->value = 0;
		
		return t;
	}
	
	
	/**
	 * 	Run the clustering process.
	 * 
	 * 	In this case it runs the Step 2 of Support Vector Clustering, called "Cluster Labeling" by its authors.
	 * 
	 * 	The implementation reflects the first solution proposed in 
	 * 
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, 
	 * 	"Support Vector Clustering," Journal of Machine 
	 * 	Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 * 	namely, Complete Graph Cluster Labeling.
	 * 
	 * 	The implementation leaves BSVs unclassified.
	 * 
	 * 	We decide to assign BSVs to the cluster that they are closest to.
	 * 
	 * */
	void SVClustering::separateClusters() {
		static struct svm_node *z;
		bool flag;
		int i,j;
		double zdist;
		int N = this->trainer->getProblem()->l;
		
		std::pair<UndirectedGraph::edge_descriptor, bool> temp;
			
		UndirectedGraph aGraph(N);
		
		// calculating adj matrix
		
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				
				// if the j is adjacent to i, then all j adjacent's are also adjacent to i.
				if (j < i) {
					temp = edge(i, j, aGraph);
					if (temp.second == true) {
						for (int k = 0; k < N; k++) {
							if (edge(i, k, aGraph).second || edge(j, k, aGraph).second) {
								add_edge(i, k, aGraph);
							}
							else {
								remove_edge(i, k, aGraph);
							}
						}
					}		
				}
				else {
					// the i-th node is adiacent to itself - trivial
					if (i == j) {
						add_edge(i, j, aGraph);
						continue;
					}

					// we avoid to compute the same edge more than once					
					temp = edge(i, j, aGraph);
					if (temp.second == false) {
						flag = true;

						// 	sampling the point on the path between x[i] and x[j]
						//	usually	10-20 points are sampled in order to limit running time
						//	here 10 points are sampled
						for (double k = 1; k <= 10; k = k + 1) {
							z = pointOnThePath(this->trainer->getProblem()->x[i], this->trainer->getProblem()->x[j], k / 10);
							
							zdist = calculateDistanceFromCenter(z);

							if (zdist > this->getSphereRadius()) {
								flag = false;
								break;
							}
						}
						
						if (flag) {
							add_edge(i, j, aGraph);
							add_edge(j, i, aGraph);
						}
						else {
							remove_edge(i, j, aGraph);
							remove_edge(j, i, aGraph);
						}
					}
				}
			}
		}
			
		// finding connected components (and so the clusters)
		
		labels.resize(N);
   		connected_components(aGraph, &labels[0]);
   		
   		
   		// clusterize the BSVs
   		
   		int nBSV = this->trainer->getModel()->lbounded;
		int *BSV_index = this->trainer->getModel()->BSV_index;

		int minimizer;
		double min_BSV_distance, d;
		
		for (int i = 0; i < nBSV; i++) {
			min_BSV_distance = MAXFLOAT; 
			minimizer = 0;
			for (int j = 0; j < (int) labels.size(); j++) {
				if (j == BSV_index[i]) continue;	//	ignore if the point is the current BSV
				if (beta(j, true) == 1) continue;	//	ignore all BSVs
				
				d = eucDist->calculateDistance(this->trainer->getProblem()->x[BSV_index[i]], this->trainer->getProblem()->x[j]);
				if (d < min_BSV_distance) {
					min_BSV_distance = d;
					minimizer = i;
				}
			}
			labels[BSV_index[i]] = labels[minimizer];
		}
   		
	}
	
}
