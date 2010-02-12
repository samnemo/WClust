// $Id: pca.cpp,v 1.4 2008/05/06 02:27:57 samn Exp $ 
#include "stdafx.h"
/*********************************/
/* Principal Components Analysis */
/*********************************/

#include "pca.h"

/*********************************************************************/
/* Principal Components Analysis or the Karhunen-Loeve expansion is a
   classical method for dimensionality reduction or exploratory data
   analysis.  One reference among many is: F. Murtagh and A. Heck,
   Multivariate Data Analysis, Kluwer Academic, Dordrecht, 1987 
   (hardbound, paperback and accompanying diskette).

   This program is public-domain.  If of importance, please reference 
   the author.  Please also send comments of any kind to the author:

   F. Murtagh
   Schlossgartenweg 1          or        35 St. Helen's Road
   D-8045 Ismaning                       Booterstown, Co. Dublin
   W. Germany                            Ireland

   Phone:        + 49 89 32006298 (work)
                 + 49 89 965307 (home)
   Telex:        528 282 22 eo d
   Fax:          + 49 89 3202362
   Earn/Bitnet:  fionn@dgaeso51,  fim@dgaipp1s,  murtagh@stsci
   Span:         esomc1::fionn
   Internet:     murtagh@scivax.stsci.edu
   

   A Fortran version of this program is also available.     
    
   F. Murtagh, Munich, 6 June 1989                                   */   
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )

void a2d(vector< vector<float> >& v,int row,int col)
{
	v = vector< vector<float> >(row);
	int i;
	for(i=0;i<row;i++) v[i] = vector<float>(col);
}

void a2d(vector< vector<double> >& v,int row,int col)
{
	v = vector< vector<double> >(row);
	int i;
	for(i=0;i<row;i++) v[i] = vector<double>(col);
}

