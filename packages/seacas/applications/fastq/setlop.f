C    Copyright (c) 2014, Sandia Corporation.
C    Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
C    the U.S. Government retains certain rights in this software.
C    
C    Redistribution and use in source and binary forms, with or without
C    modification, are permitted provided that the following conditions are
C    met:
C    
C        * Redistributions of source code must retain the above copyright
C          notice, this list of conditions and the following disclaimer.
C    
C        * Redistributions in binary form must reproduce the above
C          copyright notice, this list of conditions and the following
C          disclaimer in the documentation and/or other materials provided
C          with the distribution.
C    
C        * Neither the name of Sandia Corporation nor the names of its
C          contributors may be used to endorse or promote products derived
C          from this software without specific prior written permission.
C    
C    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
C    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
C    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
C    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
C    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
C    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
C    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
C    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
C    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
C    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
C    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
C    

C $Id: setlop.f,v 1.1 1990/11/30 11:15:27 gdsjaar Exp $
C $Log: setlop.f,v $
C Revision 1.1  1990/11/30 11:15:27  gdsjaar
C Initial revision
C
C
CC* FILE: [.PAVING]SETLOP.FOR
CC* MODIFIED BY: TED BLACKER
CC* MODIFICATION DATE: 7/6/90
CC* MODIFICATION: COMPLETED HEADER INFORMATION
C
      SUBROUTINE SETLOP (MXND, MLN, NLOOP, LNODES, NODE, IVALUE, ERR)
C***********************************************************************
C
C  SUBROUTINE SETLOP = MARKS ALL THE NODES IN THE LOOP AS DESIGNATED
C
C***********************************************************************
C
      DIMENSION LNODES (MLN, MXND)
C
      LOGICAL ERR
C
      ERR = .FALSE.
C
      KOUNT = 0
      INOW = NODE
C
  100 CONTINUE
      INOW = LNODES (3, INOW)
      LNODES (1, INOW) = IVALUE
C
      IF (INOW .EQ. NODE) RETURN
C
      KOUNT = KOUNT + 1
      IF (KOUNT .GT. NLOOP) THEN
         CALL MESAGE('PROBLEMS IN SETLOP WITH LOOP NOT CLOSING')
         ERR = .TRUE.
         GOTO 110
      ENDIF
      GOTO 100
C
  110 CONTINUE
C
      RETURN
C
      END
