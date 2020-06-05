/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVarianceImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2008-10-16 19:33:45 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVarianceImageFilter_h
#define __itkVarianceImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkNumericTraits.h"

namespace itk
{
/** \class VarianceImageFilter
 * \brief Calculate the local Variance in an image.
 *
 * Computes an image where a given output pixel is computed as the
 * variance of the input pixels in a neighborhood about the corresponding
 * input pixel. Zero pixels can be excluded by setting a mask with SetMask(). 
 * The mask is assumed to be of the same size as the input image.
 *
 * \sa Image
 * \sa Neighborhood
 * \sa NeighborhoodOperator
 * \sa NeighborhoodIterator
 * 
 * \ingroup IntensityImageFilters
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT VarianceImageFilter :
    public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Convenient typedefs for simplifying declarations. */
  typedef TInputImage  InputImageType;
  typedef typename InputImageType::Pointer InputImagePointerType;
  typedef TOutputImage OutputImageType;

  /** Standard class typedefs. */
  typedef VarianceImageFilter                                  Self;
  typedef ImageToImageFilter< InputImageType, OutputImageType> Superclass;
  typedef SmartPointer<Self>                                   Pointer;
  typedef SmartPointer<const Self>                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VarianceImageFilter, ImageToImageFilter);
  
  /** Image typedef support. */
  typedef typename InputImageType::PixelType               InputPixelType;
  typedef typename OutputImageType::PixelType              OutputPixelType;
  typedef typename NumericTraits<InputPixelType>::RealType InputRealType;
  
  typedef typename InputImageType::RegionType  InputImageRegionType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;

  typedef typename InputImageType::SizeType InputSizeType;

  /** Set the radius of the neighborhood used to compute the Variance.
   * In general, the radius should be small.  But if set to one, the
   * confidence in the estimates will be marginal. */
  itkSetMacro(Radius, InputSizeType);

  /** Get the radius of the neighborhood used to compute the Variance */
  itkGetConstReferenceMacro(Radius, InputSizeType);
  
  /** Set the input mask image */
  itkSetObjectMacro(Mask, InputImageType);
  
  /** Get the input mask image */
  itkGetObjectMacro(Mask, InputImageType);
  
  /** VarianceImageFilter needs a larger input requested region than
   * the output requested region.  As such, VarianceImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion()
   * in order to inform the pipeline execution model.
   *
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputHasNumericTraitsCheck,
                  (Concept::HasNumericTraits<InputPixelType>));
  /** End concept checking */
#endif

protected:
  VarianceImageFilter();
  virtual ~VarianceImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const ITK_OVERRIDE;

  /** VarianceImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData()
   * routine which is called for each processing thread. The output
   * image data is allocated automatically by the superclass prior to
   * calling ThreadedGenerateData().  ThreadedGenerateData can only
   * write to the portion of the output image specified by the
   * parameter "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData() */
  virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId) ITK_OVERRIDE;

private:
  VarianceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputSizeType m_Radius;
  InputImagePointerType m_Mask;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVarianceImageFilter.hxx"
#endif

#endif

