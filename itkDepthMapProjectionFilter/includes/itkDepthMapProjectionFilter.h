#ifndef __itkDepthMapProjectionFilter_h
#define __itkDepthMapProjectionFilter_h

#include "itkImageToImageFilter.h"

namespace itk
{
  
/** \class DepthMapProjectionFilter
 * \brief Volume projection using depth map.
 *
 * Filter that project a volume intensity along a dimension (default 3rd)
 * using a provided depth map. The filter allows multiple projection type.
 *
 * \author Stephane U. Rigaud (stephane.rigaud@pasteur.fr)
 */  
template <class TInputImage, class TMapImage, class TOutputImage>
class DepthMapProjectionFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(DepthMapProjectionFilter);

  /** Standard class typedef. */
  using Self = DepthMapProjectionFilter;
  using Superclass = ImageToImageFilter<TInputImage, TOutputImage>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DepthMapProjectionFilter, ImageToImageFilter);

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

  using MapImageType = TMapImage;
  using MapImagePointer = typename MapImageType::Pointer;
  using MapImageConstPointer = typename MapImageType::ConstPointer;
  using MapPixelType = typename MapImageType::PixelType;
  using MapRegionType = typename MapImageType::RegionType;
  using MapIndexType = typename MapImageType::IndexType;
  using MapSizeType = typename MapImageType::SizeType;
  using MapSpacingType = typename MapImageType::SpacingType;
  using MapPointType = typename MapImageType::PointType;
  using MapIndexValueType = typename MapImageType::IndexValueType;

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

  using ArrayType = FixedArray<int, 2>;

  itkSetMacro(Range, ArrayType);
  itkSetMacro(Shift, int);
  itkSetStringMacro(Type);

  itkGetMacro(Range, ArrayType);
  itkGetMacro(Shift, int);
  itkGetStringMacro(Type);

  itkGetConstReferenceMacro(ProjectionDimension, unsigned int);

  itkSetInputMacro(Input, InputImageType);
  itkGetInputMacro(Input, InputImageType);
  itkSetInputMacro(Map, MapImageType);
  itkGetInputMacro(Map, MapImageType);

protected:
  DepthMapProjectionFilter();
  ~DepthMapProjectionFilter() override = default;

  /** Manage the loss of dimension between input and output **/
  void GenerateOutputInformation() override;
  void GenerateInputRequestedRegion() override;

  /** Does the real work. */
  void DynamicThreadedGenerateData(const OutputRegionType &) override;

private:
  float m_Sigma;
  ArrayType m_Range;
  int m_Shift;
  std::string m_Type;
  unsigned int m_ProjectionDimension;
};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDepthMapProjectionFilter.hxx"
#endif

#endif // __itkMultiscaleVolumeDataToDepthMapFilter_h