int not_main(int argc, char** argv)
{
FILE *stream;
int  n, m,  i, j, k, k2;
PCA_T in_value;
char option;

FILE* fp = fopen("C:\\Users\\samn\\Documents\\neurosim\\spikeclust\\test_pca_out_wc_.txt","w");

vector< vector<PCA_T> > data,symmat,symmat2;
vector<PCA_T> evals,interm;

/*********************************************************************
   Get from command line:
   input data file name, #rows, #cols, option.

   Open input file: fopen opens the file whose name is stored in the
   pointer argv[argc-1]; if unsuccessful, error message is printed to
   stderr.
*********************************************************************/

   if (argc !=  5)
      {
      fprintf(fp,"Syntax help: PCA filename #rows #cols option\n\n");
      fprintf(fp,"(filename -- give full path name,\n");
      fprintf(fp," #rows                          \n");
      fprintf(fp," #cols    -- integer values,\n");                  
      fprintf(fp," option   -- R (recommended) for correlation analysis,\n");
      fprintf(fp,"             V for variance/covariance analysis\n");
      fprintf(fp,"             S for SSCP analysis.)\n");
      exit(1);
      }

   n = atoi(argv[2]);              /* # rows */
   m = atoi(argv[3]);              /* # columns */
   strncpy(&option,argv[4],1);     /* Analysis option */

   fprintf(fp,"No. of rows: %d, no. of columns: %d.\n",n,m);
   fprintf(fp,"Input file: %s.\n",argv[1]);

   if ((stream = fopen(argv[1],"r")) == NULL)
      {
      fprintf(stderr, "Program %s : cannot open file %s\n",
                       argv[0], argv[1]);
      fprintf(stderr, "Exiting to system.");
      return 0;
      /* Note: in versions of DOS prior to 3.0, argv[0] contains the
         string "C". */
      }

    /* Now read in data. */

    //data = p_matrix(n, m);  /* Storage allocation for input data */
	a2d(data,n+1,m+1);

	if(sizeof(PCA_T)==sizeof(float))
	{
		for (i = 1; i <= n; i++)
			{
			for (j = 1; j <= m; j++)
	            {
				fscanf(stream, "%f", &in_value);
				data[i][j] = in_value;
				}
			}
	}
	else
	{
		for (i = 1; i <= n; i++)
			{
			for (j = 1; j <= m; j++)
	            {
				fscanf(stream, "%lf", &in_value);
				data[i][j] = in_value;
				}
			}
	}

    // Check on (part of) input data.
    for (i = 1; i <= 18; i++) {for (j = 1; j <= 8; j++)  {
        fprintf(fp,"%7.1f", data[i][j]);  }  fprintf(fp,"\n");  }
    

    //symmat = p_matrix(m, m);  /* Allocation of correlation (etc.) matrix */
	a2d(symmat,m+1,m+1);

	vector<PCA_T> mean,stddev;
	PCA_T SqrtN;

   /* Look at analysis option; branch in accordance with this. */

     switch(option)
             {
          case 'R':
          case 'r':
              fprintf(fp,"Analysis of correlations chosen.\n");
              corcol(data, n, m, symmat, mean, stddev, SqrtN);

                          /* Output correlation matrix.
                          for (i = 1; i <= m; i++) {
                           for (j = 1; j <= 8; j++)  {
                            printf("%7.4f", symmat[i][j]);  }
                            printf("\n");  }
                          */
              break;
          case 'V':
          case 'v':
              printf("Analysis of variances-covariances chosen.\n");
              covcol(data, n, m, symmat);

                          /* Output variance-covariance matrix.
                          for (i = 1; i <= m; i++) {
                          for (j = 1; j <= 8; j++)  {
                            printf("%7.1f", symmat[i][j]);  }
                            printf("\n");  }
                          */
              break;
          case 'S':
          case 's':
              fprintf(fp,"Analysis of sums-of-squares-cross-products");
              fprintf(fp," matrix chosen.\n");
              scpcol(data, n, m, symmat);

                         /* Output SSCP matrix.
                         for (i = 1; i <= m; i++) {
                          for (j = 1; j <= 8; j++)  {
                            fprintf(fp,"%7.1f", symmat[i][j]);  }
                            fprintf(fp,"\n");  }
                         */
              break;
          default:
              fprintf(fp,"Option: %s\n",option);
              fprintf(fp,"For option, please type R, V, or S\n");
              fprintf(fp,"(upper or lower case).\n");
              fprintf(fp,"Exiting to system.\n");
              exit(1);
              break;
          }

/*********************************************************************
    Eigen-reduction
**********************************************************************/

    /* Allocate storage for dummy and new vectors. */
    //evals = p_vector(m);     /* Storage alloc. for vector of eigenvalues */
	evals=vector<PCA_T>(m+1);
    //interm = p_vector(m);    /* Storage alloc. for 'intermediate' vector */
	interm=vector<PCA_T>(m+1);
    //symmat2 = p_matrix(m, m);  /* Duplicate of correlation (etc.) matrix */
	a2d(symmat2,m+1,m+1);
    for (i = 1; i <= m; i++) {
     for (j = 1; j <= m; j++) {
      symmat2[i][j] = symmat[i][j]; /* Needed below for col. projections */
                              }
                             }
    tred2(symmat, m, evals, interm);  /* Triangular decomposition */
    tqli(evals, interm, m, symmat);   /* Reduction of sym. trid. matrix */
    /* evals now contains the eigenvalues,
       columns of symmat now contain the associated eigenvectors. */

     fprintf(fp,"\nEigenvalues:\n");
     for (j = m; j >= 1; j--) {
       fprintf(fp,"%18.5f\n", evals[j]); }
     fprintf(fp,"\n(Eigenvalues should be strictly positive; limited\n");
     fprintf(fp,"precision machine arithmetic may affect this.\n");
     fprintf(fp,"Eigenvalues are often expressed as cumulative\n");
     fprintf(fp,"percentages, representing the 'percentage variance\n");
     fprintf(fp,"explained' by the associated axis or principal component.)\n");

     fprintf(fp,"\nEigenvectors:\n");
     fprintf(fp,"(First three; their definition in terms of original vbes.)\n");
     for (j = 1; j <= m; j++) {
       for (i = 1; i <= 3; i++)  {
          fprintf(fp,"%12.4f", symmat[j][m-i+1]);  }
          fprintf(fp,"\n");  }

     /* Form projections of row-points on first three prin. components. */
     /* Store in 'data', overwriting original data. */
     for (i = 1; i <= n; i++) {
      for (j = 1; j <= m; j++) {
        interm[j] = data[i][j]; }   /* data[i][j] will be overwritten */
        for (k = 1; k <= 3; k++) {
          data[i][k] = 0.0;
          for (k2 = 1; k2 <= m; k2++) {
            data[i][k] += interm[k2] * symmat[k2][m-k+1]; }
        }
     }

     fprintf(fp,"\nProjections of row-points on first 3 prin. comps.:\n");
     for (i = 1; i <= n; i++) {
       for (j = 1; j <= 3; j++)  {
          fprintf(fp,"%12.4f", data[i][j]);  }
          fprintf(fp,"\n");  }

     /* Form projections of col.-points on first three prin. components. */
     /* Store in 'symmat2', overwriting what was stored in this. */
     for (j = 1; j <= m; j++) {
      for (k = 1; k <= m; k++) {
        interm[k] = symmat2[j][k]; }  /*symmat2[j][k] will be overwritten*/
        for (i = 1; i <= 3; i++) {
          symmat2[j][i] = 0.0;
          for (k2 = 1; k2 <= m; k2++) {
            symmat2[j][i] += interm[k2] * symmat[k2][m-i+1]; }
          if (evals[m-i+1] > 0.0005)   /* Guard against zero eigenvalue */
             symmat2[j][i] /= sqrt(evals[m-i+1]);   /* Rescale */
          else
             symmat2[j][i] = 0.0;    /* Standard kludge */
        }
     }

     fprintf(fp,"\nProjections of column-points on first 3 prin. comps.:\n");
     for (j = 1; j <= m; j++) {
       for (k = 1; k <= 3; k++)  {
          fprintf(fp,"%12.4f", symmat2[j][k]);  }
          fprintf(fp,"\n");  }

	 fclose(fp);

//    free_p_matrix(data, n, m);
//    free_p_matrix(symmat, m, m);
//    free_p_matrix(symmat2, m, m);
//    free_p_vector(evals, m);
//    free_p_vector(interm, m);

	return 0;
}

