
/* Copyright (2001) Sandia Corportation. Under the terms of Contract 
 * DE-AC04-94AL85000, there is a non-exclusive license for use of this 
 * work by or on behalf of the U.S. Government.  Export of this program
 * may require a license from the United States Government. */


/* NOTICE:  The United States Government is granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable worldwide
 * license in ths data to reproduce, prepare derivative works, and
 * perform publicly and display publicly.  Beginning five (5) years from
 * July 25, 2001, the United States Government is granted for itself and
 * others acting on its behalf a paid-up, nonexclusive, irrevocable
 * worldwide license in this data to reproduce, prepare derivative works,
 * distribute copies to the public, perform publicly and display
 * publicly, and to permit others to do so.
 * 
 * NEITHER THE UNITED STATES GOVERNMENT, NOR THE UNITED STATES DEPARTMENT
 * OF ENERGY, NOR SANDIA CORPORATION, NOR ANY OF THEIR EMPLOYEES, MAKES
 * ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL LIABILITY OR
 * RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR USEFULNESS OF ANY
 * INFORMATION, APPARATUS, PRODUCT, OR PROCESS DISCLOSED, OR REPRESENTS
 * THAT ITS USE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS. */


#include "Epetra_Util.h"
#include "Epetra_Object.h"

const double Epetra_Util::chopVal_ = 1.0e-15;

//=============================================================================
  void Epetra_Util::Sort(bool SortAscending, int NumKeys, int * Keys, 
			int NumDoubleCompanions,double ** DoubleCompanions, 
			int NumIntCompanions, int ** IntCompanions) const {

  int i;

  int n = NumKeys;
  int * const list = Keys;
  int m = n/2;
  
  while (m > 0) {
    int max = n - m;
    for (int j=0; j<max; j++)
      {
	for (int k=j; k>=0; k-=m)
	  {
	    if ((SortAscending && list[k+m] >= list[k]) || 
		( !SortAscending && list[k+m] >= list[k]))
	      break;
	    int temp = list[k+m];
	    list[k+m] = list[k];
	    list[k] = temp;
	    for (i=0; i<NumDoubleCompanions; i++) {
	      double dtemp = DoubleCompanions[i][k+m];
	    DoubleCompanions[i][k+m] = DoubleCompanions[i][k];
	    DoubleCompanions[i][k] = dtemp;
	    }
	    for (i=0; i<NumIntCompanions; i++) {
	      int itemp = IntCompanions[i][k+m];
	    IntCompanions[i][k+m] = IntCompanions[i][k];
	    IntCompanions[i][k] = itemp;
	    }
	  }
      }
    m = m/2;
  }

}
