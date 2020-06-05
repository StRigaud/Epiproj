#ifndef __itkMultiscaleVolumeToDepthMapFilter_h
#define __itkMultiscaleVolumeToDepthMapFilter_h

#include "itkImageToImageFilter.h"
#include "itkMultiResolutionPyramidImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkVolumeToDepthMapFilter.h"

namespace itk
{

/** \class MultiscaleVolumeToDepthMapFilter
 * \brief Compute depth map of a volume using multiscale pyramide.
 *
 * Filter that detect relevant signal in a volume along a dimension (default 3rd) 
 * return the corresponding depth map of the signal in the volume.
 * A multiscale resolution pyramid is use to compute the depth map at each scale and
 * use the previous scale as an initialisation step.
 *
 * \author Stephane U. Rigaud (stephane.rigaud@pasteur.fr)
 */
template <class TInputImage, class TOutputImage>
class MultiscaleVolumeToDepthMapFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(MultiscaleVolumeToDepthMapFilter);

  /** Standard class typedef. */
  using Self = MultiscaleVolumeToDepthMapFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(MultiscaleVolumeToDepthMapFilter, ImageToImageFilter);

  /** Method for getting I/O dimensions **/
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TOutputImage::ImageDimension;

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro(ImageDimensionCheck, (Concept::SameDimensionOrMinusOne<
                                            itkGetStaticConstMacro(InputImageDimension),
                                            itkGetStaticConstMacro(OutputImageDimension)>));
  // End concept checking
#endif

  using InputImageType = TInputImage;
  using InputImagePointer = typename InputImageType::Pointer;
  using InputImageConstPointer = typename InputImageType::ConstPointer;
  using InputPixelType = typename InputImageType::PixelType;
  using InputRegionType = typename InputImageType::RegionType;
  using InputIndexType = typename InputImageType::IndexType;
  using InputSizeType = typename InputImageType::SizeType;
  using InputSpacingType = typename InputImageType::SpacingType;
  using InputPointType = typename InputImageType::PointType;
  using InputIndexValueType = typename InputImageType::IndexValueType;

  using OutputImageType = TOutputImage;
  using OutputImagePointer = typename OutputImageType::Pointer;
  using OutputImageConstPointer = typename OutputImageType::ConstPointer;
  using OutputPixelType = typename OutputImageType::PixelType;
  using OutputRegionType = typename OutputImageType::RegionType;
  using OutputIndexType = typename OutputImageType::IndexType;
  using OutputSizeType = typename OutputImageType::SizeType;
  using OutputSpacingType = typename OutputImageType::SpacingType;
  using OutputPointType = typename OutputImageType::PointType;
  using OutputIndexValueType = typename OutputImageType::IndexValueType;

  using InternalImageType = Image<float, OutputImageDimension>;
  using InternalCastFilterType = CastImageFilter<OutputImageType, InternalImageType>;
  using OutputCastFilterType = CastImageFilter<InternalImageType, OutputImageType>;

  using MultiResolutionPyramidImageFilterType = MultiResolutionPyramidImageFilter<InputImageType, InputImageType>;
  using ScheduleType = typename MultiResolutionPyramidImageFilterType::ScheduleType;
  using VolumeToDepthMapFilterType = VolumeToDepthMapFilter<InputImageType, OutputImageType>;
  using RangeArrayType = typename VolumeToDepthMapFilterType::ArrayType;
  using GaussianFilterType = DiscreteGaussianImageFilter<InternalImageType, InternalImageType>;
  using SigmaArrayType = typename GaussianFilterType::ArrayType;

  using ImageInterpolatorType = BSplineInterpolateImageFunction<OutputImageType, float, float>;
  using ResampleFilterType = ResampleImageFilter<OutputImageType, OutputImageType, float, float>;
  using TransformType = IdentityTransform<float, OutputImageDimension>;

  itkSetMacro(NumberOfLevels, unsigned int);
  itkSetMacro(Schedule, ScheduleType);
  itkSetMacro(Sigma, float);
  itkSetMacro(Tolerance, float);
  itkSetMacro(Peak, unsigned int);
  itkSetMacro(Range, RangeArrayType);

  itkGetMacro(NumberOfLevels, unsigned int);
  itkGetMacro(Schedule, ScheduleType);
  itkGetMacro(Sigma, float);
  itkGetMacro(Tolerance, float);
  itkGetMacro(Peak, unsigned int);
  itkGetMacro(Range, RangeArrayType);

  itkGetConstReferenceMacro(ProjectionDimension, unsigned int);

protected:
  MultiscaleVolumeToDepthMapFilter();
  ~MultiscaleVolumeToDepthMapFilter() override = default;

  /** Manage the loss of dimension between input and output **/
  virtual void GenerateOutputInformation() override;
  virtual void GenerateInputRequestedRegion() override;

  /** Does the real work. */
  virtual void GenerateData() override;

  /** Determine compute schedule. */
  void ScheduleFromLevels();

private:
  typename MultiResolutionPyramidImageFilterType::Pointer m_MultiscalePyramideImageFilter;
  typename VolumeToDepthMapFilterType::Pointer m_DepthMapFilter;
  typename GaussianFilterType::Pointer m_GaussianFilter;
  typename InternalCastFilterType::Pointer m_InternalCastFilter;
  typename OutputCastFilterType::Pointer m_OutputCastFilter;
  typename ImageInterpolatorType::Pointer m_Interpolator;
  typename ResampleFilterType::Pointer m_ResampleFilter;
  typename TransformType::Pointer m_Transform;

  ScheduleType m_Schedule;
  float m_Sigma;
  float m_Tolerance;
  unsigned int m_NumberOfLevels;
  unsigned int m_ProjectionDimension;
  unsigned int m_Peak;
  RangeArrayType m_Range;
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiscaleVolumeToDepthMapFilter.hxx"
#endif

#endif // __itkMultiscaleVolumeDataToDepthMapFilter_h
