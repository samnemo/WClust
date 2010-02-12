/*************************************************************************
Copyright (c) 1992-2007 The University of Tennessee. All rights reserved.

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.

See subroutines comments for additional copyrights.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.

- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************/

#ifndef _lu_h
#define _lu_h

#include "ap.h"
#include "amp.h"
namespace lu
{
    template<unsigned int Precision>
    void ludecomposition(ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        ap::template_1d_array< int >& pivots);
    template<unsigned int Precision>
    void ludecompositionunpacked(ap::template_2d_array< amp::ampf<Precision> > a,
        int m,
        int n,
        ap::template_2d_array< amp::ampf<Precision> >& l,
        ap::template_2d_array< amp::ampf<Precision> >& u,
        ap::template_1d_array< int >& pivots);


    /*************************************************************************
    LU decomposition of a general matrix of size MxN

    The subroutine calculates the LU decomposition of a rectangular general
    matrix with partial pivoting (with row permutations).

    Input parameters:
        A   -   matrix A whose indexes range within [1..M, 1..N].
        M   -   number of rows in matrix A.
        N   -   number of columns in matrix A.

    Output parameters:
        A   -   matrices L and U in compact form (see below).
                Array whose indexes range within [1..M, 1..N].
        Pivots - permutation matrix in compact form (see below).
                Array whose index ranges within [1..Min(M,N)].

    Matrix A is represented as A = P * L * U, where P is a permutation matrix,
    matrix L - lower triangular (or lower trapezoid, if M>N) matrix,
    U - upper triangular (or upper trapezoid, if M<N) matrix.

    Let M be equal to 4 and N be equal to 3:

                       (  1          )    ( U11 U12 U13  )
    A = P1 * P2 * P3 * ( L21  1      )  * (     U22 U23  )
                       ( L31 L32  1  )    (         U33  )
                       ( L41 L42 L43 )

    Matrix L has size MxMin(M,N), matrix U has size Min(M,N)xN, matrix P(i) is
    a permutation of the identity matrix of size MxM with numbers I and Pivots[I].

    The algorithm returns array Pivots and the following matrix which replaces
    matrix A and contains matrices L and U in compact form (the example applies
    to M=4, N=3).

     ( U11 U12 U13 )
     ( L21 U22 U23 )
     ( L31 L32 U33 )
     ( L41 L42 L43 )

    As we can see, the unit diagonal isn't stored.

      -- LAPACK routine (version 3.0) --
         Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
         Courant Institute, Argonne National Lab, and Rice University
         June 30, 1992
    *************************************************************************/
    template<unsigned int Precision>
    void ludecomposition(ap::template_2d_array< amp::ampf<Precision> >& a,
        int m,
        int n,
        ap::template_1d_array< int >& pivots)
    {
        int i;
        int j;
        int jp;
        ap::template_1d_array< amp::ampf<Precision> > t1;
        amp::ampf<Precision> s;


        pivots.setbounds(1, ap::minint(m, n));
        t1.setbounds(1, ap::maxint(m, n));
        ap::ap_error::make_assertion(m>=0 && n>=0);
        
        //
        // Quick return if possible
        //
        if( m==0 || n==0 )
        {
            return;
        }
        for(j=1; j<=ap::minint(m, n); j++)
        {
            
            //
            // Find pivot and test for singularity.
            //
            jp = j;
            for(i=j+1; i<=m; i++)
            {
                if( amp::abs<Precision>(a(i,j))>amp::abs<Precision>(a(jp,j)) )
                {
                    jp = i;
                }
            }
            pivots(j) = jp;
            if( a(jp,j)!=0 )
            {
                
                //
                //Apply the interchange to rows
                //
                if( jp!=j )
                {
                    ap::vmove(t1.getvector(1, n), a.getrow(j, 1, n));
                    ap::vmove(a.getrow(j, 1, n), a.getrow(jp, 1, n));
                    ap::vmove(a.getrow(jp, 1, n), t1.getvector(1, n));
                }
                
                //
                //Compute elements J+1:M of J-th column.
                //
                if( j<m )
                {
                    
                    //
                    // CALL DSCAL( M-J, ONE / A( J, J ), A( J+1, J ), 1 )
                    //
                    jp = j+1;
                    s = 1/a(j,j);
                    ap::vmul(a.getcolumn(j, jp, m), s);
                }
            }
            if( j<ap::minint(m, n) )
            {
                
                //
                //Update trailing submatrix.
                //CALL DGER( M-J, N-J, -ONE, A( J+1, J ), 1, A( J, J+1 ), LDA,A( J+1, J+1 ), LDA )
                //
                jp = j+1;
                for(i=j+1; i<=m; i++)
                {
                    s = a(i,j);
                    ap::vsub(a.getrow(i, jp, n), a.getrow(j, jp, n), s);
                }
            }
        }
    }


    /*************************************************************************
    LU decomposition of a general matrix of size MxN

    It uses LUDecomposition. L and U are not output in compact form, but as
    separate general matrices filled up by zero elements in their
    corresponding positions.

    This subroutine described here only serves the purpose to show
    how the result of ComplexLUDecomposition subroutine could be unpacked.

      -- ALGLIB --
         Copyright 2005 by Bochkanov Sergey
    *************************************************************************/
    template<unsigned int Precision>
    void ludecompositionunpacked(ap::template_2d_array< amp::ampf<Precision> > a,
        int m,
        int n,
        ap::template_2d_array< amp::ampf<Precision> >& l,
        ap::template_2d_array< amp::ampf<Precision> >& u,
        ap::template_1d_array< int >& pivots)
    {
        int i;
        int j;
        int minmn;


        if( m==0 || n==0 )
        {
            return;
        }
        minmn = ap::minint(m, n);
        l.setbounds(1, m, 1, minmn);
        u.setbounds(1, minmn, 1, n);
        ludecomposition<Precision>(a, m, n, pivots);
        for(i=1; i<=m; i++)
        {
            for(j=1; j<=minmn; j++)
            {
                if( j>i )
                {
                    l(i,j) = 0;
                }
                if( j==i )
                {
                    l(i,j) = 1;
                }
                if( j<i )
                {
                    l(i,j) = a(i,j);
                }
            }
        }
        for(i=1; i<=minmn; i++)
        {
            for(j=1; j<=n; j++)
            {
                if( j<i )
                {
                    u(i,j) = 0;
                }
                if( j>=i )
                {
                    u(i,j) = a(i,j);
                }
            }
        }
    }
} // namespace

#endif
