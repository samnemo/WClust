#ifndef LIBSVMFILELOADER_H_
#define LIBSVMFILELOADER_H_

#include "svm.h"
#include <string>

using namespace std;

namespace damina
{
	/**
	 * 	Implements a specific file loader for LibSVM structures.
	 * 
	 *	This class loads data from file in libsvm format and returns them encapsulated in the
	 * 	svm_problem structure from LibSVM.
	 *  
	 * 	@author	Vincenzo Russo - vincenzo.russo@neminis.org
	 * 
	 * */
	class LibsvmFileLoader
	{
		private: 
			/**
			 * 	Absolute pathname
			 * */
			string filename;
		
		public:
			struct svm_problem *load(bool);
			struct svm_problem *load(bool, string);
		
			LibsvmFileLoader(string);
			virtual ~LibsvmFileLoader();
	};

}

#endif /*LIBSVMFILELOADER_H_*/
