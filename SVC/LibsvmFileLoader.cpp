#include "stdafx.h"
#include "LibsvmFileLoader.h"
#include "KernelMachines.h"

namespace damina
{

	/**
	 * 	Constructor
	 * 	
	 * 	@param 	s The absolute pathname for the file to load
	 * */
	LibsvmFileLoader::LibsvmFileLoader(string s) {
		this->filename = s;
	}
	
	/**
	 * 	Destructor
	 * 	
	 * */
	LibsvmFileLoader::~LibsvmFileLoader() {
	}
	
	
	/**
	 * 	Loads the data from file in LibSVM format
	 * 
	 * 	@return	The svm_problem structure encapsulanting the loaded data
	 * */
	struct svm_problem *LibsvmFileLoader::load(bool precomputedKernel) {
		int elements, max_index, i, j;
		FILE *fp = fopen(this->filename.c_str(),"r");
		struct svm_problem *prob = (struct svm_problem *) malloc(sizeof(struct svm_problem));
		
		if (fp == NULL) {
			fprintf(stderr, "can't open input file %s\n", this->filename.c_str());
			fprintf(stderr, "reading from standard input...");
			fp = stdin;
			//return NULL;
		}
	
		prob->l = 0;
		elements = 0;
		while(1)
		{
			int c = fgetc(fp);
			switch(c)
			{
				case '\n':
					++(prob->l);
					// fall through,
					// count the '-1' element
				case ':':
					++elements;
					break;
				case EOF:
					goto out;
				default:
					;
			}
		}
	out:
		rewind(fp);
	
		prob->y = (double *) malloc(sizeof(double) * prob->l);
		prob->x = (struct svm_node **) malloc(sizeof(struct svm_node*) * prob->l);
		struct svm_node *x_space = (struct svm_node *) malloc(sizeof(struct svm_node) * elements);
	
		max_index = 0;
		j=0;
		for(i=0;i<prob->l;i++)
		{
			double label;
			prob->x[i] = &x_space[j];
			fscanf(fp,"%lf",&label);
			prob->y[i] = label;
	
			while(1)
			{
				int c;
				do {
					c = getc(fp);
					if(c=='\n') goto out2;
				} while(isspace(c));
				ungetc(c,fp);
				if (fscanf(fp,"%d:%lf",&(x_space[j].index),&(x_space[j].value)) < 2)
				{
					fprintf(stderr,"Wrong input format at line %d\n", i+1);
					return NULL;
				}
				++j;
			}	
	out2:
			if(j>=1 && x_space[j-1].index > max_index)
				max_index = x_space[j-1].index;
			x_space[j++].index = -1;
		}
		
		
	
//		if(param->gamma == 0)
//			param->gamma = 1.0/max_index; //max_index is the number of attributes in the input data.
//	
		if (precomputedKernel) {
			for (i = 0; i < prob->l; i++)
			{
				if (prob->x[i][0].index != 0)
				{
					fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
					return NULL;
				}
				if ((int)prob->x[i][0].value <= 0 || (int)prob->x[i][0].value > max_index)
				{
					fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
					return NULL;
				}
			}
		}
	
		fclose(fp);
		
		return prob;
	}

	struct svm_problem *LibsvmFileLoader::load(bool precomputedKernel, string s) {
		this->filename = s;
		return load(precomputedKernel);
	}
}
