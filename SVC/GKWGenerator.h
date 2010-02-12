#ifndef GKWGENERATOR_H_
#define GKWGENERATOR_H_

#include "SVClustering.h"
#include "EuclideanDistance.h"

namespace damina
{

	/**
	 * 	An implementation of the Secant Algorithm fot Gaussian Kernel Widht generation
	 * 
	 * 	
	 * 	This class implements the Gaussian Kernel Width Generator as proposed in
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Generator for Support Vector Clustering," 
	 * 	in Advances in Bioinformatics and Its Applications, 2005, pp. 151-162.</em>
	 * 
	 * 	and
	 * 
	 * 	<em>S. Lee and K. M. Daniels, "Gaussian Kernel Width Selection and 
	 * 	Fast Cluster Labeling for Support Vector Clustering," 
	 * 	Department of Computer Science, University of Massachussets Lowell, 2005.</em>
	 * 
	 * 
	 * 	Actually, this class does not implement the whole loop for the generation of the entire list
	 * 	of 'q' values, as described in aforesaid papers. This is why:
	 *  
	 * 
	 * 	1.	Generate the values before running the Support Vector Clustering is too expensive. 
	 * 		It's better generate the 'q' values contextually the running of the SVC
	 * 
	 * 	2.	Generate the 'q' values list a priori force us to generate a quite long list, with probably
	 * 		a number of values which will never be used
	 * 
	 * 
	 * 	Following the considerations above, we have developed this implementation that generate a new 
	 * 	kernel width value on demand, delegating the evaluation of the clustering results to some piece
	 * 	of software out of there. In this way we only generate the necessary number of 'q' values. Furthermore
	 * 	we do it contextually the running of the SVC, so we avoid to run the One Class SVM training algorithm
	 * 	more than we actually need.		 	  
	 * 
	 * */
	class GKWGenerator
	{
		private: 
			SVClustering *svc;
			
			EuclideanDistance *eucDist;

			double oldRadius;
			double newRadius;
			double oldKW;
			double newKW;
			double initialKW;

			struct svm_node *x_max_1, *x_max_2;
			
			int	runs;
			
			double eps;

			double softening;
			
			void forceMonotonicity();
			void intersectLines(double, double, double, double, double, double, double, double,	double &, double &);
			double maxDistance();
			double initialKernelWidth();
		public:
			
			virtual void setGrowthSoftening(double);
			virtual double getGrowthSoftening();
			virtual double getNextKernelWidthValue();
			virtual void setEuclideanDistance(EuclideanDistance *);
			virtual void reset();
		
		
			GKWGenerator(SVClustering *, double);
			GKWGenerator(SVClustering *, double, double);
			virtual ~GKWGenerator();
	};

}

#endif /*GKWGENERATOR_H_*/