/**  Correlation matrix: creation  ***********************************/

void corcol(vector< vector<PCA_T> >& data,int n,int m,vector< vector<PCA_T> >& symmat, vector<PCA_T>& mean, vector<PCA_T>& stddev, PCA_T& SqrtN)
/* Create m * m correlation matrix from given n * m data matrix. */
{
PCA_T eps = 0.005;
PCA_T x;//, *mean, *stddev;
int i, j, j1, j2;

/* Allocate storage for mean and std. dev. vectors */

//mean = p_vector(m);
//stddev = p_vector(m);

mean = vector<PCA_T>(m+1);
stddev = vector<PCA_T>(m+1);

/* Determine mean of column vectors of input data matrix */

for (j = 1; j <= m; j++)
    {
    mean[j] = 0.0;
    for (i = 1; i <= n; i++)
        {
        mean[j] += data[i][j];
        }
    mean[j] /= (PCA_T)n;
    }

//printf("\nMeans of column vectors:\n");
//for (j = 1; j <= m; j++)  {
  //  printf("%7.1f",mean[j]);  }   printf("\n");

/* Determine standard deviations of column vectors of data matrix. */

for (j = 1; j <= m; j++)
    {
    stddev[j] = 0.0;
    for (i = 1; i <= n; i++)
        {
        stddev[j] += (   ( data[i][j] - mean[j] ) *
                         ( data[i][j] - mean[j] )  );
        }
        stddev[j] /= (PCA_T)n;
        stddev[j] = sqrt(stddev[j]);
        /* The following in an inelegant but usual way to handle
        near-zero std. dev. values, which below would cause a zero-
        divide. */
        if (stddev[j] <= eps) stddev[j] = 1.0;
    }

//printf("\nStandard deviations of columns:\n");
//for (j = 1; j <= m; j++) { printf("%7.1f", stddev[j]); }
//printf("\n");

/* Center and reduce the column vectors. */

SqrtN = sqrt((PCA_T)n);

for (i = 1; i <= n; i++)
    {
    for (j = 1; j <= m; j++)
        {
        data[i][j] -= mean[j];
        x = SqrtN;
        x *= stddev[j];
        data[i][j] /= x;
        }
    }

/* Calculate the m * m correlation matrix. */
for (j1 = 1; j1 <= m-1; j1++)
    {
    symmat[j1][j1] = 1.0;
    for (j2 = j1+1; j2 <= m; j2++)
        {
        symmat[j1][j2] = 0.0;
        for (i = 1; i <= n; i++)
            {
            symmat[j1][j2] += ( data[i][j1] * data[i][j2]);
            }
        symmat[j2][j1] = symmat[j1][j2];
        }
    }
    symmat[m][m] = 1.0;

return;

}

