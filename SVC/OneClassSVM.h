#ifndef ONECLASSSVM_H_
#define ONECLASSSVM_H_

#include "AbstractSVM.h"
#include "svm.h"
#include <cstdlib>

namespace damina
{

	/**
	 *	An implementation of One Class SVM.
	 * 
	 * 	The problem in one-class classification is to make a description of a target set of 
	 * 	objects and to detect which (new) objects resemble this training set. The difference with
	 * 	conventional classification is that in one-class classification only examples of one 
	 * 	class are available. The objects from this class will be called the target
	 * 	objects. All other objects are per definition the outlier objects.
	 * 
	 * 	One-class SVM was proposed by Schoelkopf et al. (2001) for estimating the support of 
	 *	a high-dimensional distribution and it is based on the nu-SVM variant by 
	 * 	Schoelkopf et al. (2000)
	 * 	
	 * 	References:
	 * 	
	 * 	B. Schoelkopf, J. C. Platt, J. Shawe-Taylor, A. J. Smola, and R. C. Williamson. 
	 *	Estimating the support of a high-dimensional distribution. Neural Computation, 
	 *	13(7):1443Ð1471, 2001. 
	 * 
	 * 	B. Schoelkopf, A. Smola, R. C. Williamson, and P. L. Bartlett. New support vector 
	 *	algorithms. Neural Computation, 12:1207Ð1245, 2000. 
	 * 
	 * */
	class OneClassSVM : public damina::AbstractSVM
	{
		private:

			/**
			 * 	The problem (Training Set) in the LibSVM format
			 * */
			struct svm_problem *problem;		
			
			/**
			 * 	The trained model.
			 * 	It contains also the computed lagrangian multipliers.
			 * */
			struct svm_model *model;
			
			virtual void trainingSetToProblem();
				
		public:
			virtual void setNU(double);
			virtual double getNU();
			
			virtual void learn();
			virtual void learn(DataSet *);
			virtual void learn(struct svm_problem *);
			
			virtual void classify();
			virtual void classify(DataSet *);
			
			virtual void setProblem(struct svm_problem *);
			virtual struct svm_problem *getProblem();
			
			virtual void setTrainingSet(DataSet *);
			
			virtual struct svm_model *getModel();
			
		
			OneClassSVM();
			OneClassSVM(DataSet *);
			OneClassSVM(struct svm_problem *);
			virtual ~OneClassSVM();
	};

}

#endif /*ONECLASSSVM_H_*/
