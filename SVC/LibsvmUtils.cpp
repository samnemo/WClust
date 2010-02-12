#include "stdafx.h"
#include "LibsvmUtils.h"

namespace damina
{
	
	LibsvmUtils::LibsvmUtils()
	{
	}
	
	LibsvmUtils::~LibsvmUtils()
	{
	}


	void LibsvmUtils::printPoint(struct svm_node *p) {
		struct svm_node *t;
		
		t = p;
		
		while (t->index != -1) {
			cout  << t->index << ":" << t->value << " ";
			t++;
		}
		cout << endl;
	}
}
