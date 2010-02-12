#include "stdafx.h"
#include "apcluster.h"
/* C-implementation of the affinity propagation clustering algorithm. See */
/* BJ Frey and D Dueck, Science 315, 972-976, Feb 16, 2007, for a         */
/* description of the algorithm.                                          */
/*                                                                        */
/* Copyright 2007, BJ Frey and Delbert Dueck. This software may be freely */
/* used and distributed for non-commercial purposes.                      */

#include <stdio.h>
#include <stdlib.h>
/*#include <values.h>*/

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

//#if sizeof(APNUM) == sizeof(double)
  #define MINAPNUM MINDOUBLE
  #define MAXAPNUM MAXDOUBLE
//#else
//  #define MINAPNUM MINFLOAT
//  #define MAXAPNUM MAXFLOAT
//#endif

void PrintAPClustUsage()
{
    printf("\nUsage:\n\n");
    printf("apcluster <Similarity file> <Preference file> <Output: Index file>\n");
    printf("          [ <maxits> <convits> <dampfact> ]\n\n");
    printf("\nAPCLUSTER uses the affinity propagation algorithm by\n");
    printf("BJ Frey and D Dueck (Science 315, 972-976, Feb 16, 2007)\n");
    printf("to identify data clusters, using a set of real-valued\n");
    printf("pair-wise data point similarities as input. Each cluster is\n");
    printf("represented by a data point called a cluster center, and the\n");
    printf("method searches for clusters so as to maximize a fitness\n");
    printf("function called net similarity. The method is iterative and\n");
    printf("stops after maxits iterations (default: 500) or when the cluster\n");
    printf("centers stay constant for convits iterations (default: 50).\n\n");
    printf("For N data points, there may be as many as N^2-N pair-wise\n");
    printf("similarities (note that the similarity of data point i to k\n");
    printf("need not be equal to the similarity of data point k to i).\n");
    printf("APCLUSTER can work with this full set of similarities or\n");
    printf("a smaller subset (which is helpful when N is large). The\n");
    printf("similarities should be in the input similarity file, where\n");
    printf("each line should be of the form <i k s> where i and k are\n");
    printf("data point indices and s is a real number corresponding\n");
    printf("to the similarity of data point i to data point k.\n\n");
    printf("APCLUSTER automatically determines the number of clusters,\n");
    printf("based on numbers called preferences -- there is one such\n");
    printf("number for each data point and data points with large\n");
    printf("preferences are more likely to be chosen as centers. Values\n");
    printf("of the prefences should be in the input preference file and\n");
    printf("the jth entry in this file should be the preference of the\n");
    printf("jth data point. How should you choose the preferences? A good\n");
    printf("choice is to set all preference values to the median of the\n");
    printf("similarity values. Then, the number of identified clusters can\n");
    printf("be increased or decreased  by changing this value accordingly.\n\n");
    printf("The fitness function (net similarity) used to search for\n");
    printf("solutions equals the sum of the preferences of the the data\n");
    printf("centers plus the sum of the similarities of the other data\n");
    printf("points to their cluster centers.\n\n");
    printf("The identified cluster centers and the assignments of other\n");
    printf("data points to these centers are stored in the output index\n");
    printf("file. The jth entry in this file is the index of the data\n");
    printf("point that is the cluster center for data point j. If the\n");
    printf("jth entry equals j, then data point j is itself a cluster\n");
    printf("center. The sum of the similarities of the data points to\n");
    printf("their cluster centers, the  sum of the preferences of the\n");
    printf("identified cluster centers, and the fitness or net similarity\n");
    printf("(sum of the data point similarities and preferences) are\n");
    printf("printed to the screen.\n\n");
    printf("See http://www.psi.toronto.edu/affinitypropagation for test\n");
    printf("files and MATLAB software.\n\n");
    printf("Copyright 2007, BJ Frey and D Dueck. This software may be\n");
    printf("freely distributed and used for non-commercial purposes.\n\n");
}

