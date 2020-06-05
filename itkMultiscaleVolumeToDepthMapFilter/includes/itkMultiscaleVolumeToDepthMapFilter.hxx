#ifndef __itkMultiscaleVolumeToDepthMapFilter_hxx
#define __itkMultiscaleVolumeToDepthMapFilter_hxx

#include "itkMultiscaleVolumeToDepthMapFilter.h"

namespace itk
{

template <class InputImageType, class OutputImageType>
MultiscaleVolumeToDepthMapFilter<InputImageType, OutputImageType>
::MultiscaleVolumeToDepthMapFilter()
{
  m_MultiscalePyramideImageFilter = MultiResolutionPyramidImageFilterType::New();
  m_DepthMapFilter = VolumeToDepthMapFilterType::New();
  m_GaussianFilter = GaussianFilterType::New();
  m_InternalCastFilter = InternalCastFilterType::New();
  m_OutputCastFilter = OutputCastFilterType::New();

  m_Interpolator = ImageInterpolatorType::New();
  m_ResampleFilter = ResampleFilterType::New();
  m_Transform = TransformType::New();

  m_Interpolator->SetSplineOrder(3);
  m_Transform->SetIdentity();
  m_ResampleFilter->SetTransform(m_Transform);
  m_ResampleFilter->SetInterpolator(m_Interpolator);

  m_Sigma = 1.5;
  m_Tolerance = 0.5;
  m_NumberOfLevels = 3;
  m_Peak = 0;
  m_Range.Fill(2);

  m_ProjectionDimension = InputImageDimension - 1;
}

template <class InputImageType, class OutputImageType>
void 
MultiscaleVolumeToDepthMapFilter<InputImageType, OutputImageType>
::GenerateOutputInformation()
{
  if (m_ProjectionDimension >= InputImageDimension)
    {
    itkExceptionMacro(<< "Invalid ProjectionDimension. ProjectionDimension is "
                      << m_ProjectionDimension
                      << " but input ImageDimension is "
                      << InputImageDimension);
    }

  // Get pointers to the input and output.
  OutputImagePointer output = this->GetOutput();
  InputImagePointer input = const_cast<InputImageType *>(this->GetInput());

  // Define Index, Size, Spacing, and Origin for both Input and Output.
  InputIndexType inputIndex = input->GetLargestPossibleRegion().GetIndex();
  InputSizeType inputSize = input->GetLargestPossibleRegion().GetSize();
  InputSpacingType inputSpacing = input->GetSpacing();
  InputPointType inputOrigin = input->GetOrigin();

  OutputRegionType outputRegion;
  OutputSizeType outputSize;
  OutputIndexType outputIndex;
  OutputSpacingType outputSpacing;
  OutputPointType outputOrigin;

  // Set the LargestPossibleRegion of the output.
  // Reduce the size of the dimension to be projected. 
  if (static_cast<unsigned int>(InputImageDimension) == static_cast<unsigned int>(OutputImageDimension))
    {
    for (size_t i = 0; i < InputImageDimension; i++)
      {
      if (i != m_ProjectionDimension)
        {
        outputSize[i] = inputSize[i];
        outputIndex[i] = inputIndex[i];
        outputSpacing[i] = inputSpacing[i];
        outputOrigin[i] = inputOrigin[i];
        }
      else
        {
        outputSize[i] = 1;
        outputIndex[i] = 0;
        outputSpacing[i] = inputSpacing[i] * inputSize[i];
        outputOrigin[i] = inputOrigin[i] + (i - 1) * inputSpacing[i] / 2;
        }
      }
    }
  else
    {
    for (size_t i = 0; i < OutputImageDimension; i++)
      {
      if (i != m_ProjectionDimension)
        {
        outputSize[i] = inputSize[i];
        outputIndex[i] = inputIndex[i];
        outputSpacing[i] = inputSpacing[i];
        outputOrigin[i] = inputOrigin[i];
        }
      else
        {
        outputSize[i] = inputSize[InputImageDimension - 1];
        outputIndex[i] = inputIndex[InputImageDimension - 1];
        outputSpacing[i] = inputSpacing[InputImageDimension - 1];
        outputOrigin[i] = inputOrigin[InputImageDimension - 1];
        }
      }
    }

  // Apply output Size, Index, Origin, and Spacing to Output.
  outputRegion.SetSize(outputSize);
  outputRegion.SetIndex(outputIndex);
  output->SetOrigin(outputOrigin);
  output->SetSpacing(outputSpacing);
  output->SetLargestPossibleRegion(outputRegion);
}

template <class InputImageType, class OutputImageType>
void 
MultiscaleVolumeToDepthMapFilter<InputImageType, OutputImageType>
::GenerateInputRequestedRegion()
{
  if (m_ProjectionDimension >= InputImageDimension)
    {
    itkExceptionMacro(<< "Invalid ProjectionDimension "
                      << m_ProjectionDimension
                      << " but ImageDimension is "
                      << InputImageDimension);
    }

  Superclass::GenerateInputRequestedRegion();

  if (this->GetInput())
    {
    // Define Index, Size, Spacing, and Origin for both Input and Output.
    OutputIndexType outputIndex = this->GetOutput()->GetRequestedRegion().GetIndex();
    OutputSizeType outputSize = this->GetOutput()->GetRequestedRegion().GetSize();
    InputSizeType inputLargeSize = this->GetInput()->GetLargestPossibleRegion().GetSize();
    InputIndexType inputLargeIndex = this->GetInput()->GetLargestPossibleRegion().GetIndex();
    InputSizeType inputSize;
    InputIndexType inputIndex;

    // Get the LargestPossibleRegion of the output.
    // Reduce the size of the dimension to be projected. 
    if (static_cast<unsigned int>(InputImageDimension) == static_cast<unsigned int>(OutputImageDimension))
      {
      for (size_t i = 0; i < InputImageDimension; i++)
        {
        if (i != m_ProjectionDimension)
          {
          inputSize[i] = outputSize[i];
          inputIndex[i] = outputIndex[i];
          }
        else
          {
          inputSize[i] = inputLargeSize[i];
          inputIndex[i] = inputLargeIndex[i];
          }
        }
      }
    else
      {
      for (size_t i = 0; i < OutputImageDimension; i++)
        {
        if (i != m_ProjectionDimension)
          {
          inputSize[i] = outputSize[i];
          inputIndex[i] = outputIndex[i];
          }
        else
          {
          inputSize[InputImageDimension - 1] = outputSize[i];
          inputIndex[InputImageDimension - 1] = outputIndex[i];
          }
        }
      inputSize[m_ProjectionDimension] = inputLargeSize[m_ProjectionDimension];
      inputIndex[m_ProjectionDimension] = inputLargeIndex[m_ProjectionDimension];
      }

    InputRegionType RequestedRegion;
    RequestedRegion.SetSize(inputSize);
    RequestedRegion.SetIndex(inputIndex);
    InputImagePointer input = const_cast<InputImageType *>(this->GetInput());
    input->SetRequestedRegion(RequestedRegion);
    }
}

template <class InputImageType, class OutputImageType>
void 
MultiscaleVolumeToDepthMapFilter<InputImageType, OutputImageType>
::ScheduleFromLevels()
{
  Vector<unsigned int, InputImageDimension> factors;
  factors.Fill(1 << (m_NumberOfLevels - 1));
  m_Schedule.SetSize(m_NumberOfLevels, InputImageDimension);
  for (unsigned int k = 0; k < m_NumberOfLevels; k++)
  {
  unsigned int denominator = 1 << k;
  for (unsigned int j = 0; j < InputImageDimension; j++)
    {
    if (j == m_ProjectionDimension)
      {
      m_Schedule.SetElement(k, j, 1);
      }
    else
      {
      m_Schedule.SetElement(k, j, factors[j] / denominator);
      if (m_Schedule.GetElement(k, j) == 0)
        {
        m_Schedule.SetElement(k, j, 1);
        }
      }
    }
  }
}

template <class InputImageType, class OutputImageType>
void 
MultiscaleVolumeToDepthMapFilter<InputImageType, OutputImageType>
::GenerateData()
{
  // Define Input and Output of the filter.
  InputImagePointer input = InputImageType::New();
  input->Graft(this->GetInput());
  this->AllocateOutputs();

  // Initialise variance array and set projection dimention to 0.
  SigmaArrayType sigmaArray;
  sigmaArray.Fill(m_Sigma);
  if (InputImageDimension == OutputImageDimension)
    {
    sigmaArray[m_ProjectionDimension] = 0.0;
    }

  // Initialise variable for loop.
  OutputImagePointer previousMap = nullptr;
  InputImagePointer scaledImage = nullptr;
  OutputSizeType oldSize;
  OutputSpacingType oldSpacing;
  OutputSizeType newSize;
  OutputSpacingType newSpacing;

  const double newOrigin[OutputImageDimension] = {0};

  // Compute multiscale level factors and setup filter.
  this->ScheduleFromLevels();
  m_MultiscalePyramideImageFilter->SetInput(input);
  m_MultiscalePyramideImageFilter->SetNumberOfLevels(m_NumberOfLevels);
  m_MultiscalePyramideImageFilter->SetSchedule(m_Schedule);

  // Begin loop for each scale level.
  for (size_t level = 0; level < m_NumberOfLevels; level++)
    {
    // Get scaled input.
    try
      {
      m_MultiscalePyramideImageFilter->GetOutput(level)->Update();
      }
    catch (itk::ExceptionObject &excp)
      {
      std::cerr << excp << std::endl;
      }
    scaledImage = m_MultiscalePyramideImageFilter->GetOutput(level);

    // Define Depthmap filter.
    m_DepthMapFilter->SetInput(scaledImage);
    m_DepthMapFilter->SetTolerance(m_Tolerance);
    m_DepthMapFilter->SetPeak(m_Peak);

    // Initialisation condition.
    if (previousMap.IsNotNull())
      {
      // Define new size and spacing for upsampling depthmap.
      for (size_t d = 0; d < OutputImageDimension; d++)
        {
        if (d != m_ProjectionDimension)
          {
          oldSize[d] = previousMap->GetLargestPossibleRegion().GetSize()[d];
          oldSpacing[d] = previousMap->GetSpacing()[d];
          newSize[d] = scaledImage->GetLargestPossibleRegion().GetSize()[d];
          newSpacing[d] = oldSpacing[d] * static_cast<float>(oldSize[d]) / static_cast<float>(newSize[d]);
          }
        else
          {
          oldSize[d] = 1;
          oldSpacing[d] = 0;
          newSpacing[d] = 1;
          newSize[m_ProjectionDimension] = 1;
          }
        }
      // Define Upsampler filter to resize the previous map to the current level.
      m_ResampleFilter->SetSize(newSize);
      m_ResampleFilter->SetOutputSpacing(newSpacing);
      m_ResampleFilter->SetOutputOrigin(newOrigin);
      m_ResampleFilter->SetInput(previousMap);
      try
        {
        m_ResampleFilter->UpdateLargestPossibleRegion();
        }
      catch (itk::ExceptionObject &excp)
        {
        std::cerr << excp << std::endl;
        }
      previousMap = m_ResampleFilter->GetOutput();

      // Link upscaled map as current level initialisation.
      m_DepthMapFilter->SetRange(m_Range);
      m_DepthMapFilter->SetInitialisation(m_ResampleFilter->GetOutput());
      }

  // Gaussian regularisation filter.
  m_InternalCastFilter->SetInput(m_DepthMapFilter->GetOutput());
  m_GaussianFilter->SetInput(m_InternalCastFilter->GetOutput());
  m_GaussianFilter->SetVariance(sigmaArray);
  m_GaussianFilter->SetUseImageSpacing(false);
  m_OutputCastFilter->SetInput(m_GaussianFilter->GetOutput());
  try
    {
    m_OutputCastFilter->UpdateLargestPossibleRegion();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << excp << std::endl;
    }

  // Update map for next iteration.
  previousMap = m_OutputCastFilter->GetOutput();
  previousMap->DisconnectPipeline();
  }

  // Graft it to the pipeline output
  this->GetOutput()->Graft(previousMap);
}

} // namespace itk

#endif
