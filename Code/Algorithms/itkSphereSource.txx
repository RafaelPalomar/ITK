/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkSphereSource.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef _itkSphereSource_txx
#define _itkSphereSource_txx

#include "itkSphereSource.h"

namespace itk
{

/**
 *
 */
template<class TOutputMesh>
SphereSource<TOutputMesh>
::SphereSource()
{
  /**
   * Create the output
   */
  typename TOutputMesh::Pointer output = TOutputMesh::New();
  this->ProcessObject::SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, output.GetPointer());
  m_Squareness1 = 1.0;
  m_Squareness2 = 1.0;
}

/**
 *
 */
template<class TOutputMesh>
void
SphereSource<TOutputMesh>
::GenerateData()
{
  unsigned long i, j, jn, p, numpts, numcells;
  double ustep, vstep, ubeg, vbeg, u, v; 
  int signu, signv; 

// calculate the number os cells and points
  numpts = m_Resolution[0]*m_Resolution[1] + 2; 
  numcells = 2 * (m_Resolution[0]-1) *m_Resolution[1] + 2*m_Resolution[1]; 

// calculate the steps using resolution
  ustep = vnl_math::pi / (m_Resolution[0]+1); 
  vstep = 2.0*vnl_math::pi / m_Resolution[1]; 
  ubeg = (-vnl_math::pi/2.0) + ustep; 
  vbeg = -vnl_math::pi; 

///////////////////////////////////////////////////////////////////////////
// nodes allocation

// the temporary container of nodes' connectness
  unsigned long tripoints[3] = {0,1,2};
  
// memory allocation for nodes
  this->GetOutput()->GetPoints()->Reserve(numpts);

  PointsContainerPointer  myPoints = this->GetOutput()->GetPoints();
  typename PointsContainer::Iterator   point = myPoints->Begin();

  PointType p1;

// calculate all regular nodes
  while( point != myPoints->End() ) 
    { 
    for (u=ubeg, i=0; i < m_Resolution[0]; u += ustep, i++) { 
      for (v=vbeg, j=0; j < m_Resolution[1]; v += vstep, j++) { 
        if (cos(u) > 0) {signu = 1;} else {signu = -1;}
        if (cos(v) > 0) {signv = 1;} else {signv = -1;}

        p1[0] = m_Scale[0]*signu*(pow((float)(fabs(cos(u))), m_Shape[0]))*signv* 
            (pow((float)(fabs(cos(v))), m_Shape[1])) + m_Center[0]; 

        if (sin(v) > 0) {signv = 1;} else {signv = -1;}

        p1[1] = m_Scale[1]*signu*(pow((float)(fabs(cos(u))), m_Shape[0]))*signv* 
            (pow((float)(fabs(sin(v))), m_Shape[1])) + m_Center[1]; 

        if (sin(u) > 0) {signu = 1;} else {signu = -1;}

        p1[2] = m_Scale[2]*signu*(pow((float)(fabs(sin(u))), m_Shape[0])) + 
            m_Center[2];

        point.Value() = p1;
        ++point;
      } 
    }   

// calculate the south pole node
    p1[0] = (m_Scale[0]*(pow((float)(fabs(cos(-vnl_math::pi/2))),1.0f))* 
        (pow((float)(fabs(cos(0.0))),1.0f)) + m_Center[0]); 
    p1[1] = (m_Scale[1]*(pow((float)(fabs(cos(-vnl_math::pi/2))),1.0f))* 
        (pow((float)(fabs(sin(0.0))),1.0f)) + m_Center[1]); 
    p1[2] = (m_Scale[2]*-1*(pow((float)(fabs(sin(-vnl_math::pi/2))),1.0f)) 
        + m_Center[2]);
    point.Value() = p1;
    ++point;

// calculate the north pole node
    p1[0] = (m_Scale[0]*(pow((float)(fabs(cos(vnl_math::pi/2))),1.0f))* 
        (pow(fabs(cos(0.0)),1.0)) + m_Center[0]); 
    p1[1] = (m_Scale[1]*(pow((float)(fabs(cos(vnl_math::pi/2))),1.0f))* 
        (pow(fabs(sin(0.0)),1.0)) + m_Center[1]); 
    p1[2] = (m_Scale[2]*(pow((float)(fabs(sin(vnl_math::pi/2))),1.0f)) 
        + m_Center[2]);
    point.Value() = p1;
    ++point;
  }

///////////////////////////////////////////////////////////////////////////
// cells allocation
  p = 0;
  TriCellPointer testCell(TriCell::New());

// store all regular cells
  for(int i=0; i < m_Resolution[0]-1 ; i++) {
    for (int j=0; j<m_Resolution[1]; j++) {
      jn = (j+1)%m_Resolution[1]; 
      tripoints[0] = i*m_Resolution[1]+j; 
      tripoints[1] = tripoints[0]-j+jn; 
      tripoints[2] = tripoints[0]+m_Resolution[1]; 
      testCell->SetPointIds(tripoints);
      this->GetOutput()->SetCell(p, testCell);
      this->GetOutput()->SetCellData(p, (PixelType)3.0);
      p++;
      testCell = TriCell::New();
      tripoints[0] = tripoints[1]; 
      tripoints[1] = tripoints[0]+m_Resolution[1]; 
      testCell->SetPointIds(tripoints);
      this->GetOutput()->SetCell(p, testCell);
      this->GetOutput()->SetCellData(p, (PixelType)3.0);
      p++;
      testCell = TriCell::New();
    }
  }
 
// store cells containing the south pole nodes
  for (int j=0; j<m_Resolution[1]; j++) {
    jn = (j+1)%m_Resolution[1]; 
    tripoints[0] = numpts-2; 
    tripoints[1] = jn; 
    tripoints[2] = j; 
    testCell->SetPointIds(tripoints);
    this->GetOutput()->SetCell(p, testCell);
    this->GetOutput()->SetCellData(p, (PixelType)1.0);
    p++;
    testCell = TriCell::New();
  }

// store cells containing the north pole nodes
  for (int j=0; j<m_Resolution[1]; j++) {
    jn = (j+1)%m_Resolution[1]; 
    tripoints[2] = (m_Resolution[0]-1)*m_Resolution[1]+j; 
    tripoints[1] = numpts-1; 
    tripoints[0] = tripoints[2]-j+jn; 
    testCell->SetPointIds(tripoints);
    this->GetOutput()->SetCell(p, testCell);
    this->GetOutput()->SetCellData(p, (PixelType)2.0);
    p++;
    testCell = TriCell::New();
  }
}


} // end namespace itk

#endif
