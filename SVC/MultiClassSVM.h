#ifndef MULTICLASSSVM_H_
#define MULTICLASSSVM_H_

#include "AbstractSVM.h"
#include "svm.h"
#include <cstdlib>

namespace damina
{

	/**
	 *	An implementation of Multi-class SVM classifier.
	 * 
	 * 	References:
	 * 	
	 * 	V. N. Vapnik, The Nature of Statistical Learning Theory, 2nd ed., Springer, 2000.
	 *
	 * */
	class MultiClassSVM : public damina::AbstractSVM
	{
		private:

			/**
			 * 	The problem (Training Set) in the LibSVM format
			 * */
			struct svm_problem *problem;		
			
			/**
			 *	The test set in the LibSVM format
			 * */
			struct svm_problem *test_problem;

			/**
			 * 	The trained model.
			 * 	It contains also the computed lagrangian multipliers.
			 * */
			struct svm_model *model;
			
			virtual void trainingSetToProblem();
				
		public:
			
			virtual void learn();
			virtual void learn(DataSet *);
			virtual void learn(struct svm_problem *);
			
			virtual void classify();
			virtual void classify(DataSet *);
			virtual void classify(struct svm_problem *);
			
			virtual void setC(double);
			virtual double getC();

			virtual void setProblem(struct svm_problem *);
			virtual struct svm_problem *getProblem();
	
			virtual double *getClassification();

			virtual void setTrainingSet(DataSet *);
			
			virtual struct svm_model *getModel();
			
		
			MultiClassSVM();
			MultiClassSVM(DataSet *);
			MultiClassSVM(struct svm_problem *);
			virtual ~MultiClassSVM();
	};

}

#endif /*MULTICLASSSVM_H_*/
