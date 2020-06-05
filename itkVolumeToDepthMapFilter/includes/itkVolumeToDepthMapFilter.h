#ifndef __itkVolumeToDepthMapFilter_h
#define __itkVolumeToDepthMapFilter_h

#include "itkImageToImageFilter.h"
#include "itkArray2D.h"

namespace itk
{

/** \class VolumeToDepthMapFilter
 * \brief Compute depth map of a volume.
 *
 * Filter that detect relevant signal in a volume along a dimension (default 3rd) 
 * return the corresponding depth map of the signal in the volume.
 * An initialisation map can be provided to speed up and restrict the computation.
 *
 * \author Stephane U. Rigaud (stephane.rigaud@pasteur.fr)
 */
template <class TInputImage, class TOutputImage>
class VolumeToDepthMapFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(VolumeToDepthMapFilter);

  /** Standard class typedefs. **/
  using Self = VolumeToDepthMapFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. **/
  itkNewMacro(Self);

  /** Run-time type information (and related methods). **/
  itkTypeMacro(VolumeToDepthMapFilter, ImageToImageFilter);

  /** Method for getting I/O dimensions **/
  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TOutputImage::ImageDimension;

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

  using ArrayType = FixedArray<InputIndexValueType, 2>;

  itkSetMacro(ProjectionDimension, unsigned int);
  itkSetMacro(Range, ArrayType);
  itkSetMacro(Tolerance, float);
  itkSetMacro(Peak, unsigned int);

  itkGetConstReferenceMacro(ProjectionDimension, unsigned int);
  itkGetMacro(Range, ArrayType);
  itkGetMacro(Tolerance, float);
  itkGetMacro(Peak, unsigned int);

  // itkSetInputMacro(Input, InputImageType);
  // itkGetInputMacro(Input, InputImageType);
  // itkSetInputMacro(Initialisation, OutputImageType);
  // itkGetInputMacro(Initialisation, OutputImageType);

  itkSetMacro(Initialisation, OutputImagePointer);
  itkGetMacro(Initialisation, OutputImagePointer);

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro(ImageDimensionCheck, (Concept::SameDimensionOrMinusOne<
                                            itkGetStaticConstMacro(InputImageDimension),
                                            itkGetStaticConstMacro(OutputImageDimension)>));
  // End concept checking
#endif

protected:
  VolumeToDepthMapFilter();
  ~VolumeToDepthMapFilter() override = default;

  /** Manage the loss of dimension between input and output **/
  void GenerateOutputInformation() override;
  void GenerateInputRequestedRegion() override;

  /** Does the real work. **/
  void DynamicThreadedGenerateData(const OutputRegionType &) override;

  /** Internal methods. **/
  InputIndexValueType GetPeak(std::vector<InputPixelType> &, std::vector<InputIndexValueType> &);

private:
  float m_Tolerance;
  ArrayType m_Range;
  unsigned int m_Peak;
  unsigned int m_ProjectionDimension;
  OutputImagePointer m_Initialisation;
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVolumeToDepthMapFilter.hxx"
#endif

#endif // __itkVolumeToDepthMapFilter_h