void corcol(A2D<PCA_T>& data,int n,int m,vector< vector<PCA_T> >& symmat, vector<PCA_T>& mean, vector<PCA_T>& stddev, PCA_T& SqrtN)
/* Create m * m correlation matrix from given n * m data matrix. */
{
PCA_T eps = 0.005;
PCA_T x;//, *mean, *stddev;
int i, j, j1, j2;

/* Allocate storage for mean and std. dev. vectors */

//mean = p_vector(m);
//stddev = p_vector(m);

mean = vector<PCA_T>(m+1);
stddev = vector<PCA_T>(m+1);

/* Determine mean of column vectors of input data matrix */

for (j = 1; j <= m; j++)
    {
    mean[j] = 0.0;
    for (i = 1; i <= n; i++)
        {
        mean[j] += data[i][j];
        }
    mean[j] /= (PCA_T)n;
    }

//printf("\nMeans of column vectors:\n");
//for (j = 1; j <= m; j++)  {
  //  printf("%7.1f",mean[j]);  }   printf("\n");

/* Determine standard deviations of column vectors of data matrix. */

for (j = 1; j <= m; j++)
    {
    stddev[j] = 0.0;
    for (i = 1; i <= n; i++)
        {
        stddev[j] += (   ( data[i][j] - mean[j] ) *
                         ( data[i][j] - mean[j] )  );
        }
        stddev[j] /= (PCA_T)n;
        stddev[j] = sqrt(stddev[j]);
        /* The following in an inelegant but usual way to handle
        near-zero std. dev. values, which below would cause a zero-
        divide. */
        if (stddev[j] <= eps) stddev[j] = 1.0;
    }

//printf("\nStandard deviations of columns:\n");
//for (j = 1; j <= m; j++) { printf("%7.1f", stddev[j]); }
//printf("\n");

/* Center and reduce the column vectors. */

SqrtN = sqrt((PCA_T)n);

for (i = 1; i <= n; i++)
    {
    for (j = 1; j <= m; j++)
        {
        data[i][j] -= mean[j];
        x = SqrtN;
        x *= stddev[j];
        data[i][j] /= x;
        }
    }

/* Calculate the m * m correlation matrix. */
for (j1 = 1; j1 <= m-1; j1++)
    {
    symmat[j1][j1] = 1.0;
    for (j2 = j1+1; j2 <= m; j2++)
        {
        symmat[j1][j2] = 0.0;
        for (i = 1; i <= n; i++)
            {
            symmat[j1][j2] += ( data[i][j1] * data[i][j2]);
            }
        symmat[j2][j1] = symmat[j1][j2];
        }
    }
    symmat[m][m] = 1.0;

return;

}


/**  Variance-covariance matrix: creation  *****************************/

void covcol(vector< vector<PCA_T> >& data,int n,int m,vector< vector<PCA_T> >& symmat)
/* Create m * m covariance matrix from given n * m data matrix. */
{
//PCA_T *mean;
int i, j, j1, j2;

/* Allocate storage for mean vector */

//mean = p_vector(m);
vector<PCA_T> mean(m+1);

/* Determine mean of column vectors of input data matrix */

for (j = 1; j <= m; j++)
    {
    mean[j] = 0.0;
    for (i = 1; i <= n; i++)
        {
        mean[j] += data[i][j];
        }
    mean[j] /= (PCA_T)n;
    }

//printf("\nMeans of column vectors:\n");
//for (j = 1; j <= m; j++)  {
    //printf("%7.1f",mean[j]);  }   printf("\n");

/* Center the column vectors. */

for (i = 1; i <= n; i++)
    {
    for (j = 1; j <= m; j++)
        {
        data[i][j] -= mean[j];
        }
    }

/* Calculate the m * m covariance matrix. */
for (j1 = 1; j1 <= m; j1++)
    {
    for (j2 = j1; j2 <= m; j2++)
        {
        symmat[j1][j2] = 0.0;
        for (i = 1; i <= n; i++)
            {
            symmat[j1][j2] += data[i][j1] * data[i][j2];
            }
        symmat[j2][j1] = symmat[j1][j2];
        }
    }

return;

}