//void apcluster_main(int argc, char** argv,vector<int>& vIDs,const CUPDUPDATA* pUp)
bool apcluster_main(vector<APNUM>& s,vector<unsigned long>& i,vector<unsigned long>& k,int maxits,int convits,APNUM lam,int n,vector<int>& vIDs,const CUPDUPDATA* pUp)
{
	vIDs = vector<int>(0);

	int /*flag,*/ dn, it, conv, decit/*, maxits, convits*/;
	unsigned long i1, /*i2,*/ j, /**i, *k,*/ m, /*n,*/ /***dec, *decsum, *idx,*/ K;
	APNUM tmp, /* *s, *a, *r, *mx1, *mx2, *srp,*/ netsim, dpsim, expref;//, lam;
	//FILE *f;

	vector<APNUM> a,r,mx1,mx2,srp;
	vector<unsigned long> decsum,idx;
	vector< vector<unsigned long> > dec;

	/* Usage */
	/*if((argc!=4)&&(argc!=7)){
		PrintAPClustUsage();
		return;
	}*/

	if (MINAPNUM==0.0) {
		printf("There are numerical precision problems on this architecture.  Please recompile after adjusting MIN_DOUBLE and MAX_DOUBLE\n\n");
		return false;
	}

	/* Parse command line */
	/*if(argc==4)
	{ 
		lam=0.5; maxits=500; convits=50;
	} 
	else 
	{
		flag=sscanf(argv[4],"%d",&maxits);
		if(flag==1)
		{
			flag=sscanf(argv[5],"%d",&convits);
			if(flag==1)
			{
				flag=sscanf(argv[6],"%lf",&lam);
				if(flag==0)
				{
					printf("\n\n*** Error in <damping factor> argument\n\n");
					return;
				}
			} 
			else 
			{
				printf("\n\n*** Error in <convergence iterations> argument\n\n");
				return;
			}
		} 
		else 
		{
			printf("\n\n*** Error in <maximum iterations> argument\n\n");
			return;
		}
	}*/
	if(maxits<1)
	{
		printf("\n\n*** Error: maximum number of iterations must be at least 1\n\n");
		return false;
	}
	if(convits<1)
	{
		printf("\n\n*** Error: number of iterations to test convergence must be at least 1\n\n");
		return false;
	}
	if((lam<0.5)||(lam>=1))
	{
		printf("\n\n*** Error: damping factor must be between 0.5 and 1\n\n");
		return false;
	}
	printf("\nmaxits=%d, convits=%d, dampfact=%lf\n",maxits,convits,lam);


	/* Find out how many data points and similarities there are */
	/*f=fopen(argv[1],"r");
	if(f==NULL)
	{
		printf("\n\n*** Error opening similarities file\n\n");
		return;
	}*/

	//this loop determines # of points and # of similarities
	//n is # of "points"
	//m is # of similiarities
	//m=0; //n=0;
	/*flag=fscanf(f,"%lu %lu %lf",&i1,&i2,&tmp);
	pUp->SetProgress("Reading similarities",10);
	while(flag!=EOF)
	{
		if(i1>n) n=i1;
		if(i2>n) n=i2;
		m=m+1;
		flag=fscanf(f,"%lu %lu %lf",&i1,&i2,&tmp);
	}
	fclose(f);*/

//	n = iPoints;
	m = i.size() - n; //# of points - # of preferences = # of similarity entries

	/* Allocate memory for similarities, preferences, messages, etc */

	//index of points , for similarities and then preferences
	//i=(unsigned long *)calloc(m+n,sizeof(unsigned long));

	//index of points , for similarities and then preferences
	//k=(unsigned long *)calloc(m+n,sizeof(unsigned long));

	//similarities. after m similarities there are n preferences stored
	//1 for each point
	//s=(double *)calloc(m+n,sizeof(double));

	//s[j] is similarity from point i[j] to k[j]
	//this should be passed in to function, instead
	//of being read from file


	a = vector<APNUM>(m+n);
	//a=(double *)calloc(m+n,sizeof(double));

	//responsibilities
	//r=(double *)calloc(m+n,sizeof(double));
	r = vector<APNUM>(m+n);

	mx1 = vector<APNUM>(n);
	mx2 = vector<APNUM>(n);
	srp = vector<APNUM>(n);
	
	//mx1=(double *)calloc(n,sizeof(double));
	//mx2=(double *)calloc(n,sizeof(double));
	//srp=(double *)calloc(n,sizeof(double));

	dec = vector< vector<unsigned long> >(convits, vector<unsigned long>(n));
	
	//dec=(unsigned long **)calloc(convits,sizeof(unsigned long *));
	//for(j=0;j<convits;j++)
	//	dec[j]=(unsigned long *)calloc(n,sizeof(unsigned long));
	
	decsum = vector<unsigned long>(n);
	idx = vector<unsigned long>(n);
	//decsum=(unsigned long *)calloc(n,sizeof(unsigned long));
	//idx=(unsigned long *)calloc(n,sizeof(unsigned long));

	/* Read similarities and preferences */
	/*f=fopen(argv[1],"r");
	for(j=0;j<m;j++)
	{
		if(j%5000==0)
		{
			if(pUp->ShouldTerminate()) break;
			pUp->SetProgress("Reading similarities and preferences", 99.0 * j / m);
		}

		fscanf(f,"%lu %lu %lf",&(i[j]),&(k[j]),&(s[j]));
		i[j]--; k[j]--;
	}
	fclose(f);*/
	
	if(pUp->ShouldTerminate()) return true;

	//read preferences, 1 for each point
	/*f=fopen(argv[2],"r");
	if(f==NULL)
	{
		pUp->SetProgress("*** Error opening preferences file",0);
		goto freememory;
		return;
	}*/
	/*for(j=0;j<n && !pUp->ShouldTerminate();j++)
	{
		pUp->SetProgress("Reading preferences", 99.0 * j / n);		

		i[m+j]=j; k[m+j]=j;//index of preferences, same index for i,k

		//preferences is stored in s pointer after similarity values
		flag=fscanf(f,"%lf",&(s[m+j]));
	}
	fclose(f);*/

	if(pUp->ShouldTerminate()) return true;

	/*if(flag==EOF)
	{
		pUp->SetProgress("*** Error: Number of entries in the preferences file is less than number of data points",0);
		goto freememory;
	}*/
	m=m+n;

	/* Include a tiny amount of noise in similarities to avoid degeneracies */
	for(j=0;j<m;j++) s[j]=s[j]+(1e-16*s[j]+MINAPNUM*100)*(rand()/((APNUM)RAND_MAX+1));

	/* Initialize availabilities to 0 and run affinity propagation */
	for(j=0;j<m;j++) 
		a[j]=0.0;
	for(j=0;j<convits;j++) 
		for(i1=0;i1<n;i1++) 
			dec[j][i1]=0;
	for(j=0;j<n;j++) 
		decsum[j]=0;
	
	dn=0; it=0; decit=convits;

	if(pUp->ShouldTerminate()) return true;

	char msg[1024];
	
	while(dn==0)
	{
		if(pUp->ShouldTerminate()) return true;

		it++; /* Increase iteration index */

		sprintf(msg,"AP iteration %d of %d",it,maxits);
		pUp->SetProgress(msg,99.0 * it / maxits);

		/* Compute responsibilities */
		for(j=0;j<n;j++)
		{ 
			mx1[j]=-MAXAPNUM; 
			mx2[j]=-MAXAPNUM;
		}
		for(j=0;j<m;j++)
		{
			tmp=a[j]+s[j];
			if(tmp>mx1[i[j]])
			{
				mx2[i[j]]=mx1[i[j]];
				mx1[i[j]]=tmp;
			}
			else if(tmp>mx2[i[j]]) 
				mx2[i[j]]=tmp;
		}
		for(j=0;j<m;j++)
		{
			tmp=a[j]+s[j];
			if(tmp==mx1[i[j]])
				r[j]=lam*r[j]+(1-lam)*(s[j]-mx2[i[j]]);
			else 
				r[j]=lam*r[j]+(1-lam)*(s[j]-mx1[i[j]]);
		}

		/* Compute availabilities */
		for(j=0;j<n;j++) 
			srp[j]=0.0;
		for(j=0;j<m-n;j++) 
			if(r[j]>0.0) 
				srp[k[j]]=srp[k[j]]+r[j];
		for(j=m-n;j<m;j++)
			srp[k[j]]=srp[k[j]]+r[j];
		for(j=0;j<m-n;j++)
		{
			if(r[j]>0.0) 
				tmp=srp[k[j]]-r[j]; 
			else 
				tmp=srp[k[j]];
			if(tmp<0.0) 
				a[j]=lam*a[j]+(1-lam)*tmp;
			else 
				a[j]=lam*a[j];
		}
		for(j=m-n;j<m;j++) 
			a[j]=lam*a[j]+(1-lam)*(srp[k[j]]-r[j]);

		/* Identify exemplars and check to see if finished */
		decit++; 
		if(decit>=convits) 
			decit=0;
		for(j=0;j<n;j++) 
			decsum[j]=decsum[j]-dec[decit][j];
		for(j=0;j<n;j++)
			if(a[m-n+j]+r[m-n+j]>0.0) 
				dec[decit][j]=1; 
			else 
				dec[decit][j]=0;
		K=0; 
		for(j=0;j<n;j++) 
			K=K+dec[decit][j];
		for(j=0;j<n;j++) 
			decsum[j]=decsum[j]+dec[decit][j];
		if((it>=convits)||(it>=maxits))
		{
			/* Check convergence */
			conv=1; 
			for(j=0;j<n;j++) 
				if((decsum[j]!=0)&&(decsum[j]!=convits)) 
					conv=0;
			/* Check to see if done */
			if(((conv==1)&&(K>0))||(it==maxits)) dn=1;
		}
	}
	/* If clusters were identified, find the assignments and output them */
	if(K>0)
	{
		pUp->SetProgress("Saving results",0);

		for(j=0;j<m;j++)
			if(dec[decit][k[j]]==1) 
				a[j]=0.0;
			else 
				a[j]=-MAXAPNUM;
		for(j=0;j<n;j++) 
			mx1[j]=-MAXAPNUM;
		for(j=0;j<m;j++)
		{
			tmp=a[j]+s[j];
			if(tmp>mx1[i[j]])
			{
				mx1[i[j]]=tmp;
				idx[i[j]]=k[j];
			}
		}
		for(j=0;j<n;j++) 
			if(dec[decit][j]) 
				idx[j]=j;
		for(j=0;j<n;j++) 
			srp[j]=0.0;
		for(j=0;j<m;j++) 
			if(idx[i[j]]==idx[k[j]]) 
				srp[k[j]]=srp[k[j]]+s[j];
		for(j=0;j<n;j++) 
			mx1[j]=-MAXAPNUM;
		for(j=0;j<n;j++) 
			if(srp[j]>mx1[idx[j]]) 
				mx1[idx[j]]=srp[j];
		for(j=0;j<n;j++)
			if(srp[j]==mx1[idx[j]]) 
				dec[decit][j]=1; 
			else 
				dec[decit][j]=0;
		for(j=0;j<m;j++)
			if(dec[decit][k[j]]==1) 
				a[j]=0.0; 
			else 
				a[j]=-MAXAPNUM;
		for(j=0;j<n;j++) 
			mx1[j]=-MAXAPNUM;
		for(j=0;j<m;j++)
		{
			tmp=a[j]+s[j];
			if(tmp>mx1[i[j]])
			{
				mx1[i[j]]=tmp;
				idx[i[j]]=k[j];
			}
		}
		for(j=0;j<n;j++) 
			if(dec[decit][j]) 
				idx[j]=j;
		//f=fopen(argv[3],"w");
		vIDs = vector<int>(n);//output
		for(j=0;j<n;j++) 
		{
//			pUp->SetProgress("Saving results", 99.0 * j / n);

		//	fprintf(f,"%lu\n",idx[j]+1);
			vIDs[j] = idx[j]+1;
		}
		//fclose(f);
		dpsim=0.0; 
		expref=0.0;
		for(j=0;j<m;j++)
		{
			if(idx[i[j]]==k[j])
			{
				if(i[j]==k[j]) 
					expref=expref+s[j];
				else 
					dpsim=dpsim+s[j];
			}
		}
		netsim=dpsim+expref;
		printf("\nNumber of identified clusters: %d\n",K);
		printf("Fitness (net similarity): %f\n",netsim);
		printf("  Similarities of data points to exemplars: %f\n",dpsim);
		printf("  Preferences of selected exemplars: %f\n",expref);
		printf("Number of iterations: %d\n\n",it);
	} 
	else 
		printf("\nDid not identify any clusters\n");
	if(conv==0)
	{
		sprintf(msg,"*** Warning: Algorithm did not converge. \
					Consider increasing maxits to enable more iterations. \
					It may also be necessary to increase damping (increase dampfact).");
		MessageBox(0,msg,"WClust - APClust Warning!!",MB_ICONWARNING);
	}

	return true;
//freememory:
//	free(i);
//	free(k);
//	free(s);
//	free(a);
//	free(r);
//	free(mx1);
//	free(mx2);
//	free(srp);
//	for(j=0;j<convits;j++) 
//		free(dec[j]); 
//	free(dec);
//	free(decsum);
//	free(idx);
}

