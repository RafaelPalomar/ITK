/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFuzzyConnectednessRGBImageFilter.h
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
#ifndef __itkFuzzyConnectednessRGBImageFilter_h
#define __itkFuzzyConnectednessRGBImageFilter_h

#include "itkImage.h"
#include "itkImageToImageFilter.h"
#include "itkSimpleFuzzyConnectednessImageFilterBase.h"
#include <vnl/vnl_matrix_fixed.h>

#include <queue>

namespace itk{

/** /class FuzzyConnectednessRGBImageFilter
 * 
 * Perform the segmentation by three channel(RGB) fuzzy connectedness.  Used
 * as a node of the segmentation toolkits.  The Basic concept here is the
 * fuzzy affinity which is defined between two neighbor pixels, it reflects
 * the similarity and possibility of these two pixels to be in the same
 * object.  A "path" between two pixels is a list of pixels that connect
 * them, the stregth of a particular path was defined as the weakest affinity
 * between the neighbor pixels that form the path.  The fuzzy connectedness
 * between two pixels is defined as the strongest path stregth between these
 * two pixels.  The segmentation based on fuzzy connectedness assumes that
 * the fuzzy connectedness between any two pixels is significantly higher
 * than those belongs to different objects.  A fuzzy connectedness scene was
 * first computed, which is the fuzzy connectedness value to a preset seed
 * point believed to be inside the object of interest.  then a threshold was
 * applied to obtain the binary segmented object.
 * 
 * Usage:
 *
 * 1. use SetInput to import the input image object
 * 2. use SetParameter, SetSeed, SetThreshold to set the parameters
 * 3. run ExcuteSegment to perform the segmenation
 * 4. threshold can be set after the segmentation, and no computation
 *    will be redo. no need to run GenerateData. But if SetThreshold was used.
 *    MakeSegmentObject() should be called to get the updated result.
 * 5. use GetOutput to obtain the resulted binary image Object.
 * 6. GetFuzzyScene gives the pointer of Image<unsigned short> for the 
 *    fuzzy scene.
 *
 * Detail information about this algorithm can be found in:
 *  "Fuzzy Connectedness and Object Definition: Theory, Algorithms,
 *    and Applications in Image Segmentation", J. Udupa and S. Samarasekera
 *  Graphical Models and Image Processing, Vol.58, No.3. pp 246-261, 1996.
 *
 * the input image should be in the form of:
 *  itkImage<itkVector<Pixeltype,3>,2>
 *
 * \ingroup FuzzyConnectednessSegmentation */

template <class TInputImage, class TOutputImage>
class FuzzyConnectednessRGBImageFilter:
  public SimpleFuzzyConnectednessImageFilterBase<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FuzzyConnectednessRGBImageFilter       Self;
  typedef SimpleFuzzyConnectednessImageFilterBase<TInputImage,TOutputImage>
                                                 Superclass;
  typedef SmartPointer <Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FuzzyConnectednessRGBImageFilter,
               SimpleFuzzyConnectednessImageFilterBase);

  /** The pixel type. */
  typedef typename TInputImage::PixelType PixelType;

  /** Setting and geting the segmentation parameters. */
  itkSetVectorMacro(Mean,double,3);
  void GetMean(double omean[3])
    {
    omean[0]=m_Mean[0];omean[1]=m_Mean[1];omean[2]=m_Mean[2];
    }
  void SetVar(double ivar[3][3])
    {
    m_Var[0][0]=ivar[0][0];m_Var[0][1]=ivar[0][1];m_Var[0][2]=ivar[0][2];
    m_Var[1][0]=ivar[1][0];m_Var[1][1]=ivar[1][1];m_Var[1][2]=ivar[1][2];
    m_Var[2][0]=ivar[2][0];m_Var[2][1]=ivar[2][2];m_Var[2][2]=ivar[2][2];
    }
  void GetVar(double ovar[3][3])
    {
    ovar[0][0]=m_Var[0][0];ovar[0][1]=m_Var[0][1];ovar[0][2]=m_Var[0][2];
    ovar[1][0]=m_Var[1][0];ovar[1][1]=m_Var[1][1];ovar[1][2]=m_Var[1][2];
    ovar[2][0]=m_Var[2][0];ovar[2][1]=m_Var[2][1];ovar[2][2]=m_Var[2][2];
    }
  itkSetVectorMacro(DiffMean,double,3);
  void GetDiffMean(double odmean[3])
    {
    odmean[0]=m_DiffMean[0];odmean[1]=m_DiffMean[1];odmean[2]=m_DiffMean[2];
    };
  void SetDiffVar(double idvar[3][3])
    {
    m_DiffVar[0][0]=idvar[0][0];m_DiffVar[0][1]=idvar[0][1];m_DiffVar[0][2]=idvar[0][2];
    m_DiffVar[1][0]=idvar[1][0];m_DiffVar[1][1]=idvar[1][1];m_DiffVar[1][2]=idvar[1][2];
    m_DiffVar[2][0]=idvar[2][0];m_DiffVar[2][1]=idvar[2][1];m_DiffVar[2][2]=idvar[2][2];
    }
  void GetDiffVar(double odvar[3][3])
    {
    odvar[0][0]=m_DiffVar[0][0];odvar[0][1]=m_DiffVar[0][1];odvar[0][2]=m_DiffVar[0][2];
    odvar[1][0]=m_DiffVar[1][0];odvar[1][1]=m_DiffVar[1][1];odvar[1][2]=m_DiffVar[1][2];
    odvar[2][0]=m_DiffVar[2][0];odvar[2][1]=m_DiffVar[2][1];odvar[2][2]=m_DiffVar[2][2];
    }

protected:
  FuzzyConnectednessRGBImageFilter();
  ~FuzzyConnectednessRGBImageFilter();

  void GenerateData(void);
 
private:
  double m_Mean[3];
  double m_Var[3][3]; //covariance matrix of the RGB channels.(estimated)
  double m_DiffMean[3];
  double m_DiffVar[3][3];
  double m_VarInverse[3][3];
  double m_DiffVarInverse[3][3];
  double m_VarDet;
  double m_DiffVarDet;

  virtual double FuzzyAffinity(const PixelType f1, const PixelType f2);
};


} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFuzzyConnectednessRGBImageFilter.txx"
#endif

#endif