void covcol(A2D<PCA_T>& data,int n,int m,vector< vector<PCA_T> >& symmat,vector<PCA_T>& mean)
/* Create m * m covariance matrix from given n * m data matrix. */
{
	//PCA_T *mean;
	int i, j, j1, j2;

	/* Allocate storage for mean vector */

	//mean = p_vector(m);
	mean.resize(m+1);

	/* Determine mean of column vectors of input data matrix */

	for (j = 1; j <= m; j++)
	{
		mean[j] = 0.0;
		for (i = 1; i <= n; i++)
		{
			mean[j] += data[i][j];
		}
		mean[j] /= (PCA_T)n;
	}

	//printf("\nMeans of column vectors:\n");
	//for (j = 1; j <= m; j++)  {
	//printf("%7.1f",mean[j]);  }   printf("\n");

	/* Center the column vectors. */

	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= m; j++)
		{
			data[i][j] -= mean[j];
		}
	}

	/* Calculate the m * m covariance matrix. */
	for (j1 = 1; j1 <= m; j1++)
	{
		for (j2 = j1; j2 <= m; j2++)
		{
			symmat[j1][j2] = 0.0;
			for (i = 1; i <= n; i++)
			{
				symmat[j1][j2] += data[i][j1] * data[i][j2];
			}
			symmat[j2][j1] = symmat[j1][j2];
		}
	}

	return;
}


/**  Sums-of-squares-and-cross-products matrix: creation  **************/

void scpcol(vector< vector<PCA_T> >& data,int n,int m,vector< vector<PCA_T> >& symmat)
/* Create m * m sums-of-cross-products matrix from n * m data matrix. */
{
int i, j1, j2;

/* Calculate the m * m sums-of-squares-and-cross-products matrix. */

for (j1 = 1; j1 <= m; j1++)
    {
    for (j2 = j1; j2 <= m; j2++)
        {
        symmat[j1][j2] = 0.0;
        for (i = 1; i <= n; i++)
            {
            symmat[j1][j2] += data[i][j1] * data[i][j2];
            }
        symmat[j2][j1] = symmat[j1][j2];
        }
    }

return;

}

/**  Error handler  **************************************************/

void erhand(char* err_msg)
/* Error handler */
{
//	fprintf(stderr,"PCA: Run-time error:\n");
//    fprintf(stderr,"%s\n", err_msg);
}

/**  Allocation of vector storage  ***********************************/

PCA_T *p_vector(int n)
/* Allocates a PCA_T vector with range [1..n]. */
{

    PCA_T *v;

    v = (PCA_T *) malloc ((unsigned) n*sizeof(PCA_T));
    if (!v) erhand("Allocation failure in vector().");
    return v-1;

}

/**  Allocation of PCA_T matrix storage  *****************************/

PCA_T **p_matrix(int n,int m)
/* Allocate a PCA_T matrix with range [1..n][1..m]. */
{
    int i;
    PCA_T **mat;

    /* Allocate pointers to rows. */
    mat = (PCA_T **) malloc((unsigned) (n)*sizeof(PCA_T*));
    if (!mat) erhand("Allocation failure 1 in matrix().");
    mat -= 1;

    /* Allocate rows and set pointers to them. */
    for (i = 1; i <= n; i++)
        {
        mat[i] = (PCA_T *) malloc((unsigned) (m)*sizeof(PCA_T));
        if (!mat[i]) erhand("Allocation failure 2 in matrix().");
        mat[i] -= 1;
        }

     /* Return pointer to array of pointers to rows. */
     return mat;

}

/**  Deallocate vector storage  *********************************/

void free_p_vector(PCA_T* v,int n)
/* Free a PCA_T vector allocated by vector(). */
{
   free((char*) (v+1));
}

/**  Deallocate PCA_T matrix storage  ***************************/

void free_p_matrix(PCA_T** mat,int n,int m)
/* Free a PCA_T matrix allocated by matrix(). */
{
   int i;

   for (i = n; i >= 1; i--)
       {
       free ((char*) (mat[i]+1));
       }
   free ((char*) (mat+1));
}

