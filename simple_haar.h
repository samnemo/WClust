// $Id: simple_haar.h,v 1.1 2008/04/01 06:08:37 samn Exp $
#ifndef SIMPLE_HAAR
#define SIMPLE_HAAR

#include <vector>

using namespace std;

typedef unsigned char byte;

/**
 *

  Calculate power and log functions using fast
  binary operations.

 */
struct binary {
  /**
   *
     Calculate 2<sup>n</sup> where n >= 0

     @param n
        a value between 0..31

     @return power2 returns 2<sup>n</sup>

   */
  static int power2( byte n)
  {
    int rslt = 0x1 << (n & 0x1f);
    return rslt;
  } // power2


  /**
   *
  <p>
     Calculate floor( log<sub>2</sub>( val ) ), where val > 0
  <p>
     The log function is undefined for 0.

     @param val
        a positive value

     @return <tt>floor( log<sub>2</sub>( val ) )</tt>

   */
  static byte log2(int val )
  {
    byte log;

    for (log = 0; val > 0; log++, val = val >> 1)
      ;

    log--;

    return log;
  } // log2


  /**
    nearestPower2
  <p>
    Given a value "val", where val > 0, nearestPower2 returns
    the power of 2 that is less than or equal to val.

   */
  static int nearestPower2( int val )
  {
    byte l = log2( val );
    int power = power2( l );

    return power;
  } // nearestPower2

}; // class binary

/**
 *

  <p>
  Wavelet base class

  <p>
  The wavelet base class supplies the common functions
  <tt>power2</tt> and <tt>log2</tt> and defines the
  abstract methods for the derived classes.

@author Ian Kaplan

@see <i>Wavelets Made Easy by Yves Nieverglt, Birkhauser, 1999</i>


 */
 class wavelet_base {
 
 public:

  /**
   *
     Abstract function for calculating a wavelet function.

     @param values
        Calculate the wavelet function from the <tt>values</tt>
        array.
   */
  void wavelet_calc( vector<double> values );

  /**
   *
     Print the wavelet function result.

   */
  void pr();

  void inverse();

};  // wavelet_interface

 class simple_haar : public wavelet_base  
 {
 public:
  double haar_value;  // the final Haar step value
  vector< vector< double > > coef;        // The Haar coefficients
  vector<double> data;

  void reverseCoef();

  public:
  
  void wavelet_calc( vector<double>& values );
  double haar_calc( vector<double>& values );
  void inverse();
  void pr();
  void pr_values();


 };

#endif
