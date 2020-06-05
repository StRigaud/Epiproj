/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVarianceImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2009-04-06 00:19:17 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVarianceImageFilter_txx
#define __itkVarianceImageFilter_txx

#include "itkVarianceImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkConstantBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace itk
{

template <class TInputImage, class TOutputImage>
VarianceImageFilter<TInputImage, TOutputImage>
::VarianceImageFilter()
{
  this->DynamicMultiThreadingOff();
  m_Radius.Fill(1);
  m_Mask = NULL;
}

template <class TInputImage, class TOutputImage>
void 
VarianceImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer inputPtr = 
    const_cast< TInputImage * >( this->GetInput() );
  typename Superclass::OutputImagePointer outputPtr = this->GetOutput();
  
  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Radius );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
}


template< class TInputImage, class TOutputImage>
void
VarianceImageFilter< TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId)
{
  unsigned int i;
  ConstantBoundaryCondition<InputImageType> nbc;
  nbc.SetConstant(0);

  ZeroFluxNeumannBoundaryCondition<InputImageType> zbc;
  
  ConstNeighborhoodIterator<InputImageType> bit;
  ImageRegionIterator<OutputImageType> it;
  
  ConstNeighborhoodIterator<InputImageType> bit2;
  ImageRegionIterator<OutputImageType> it2;
  
  // Allocate output
  typename OutputImageType::Pointer output = this->GetOutput();
  typename  InputImageType::ConstPointer input  = this->GetInput();
  
  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType> bC;
  faceList = bC(input, outputRegionForThread, m_Radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  // support progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());
  
  InputRealType value;
  InputRealType sum;
  InputRealType mean;
  InputRealType sumOfSquares;
  InputRealType var;
  InputRealType num;

  // Process each of the boundary faces.  These are N-d regions which border
  // the edge of the buffer.
  for (fit=faceList.begin(); fit != faceList.end(); ++fit)
    { 
    if(m_Mask.IsNotNull())
      {    
      bit = ConstNeighborhoodIterator<InputImageType>(m_Radius, m_Mask, *fit);
      unsigned int neighborhoodSize = bit.Size();
      bit.OverrideBoundaryCondition(&nbc);
      bit.GoToBegin();
      
      it = ImageRegionIterator<OutputImageType>(output, *fit);
      it.GoToBegin();
      while ( ! bit.IsAtEnd() )
        {
         sum = NumericTraits<InputRealType>::Zero;
         sumOfSquares = NumericTraits<InputRealType>::Zero;
         num = 0;
      
         if(bit.GetPixel(0) != 0) 
           {
           for (i = 0; i < neighborhoodSize; ++i)
            { 
            if(bit.GetPixel(i) != 0)
              {         
              value = static_cast<InputRealType>( input->GetPixel(bit.GetIndex(i)) );
              sum += value;
              sumOfSquares += (value*value);
              ++num;
              }
            }
          }
        // calculate the variance value
        if(num < 1) 
          var = 0;

        else
          {
          mean = sum/num;
          var = (sumOfSquares/num) - (mean*mean);
          }
        it.Set( static_cast<OutputPixelType>(var) );
      
        ++bit;
        ++it;
        progress.CompletedPixel();
        }
      }
    else
      {
      bit2 = ConstNeighborhoodIterator<InputImageType>(m_Radius, input, *fit);
      unsigned int neighborhoodSize = bit2.Size();
      num = neighborhoodSize;
      bit2.OverrideBoundaryCondition(&zbc);
      bit2.GoToBegin();
      
      it2 = ImageRegionIterator<OutputImageType>(output, *fit);
      it2.GoToBegin();
      while ( ! bit2.IsAtEnd() )
        {
        sum = NumericTraits<InputRealType>::Zero;
        sumOfSquares = NumericTraits<InputRealType>::Zero;
        for (i = 0; i < neighborhoodSize; ++i)
          {          
          value = static_cast<InputRealType>( bit2.GetPixel(i) );
          sum += value;
          sumOfSquares += (value*value);
          }
        // calculate the variance value
        mean = sum/num;
        var = (sumOfSquares/num) - (mean*mean);
        it2.Set( static_cast<OutputPixelType>(var) );
      
        ++bit2;
        ++it2;
        progress.CompletedPixel();
        }
      }
    }
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutput>
void
VarianceImageFilter<TInputImage, TOutput>
::PrintSelf(
  std::ostream& os, 
  Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Radius: " << m_Radius << std::endl;

}

} // end namespace itk

#endif