/**  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */

void tred2(vector< vector<PCA_T> >& a,int n,vector<PCA_T>& d,vector<PCA_T>& e)
{
int l, k, j, i;
float scale, hh, h, g, f;

for (i = n; i >= 2; i--)
    {
    l = i - 1;
    h = scale = 0.0;
    if (l > 1)
       {
       for (k = 1; k <= l; k++)
           scale += fabs(a[i][k]);
       if (scale == 0.0)
          e[i] = a[i][l];
       else
          {
          for (k = 1; k <= l; k++)
              {
              a[i][k] /= scale;
              h += a[i][k] * a[i][k];
              }
          f = a[i][l];
          g = f>0 ? -sqrt(h) : sqrt(h);
          e[i] = scale * g;
          h -= f * g;
          a[i][l] = f - g;
          f = 0.0;
          for (j = 1; j <= l; j++)
              {
              a[j][i] = a[i][j]/h;
              g = 0.0;
              for (k = 1; k <= j; k++)
                  g += a[j][k] * a[i][k];
              for (k = j+1; k <= l; k++)
                  g += a[k][j] * a[i][k];
              e[j] = g / h;
              f += e[j] * a[i][j];
              }
          hh = f / (h + h);
          for (j = 1; j <= l; j++)
              {
              f = a[i][j];
              e[j] = g = e[j] - hh * f;
              for (k = 1; k <= j; k++)
                  a[j][k] -= (f * e[k] + g * a[i][k]);
              }
         }
    }
    else
        e[i] = a[i][l];
    d[i] = h;
    }
d[1] = 0.0;
e[1] = 0.0;
for (i = 1; i <= n; i++)
    {
    l = i - 1;
    if (d[i])
       {
       for (j = 1; j <= l; j++)
           {
           g = 0.0;
           for (k = 1; k <= l; k++)
               g += a[i][k] * a[k][j];
           for (k = 1; k <= l; k++)
               a[k][j] -= g * a[k][i];
           }
       }
       d[i] = a[i][i];
       a[i][i] = 1.0;
       for (j = 1; j <= l; j++)
           a[j][i] = a[i][j] = 0.0;
    }
}

/* void tred2(vector< vector<PCA_T> >& a,int n,vector<PCA_T>& d,vector<PCA_T>& e)
// Householder reduction of matrix a to tridiagonal form.
//   Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
//   Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
//        Springer-Verlag, 1976, pp. 489-494.
//        W H Press et al., Numerical Recipes in C, Cambridge U P,
//        1988, pp. 373-374.  
{
int l, k, j, i;
PCA_T scale, hh, h, g, f;

for (i = n; i >= 2; i--)
    {
    l = i - 1;
    h = scale = 0.0;
    if (l > 1)
       {
       for (k = 1; k <= l; k++)
           scale += fabs(a[i][k]);
       if (scale == 0.0)
          e[i] = a[i][l];
       else
          {
          for (k = 1; k <= l; k++)
              {
              a[i][k] /= scale;
              h += a[i][k] * a[i][k];
              }
          f = a[i][l];
          g = f>0 ? -sqrt(h) : sqrt(h);
          e[i] = scale * g;
          h -= f * g;
          a[i][l] = f - g;
          f = 0.0;
          for (j = 1; j <= l; j++)
              {
              a[j][i] = a[i][j]/h;
              g = 0.0;
              for (k = 1; k <= j; k++)
                  g += a[j][k] * a[i][k];
              for (k = j+1; k <= l; k++)
                  g += a[k][j] * a[i][k];
              e[j] = g / h;
              f += e[j] * a[i][j];
              }
          hh = f / (h + h);
          for (j = 1; j <= l; j++)
              {
              f = a[i][j];
              e[j] = g = e[j] - hh * f;
              for (k = 1; k <= j; k++)
                  a[j][k] -= (f * e[k] + g * a[i][k]);
              }
         }
    }
    else
        e[i] = a[i][l];
    d[i] = h;
    }
d[1] = 0.0;
e[1] = 0.0;
for (i = 1; i <= n; i++)
    {
    l = i - 1;
    if (d[i])
       {
       for (j = 1; j <= l; j++)
           {
           g = 0.0;
           for (k = 1; k <= l; k++)
               g += a[i][k] * a[k][j];
           for (k = 1; k <= l; k++)
               a[k][j] -= g * a[k][i];
           }
       }
       d[i] = a[i][i];
       a[i][i] = 1.0;
       for (j = 1; j <= l; j++)
           a[j][i] = a[i][j] = 0.0;
    }
}*/

