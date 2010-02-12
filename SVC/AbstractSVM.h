#ifndef SVM_H_
#define SVM_H_

#include "ClassificationEngine.h"	//	inherit from Classification Engine
#include "KernelMachines.h"			//	use Kernel Machines Utilies
#include "svm.h"					//	use LibSVM

namespace damina
{

	/**
	 * 	An abstract class which implements the basics for the Support Vector Machines.
	 * 
	 * 	The class was built around the LibSVM library: 
	 * 	<em>C. Chang and C. Lin, LIBSVM: A Library for Support Vector Machines, 2001.</em>
	 * 
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * */
	class AbstractSVM : public ClassificationEngine
	{
	
		protected: 
			/**
			 * 	The test set
			 * */
			DataSet *testSet;
			
			/**
			 * 	The training set
			 * */
			DataSet *trainingSet;
			
			/**
			 * 	The configuration parameters for set up an SVM using LibSVM
			 * */
			struct svm_parameter *params;
					
		public:
			
			virtual void setKernel(int);
			virtual void setKernel(int, int);
			virtual void setKernel(int, double);
			virtual void setKernel(int, int, double);
			
			virtual void setKernelWidth(double);
			virtual void setKernelDegree(int);
			
			virtual int getKernel();
			virtual double getKernelWidth();
			virtual int getKernelDegree();
			
			virtual void setTolerance(double);
			virtual double getTolerance();
			
			virtual void setCacheSize(int);
			virtual double getCacheSize();
			
			virtual void enableShrinkingHeuristics();
			virtual void disableShrinkingHeuristics();
			virtual bool isShrinkingHeuristicsEnabled();
			
			virtual void setTestSet(DataSet *);
			virtual void setTrainingSet(DataSet *);
			
			virtual DataSet *getTestSet();
			virtual DataSet *getTrainingSet();

			virtual struct svm_parameter *getParameters();
			AbstractSVM();
			virtual ~AbstractSVM();
	};

}

#endif /*SVM_H_*/
