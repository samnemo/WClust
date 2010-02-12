#ifndef LIBSVMUTILS_H_
#define LIBSVMUTILS_H_

/**
 * 	The LibsvmUtils header file includes a set of classes
 *  you can use to perform some useful and frequent actions.
 * 
 * */

#include "svm.h"
#include <iostream>

using namespace std;

namespace damina
{
	
	/**
	 * 	Provides utilties for Libsvm as public static methods.
	 * 
	 * 	This class provides a set of public static methods each of them implements
	 * 	an action, utility or something else of similar.
	 * 
	 * */
	class LibsvmUtils
	{
		public:
		
			/**
			 * 	Print a point in svm_node libsvm format
			 * */
			static void printPoint(struct svm_node *);
		
			LibsvmUtils();
			virtual ~LibsvmUtils();
	};

}

#endif /*LIBSVMUTILS_H_*/
