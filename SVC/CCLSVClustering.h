#ifndef CCLSVCLUSTERING_H_
#define CCLSVCLUSTERING_H_

#include "SVClustering.h"

namespace damina
{

	/**
	 * 	An implementation of the Support Vector Clustering originally proposed by Ben-Hur et al (2001), with a variation in the Cluster Labeling part.
	 * 
	 * 	This class implements the Support Vector Clustering originally proposed in 
	 * 
	 * 	<em>A. Ben-Hur, D. Horn, H. T. Siegelmann, and V. Vapnik, 
	 * 	"Support Vector Clustering," Journal of Machine 
	 * 	Learning Research, vol. 2, pp. 125-137, 2001.</em>
	 * 
	 * 	The Support Vector Clustering is among the few attempts to use the SVMs
	 * 	in an unsupervised way.
	 * 
	 * 	It consists mainly of two steps: 
	 * 
	 * 	1.	Cluster description <br />
	 *	======================= <br />
	 * 
	 * 	Find the Minimum Enclosing Sphere in the feature space, exactly as SVDD described in
	 * 
	 * 	<em>D. M. J. Tax and R. P. W. Duin, "Data Domain Description using Support Vectors," 
	 * 	in European Symposium on Artificial Neural Network, Bruges (Belgium), 1999.</em>
	 * 
	 * 	<em>D. M. J. TAX, "One-class classification: concept learning in the absence of counter-examples," 
	 * 	PhD Thesis , 2001.</em> 
	 * 
	 *	This leads to a Quadratic Programming problem like in the classic SVM training problem
	 * 
	 * 	This sphere, mapped back to the data space, split in various contours, each representing a cluster.
	 * 
	 * 	Here, we use One-class classification by Schoelkopf et al (2001) instead of SVDD by Tax.
	 * 
	 * 	The two methods are showed to be equivalent under following conditions: <br />
	 * 	a. The kernel must be Gaussian Kernel <br />
	 * 	b. The kernel width must be the same <br />
	 * 	c. The C parameter in SVDD must be equal to 1/(nu*N), where nu is the parameter in One-class SVM
	 * 		and N is the number of input elements. <br />
	 * 
	 * 
	 * 	The conditions a. is satisfied because the Support Vector Clustering formulation was based on the
	 * 	Gaussian Kernel (due the other kernels, like polynomial one, not describe the cluster 
	 *	boundaries well, as showed by Tax & Duin 1999)
	 * 
	 * 	 	
	 * 	2. Cluster labeling <br />
	 * 	=================== <br />
	 * 
	 * 	The cluster description algorithm does not differentiate between points that belong to differ- 
	 *	ent clusters. 
	 *
	 *	Here we replace the originally proposed cluster labeling algorithm with a novel and faster 
	 *	method proposed in
	 *
	 *	S. Lee and K. M. Daniels, "Cone Cluster Labeling for Support Vector Clustering," 
	 *  in Proceedings of 6th SIAM Conference on Data Mining, 2006, pp. 484-488.
	 * 
	 * 
	 * 	@see OneClassSVM
	 * 
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * 
	 * */
	class CCLSVClustering : public damina::SVClustering
	{
		
		private:
			
			
			
			// the following inline methods are unused in this child
			// their implementation is constrained by some abstract anchestors
			inline void clusterizeBSVs() {};
			inline struct svm_node *pointOnThePath(struct svm_node *, struct svm_node *, double) {return NULL;};
			
		protected:
			
			double zeta;
			
			virtual void calculateZeta();
			
			virtual void experimentalSeparateClusters();
			virtual void separateClusters();
			
				
		public:

			virtual void clusterize();
		
			CCLSVClustering(DataSet *);
			CCLSVClustering(struct svm_problem *);		
			CCLSVClustering(double, DataSet *);
			CCLSVClustering(double, struct svm_problem *);
			CCLSVClustering(double, double, DataSet *);
			CCLSVClustering(double, double, struct svm_problem *);

			virtual ~CCLSVClustering();
	};

}

#endif /*CCLSVCLUSTERING_H_*/
