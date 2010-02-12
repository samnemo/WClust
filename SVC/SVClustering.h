#ifndef SVCLUSTERING_H_
#define SVCLUSTERING_H_

#include <cmath>
#ifdef linux
	#include <values.h>
#endif

#include <iostream>

#include <boost/config.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

#include <boost/multi_array.hpp>

#include <algorithm>
#include <utility>

//#include <bits/stl_pair.h>
#include <set>

#include "ClusteringEngine.h"
#include "OneClassSVM.h"
#include "svm.h"
#include "EuclideanDistance.h"

#ifndef MINFLOAT
#define MINFLOAT 2.2250e-35f
#endif
#ifndef MAXFLOAT
#define MAXFLOAT 1.7976e35f
#endif

#ifndef MINDOUBLE
#define MINDOUBLE 2.2250e-308
#endif
#ifndef MAXDOUBLE
#define MAXDOUBLE 1.7976e308
#endif


namespace damina
{

	/**
	 * 	Boost library namespace
	 * */
	using namespace boost;
	
	/**
	 * 	Definition of and Undirected Graph as an Adjacency Matrix from the Boost Graph Library
	 * */
	typedef adjacency_matrix<undirectedS> UndirectedGraph;
	
	/**
	 * 	An implementation of the Support Vector Clustering originally proposed by Ben-Hur et al (2001)
	 * 
	 * 	This class implements the Support Vector Clustering as proposed in 
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
	 * 	a. The data point must be normalized to the unit norm vectors (implicitly done using the Gaussian Kernel) <br />
	 * 	b. The kernel width must be the same <br />
	 * 	c. The C parameter in SVDD must be equal to 1/(nu*N), where nu is the parameter in One-class SVM
	 * 		and N is the number of input elements. <br />
	 * 
	 * 
	 * 	2. Cluster labeling <br />
	 * 	=================== <br />
	 * 
	 * 	The cluster description algorithm does not differentiate between points that belong to differ- 
	 *	ent clusters. To do so, we use a geometric approach involving R(x), based on the following 
	 *	observation: given a pair of data points that belong to different components (clusters), any 
	 * 	path that connects them must exit from the sphere in feature space.	
	 * 
	 * 	So, we build a graph such that two vertex are connected if their images in feature space are
	 * 	connected by a path which lie entirely inside the sphere.
	 * 	The connected components algorithm on this graph returns the clusters.
	 * 
	 * 
	 * 	@see OneClassSVM
	 * 
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * 
	 * */
	class SVClustering : public damina::ClusteringEngine
	{
		
		private: 
			
			/**
			 *	
			 * */		
			bool clusterizeBSV;

			/**
			 * 	The number of clusters found
			 * */
			long int numberOfClusters;
			
			/**
			 * 	Number of points per cluster
			 * 
			 * 	The i-th position report the number of points for the i-th cluster
			 * */
			std::vector<unsigned long int> pointPerCluster;
			
			/**
			 * 	This could be contains the right assignments.
			 * 	It is not always true. It depends on the input data file.
			 * 	If the input data file report the right class for each point, then 
			 * 	you can find them here in this attribute. Otherwise not.
			 * */
			std::vector<int> originalClasses;
			
			
			/**
			 * 	The soft constraint in the original SV Clustering formulation. 
			 * 	The same "C" we can find in the SVDD formulation by Tax et al.
			 * */
			double C;

		
			/**
			 * 	The radius of the Sphere in the Feature space
			 * */
			double sphereRadius;	
			
			/**
			 * 	The quadratic part of the distance from the sphere's center 
			 * 	It's the same for all points
			 * */
			double *quadratic;
			
			
			void calculateQuadraticPartOfDistanceFromCenter();

			
			// the following inline methods are unused in this child
			// their implementation is constrained by some abstract anchestors
			virtual inline void clusterize(DataSet *) {};
			virtual inline void setTestSet(DataSet *) {};
			virtual inline DataSet *getTestSet() {return NULL;};


			
		protected:	
		
			/**
			 *	An estimation of a more appropriate C value, based on the heuristics proposed in the last paragraph of
			 *	
			 *	<em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Generator for Support Vector Clustering," in 
			 *	Advances in Bioinformatics and Its Applications, 2005, pp. 151-162 </em>.
			 *	
			 * */
			double C_estimation;
			
			
			/**
			 * 	The One Class SVM for the first step of the SV Clustering process
			 * */
			OneClassSVM *trainer;
		
			/**
			 * 	Clustering assignments.
			 * 	In the i-th position you can find the cluster wich contains the i-th input data point
			 * */
			std::vector<int> labels; 



			/**
			 *	Euclidean Measure for compute distance between vectors
			 *
			 * */
			EuclideanDistance *eucDist;	


			virtual double calculateDistanceFromCenter(struct svm_node *);
			
			virtual void calculateSphereRadius();
			
			
			virtual struct svm_node *pointOnThePath(struct svm_node *, struct svm_node *, double); 
			virtual void separateClusters();
			
				
			virtual double beta(int);
			virtual double beta(int, bool);
			
			virtual double rho();
			virtual double rho(bool); 
			

			
			
		public:
				
			virtual void learn();
			virtual void learn(DataSet *);
			virtual void learn(struct svm_problem *);
			
			virtual void clusterize();
			
			virtual void setTrainingSet(DataSet *);
			virtual DataSet *getTrainingSet();
			
			virtual void setProblem(struct svm_problem *);
			virtual struct svm_problem *getProblem();
			
			virtual struct svm_model *getModel();

			virtual struct svm_parameter *getParameters();

			virtual void setSoftConstraint(double);
			virtual double getSoftConstraint();
			
			virtual double getSoftConstraintEstimate();
			
			virtual double initialKernelWidth();
			virtual void setKernelWidth(double);
			virtual double getKernelWidth();

			virtual void setKernelType(int);
			virtual int getKernelType();
			
			virtual double getSphereRadius();

			
			virtual void setEuclideanDistance(EuclideanDistance *);


			virtual std::vector<int>& getClustersAssignment();
			virtual unsigned long int getNumberOfClusters();
			virtual unsigned long int getNumberOfNonSingletonClusters();
			virtual unsigned long int getNumberOfValidClusters(unsigned long int);
			virtual std::vector<unsigned long int>& getPointPerCluster();
			
			virtual std::vector<int>& getOriginalClasses(int);
			
			virtual void useSpecialClusteringPolicyForBSV();
			virtual void useClassicClusteringPolicyForBSV();
			virtual bool usingSpecialClusteringPolicyForBSV();

			SVClustering(DataSet *);
			SVClustering(struct svm_problem *);		
			SVClustering(double, DataSet *);
			SVClustering(double, struct svm_problem *);
			SVClustering(double, double, DataSet *);
			SVClustering(double, double, struct svm_problem *);
			
			virtual ~SVClustering();
	};

}

#endif /*SVCLUSTERING_H_*/
