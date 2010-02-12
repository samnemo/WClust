#ifndef CLASSIFICATIONENGINE_H_
#define CLASSIFICATIONENGINE_H_


#include "LearningEngine.h"

namespace damina
{
	
	/**
	 *	A class which provides the basic interface for all classifiers.
	 * 	
	 * 	It acts like an interface (in UML or Java meaning) 
	 * 	for all classification engines we/you need to implement.
	 * 	It just force its children to implement two overloaded methods
	 * 	for the classification, that is the only one mandatory operation
	 * 	for a classifier. 
	 * 	
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * */
	class ClassificationEngine: public LearningEngine
	{
			
		public:
		
			virtual void classify() = 0;
			virtual void classify(DataSet *) = 0;
			
			ClassificationEngine();
			virtual ~ClassificationEngine();
	};

}

#endif /*CLASSIFICATIONENGINE_H_*/