/**  Tridiagonal QL algorithm -- Implicit  **********************/

void tqli(vector<PCA_T>& d, vector<PCA_T>& e,int n,vector< vector<PCA_T> >& z)
{
int m, l, iter, i, k;
float s, r, p, g, f, dd, c, b;

for (i = 2; i <= n; i++)
    e[i-1] = e[i];
e[n] = 0.0;
for (l = 1; l <= n; l++)
    {
    iter = 0;
    do
      {
      for (m = l; m <= n-1; m++)
          {
          dd = fabs(d[m]) + fabs(d[m+1]);
          if (fabs(e[m]) + dd == dd) break;
          }
          if (m != l)
             {
             if (iter++ == 30) erhand("No convergence in TLQI.");
             g = (d[l+1] - d[l]) / (2.0 * e[l]);
             r = sqrt((g * g) + 1.0);
             g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
             s = c = 1.0;
             p = 0.0;
             for (i = m-1; i >= l; i--)
                 {
                 f = s * e[i];
                 b = c * e[i];
                 if (fabs(f) >= fabs(g))
                    {
                    c = g / f;
                    r = sqrt((c * c) + 1.0);
                    e[i+1] = f * r;
                    c *= (s = 1.0/r);
                    }
                 else
                    {
                    s = f / g;
                    r = sqrt((s * s) + 1.0);
                    e[i+1] = g * r;
                    s *= (c = 1.0/r);
                    }
                 g = d[i+1] - p;
                 r = (d[i] - g) * s + 2.0 * c * b;
                 p = s * r;
                 d[i+1] = g + p;
                 g = c * r - b;
                 for (k = 1; k <= n; k++)
                     {
                     f = z[k][i+1];
                     z[k][i+1] = s * z[k][i] + c * f;
                     z[k][i] = c * z[k][i] - s * f;
                     }
                 }
                 d[l] = d[l] - p;
                 e[l] = g;
                 e[m] = 0.0;
             }
          }  while (m != l);
      }
}
/*
void tqli(vector<PCA_T>& d,vector<PCA_T>& e,int n,vector< vector<PCA_T> >& z)
{
int m, l, iter, i, k;
PCA_T s, r, p, g, f, dd, c, b;

for (i = 2; i <= n; i++)
    e[i-1] = e[i];
e[n] = 0.0;
for (l = 1; l <= n; l++)
    {
    iter = 0;
    do
      {
      for (m = l; m <= n-1; m++)
          {
          dd = fabs(d[m]) + fabs(d[m+1]);
          if (fabs(e[m]) + dd == dd) break;
          }
          if (m != l)
             {
             if (iter++ == 30) erhand("No convergence in TLQI.");
             g = (d[l+1] - d[l]) / (2.0 * e[l]);
             r = sqrt((g * g) + 1.0);
             g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
             s = c = 1.0;
             p = 0.0;
             for (i = m-1; i >= l; i--)
                 {
                 f = s * e[i];
                 b = c * e[i];
                 if (fabs(f) >= fabs(g))
                    {
                    c = g / f;
                    r = sqrt((c * c) + 1.0);
                    e[i+1] = f * r;
                    c *= (s = 1.0/r);
                    }
                 else
                    {
                    s = f / g;
                    r = sqrt((s * s) + 1.0);
                    e[i+1] = g * r;
                    s *= (c = 1.0/r);
                    }
                 g = d[i+1] - p;
                 r = (d[i] - g) * s + 2.0 * c * b;
                 p = s * r;
                 d[i+1] = g + p;
                 g = c * r - b;
                 for (k = 1; k <= n; k++)
                     {
                     f = z[k][i+1];
                     z[k][i+1] = s * z[k][i] + c * f;
                     z[k][i] = c * z[k][i] - s * f;
                     }
                 }
                 d[l] = d[l] - p;
                 e[l] = g;
                 e[m] = 0.0;
             }
          }  while (m != l);
      }
 }
 */
