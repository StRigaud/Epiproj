#ifndef __itkVolumeToDepthMapFilter_hxx
#define __itkVolumeToDepthMapFilter_hxx

#include "itkVolumeToDepthMapFilter.h"

#include <vector>
#include <algorithm>

#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkProgressReporter.h"

namespace itk
{

template <class TInputImage, class TOutputImage>
VolumeToDepthMapFilter<TInputImage, TOutputImage>
::VolumeToDepthMapFilter()
{
  m_Initialisation = nullptr;
  m_ProjectionDimension = InputImageDimension - 1;
  m_Range.Fill(0);
  m_Tolerance = 0.0;
  m_Peak = 0;
}

template <class TInputImage, class TOutputImage>
void 
VolumeToDepthMapFilter<TInputImage, TOutputImage>
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

template <class TInputImage, class TOutputImage>
void 
VolumeToDepthMapFilter<TInputImage, TOutputImage>
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
    // Define Index, Size, Spacing, and Origin for both Input and Output. **/
    OutputIndexType outputIndex = this->GetOutput()->GetRequestedRegion().GetIndex();
    OutputSizeType outputSize = this->GetOutput()->GetRequestedRegion().GetSize();
    InputSizeType inputLargeSize = this->GetInput()->GetLargestPossibleRegion().GetSize();
    InputIndexType inputLargeIndex = this->GetInput()->GetLargestPossibleRegion().GetIndex();
    InputSizeType inputSize;
    InputIndexType inputIndex;

    // Get the LargestPossibleRegion of the output.
    // Reduce the size of the dimension to be projected if needed. 
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

template <class TInputImage, class TOutputImage>
typename TInputImage::IndexValueType
VolumeToDepthMapFilter<TInputImage, TOutputImage>
::GetPeak(std::vector<typename TInputImage::PixelType> &A, std::vector<typename TInputImage::IndexValueType> &B)
{
  InputIndexValueType result = -1;
  if (m_Peak > 0)
    {
    int mx_pos = 0;
    int mn_pos = 0;
    float mx = A[0];
    float mn = A[0];
    bool emi_first = true;
    std::vector<InputIndexValueType> peakList;
    std::vector<InputIndexValueType> negPeakList;
    for (size_t i = 1; i < A.size(); i++)
      {
      if (A[i] > mx)
        {
        mx_pos = i;
        mx = A[i];
        }
      if (A[i] < mn)
        {
        mn_pos = i;
        mn = A[i];
        }
      if (emi_first && A[i] < mx - m_Tolerance)
        {
        peakList.push_back(mx_pos);
        emi_first = false;
        i = mx_pos - 1;
        mn = A[mx_pos];
        mn_pos = mx_pos;
        }
      else if ((!emi_first) && A[i] > mn + m_Tolerance)
        {
        negPeakList.push_back(mn_pos);
        emi_first = true;
        i = mn_pos - 1;
        mx = A[mn_pos];
        mx_pos = mn_pos;
        }
      }
      if (!peakList.empty())
        {
        if (m_Peak == 1)
          {
          result = B[peakList.front()];
          }
        else if (m_Peak == 2)
          {
          result = B[peakList.back()];
          }
        }
    }
  if (m_Peak == 0 || result == -1)
    {
    auto ite = std::max_element(A.begin(), A.end());
    result = B[ite - A.begin()];
    }
  return result;
}

template <class TInputImage, class TOutputImage>
void 
VolumeToDepthMapFilter<TInputImage, TOutputImage>
::DynamicThreadedGenerateData(const OutputRegionType &outputRegionForThread)
{
  if (m_ProjectionDimension >= InputImageDimension)
    {
    itkExceptionMacro(<< "Invalid ProjectionDimension "
                      << m_ProjectionDimension
                      << " but ImageDimension is "
                      << InputImageDimension);
    }

  // Use the output image to report the progress. 
  ProgressReporter progress(this, this->GetNumberOfWorkUnits(), outputRegionForThread.GetNumberOfPixels());

  // Get some values, to simplify future manipulation of input. 
  InputImagePointer input = InputImageType::New();
  input->Graft(this->GetInput());
  InputRegionType inputRegion = input->GetLargestPossibleRegion();
  InputSizeType inputSize = inputRegion.GetSize();
  InputIndexType inputIndex = inputRegion.GetIndex();

  // Get some values, to simplify future manipulation of output. 
  OutputImagePointer output = OutputImageType::New();
  output->Graft(this->GetOutput());
  OutputRegionType outputRegion = output->GetLargestPossibleRegion();
  OutputSizeType outputSizeForThread = outputRegionForThread.GetSize();
  OutputIndexType outputIndexForThread = outputRegionForThread.GetIndex();

  // Manage initialisation map if provided.
  OutputImagePointer initialisationMap = nullptr;
  if (this->GetInitialisation())
    {
    initialisationMap = OutputImageType::New();
    initialisationMap->Graft(this->GetInitialisation());
    }

  // Compute the input region for this thread.
  InputRegionType inputRegionForThread = inputRegion;
  InputSizeType inputSizeForThread = inputSize;
  InputIndexType inputIndexForThread = inputIndex;
  if (static_cast<unsigned int>(InputImageDimension) == static_cast<unsigned int>(OutputImageDimension))
    {
    for (size_t i = 0; i < InputImageDimension; i++)
      {
      if (i != m_ProjectionDimension)
        {
        inputSizeForThread[i] = outputSizeForThread[i];
        inputIndexForThread[i] = outputIndexForThread[i];
        }
      }
    }
  else
    {
    for (size_t i = 0; i < OutputImageDimension; i++)
      {
      if (i != m_ProjectionDimension)
        {
        inputSizeForThread[i] = outputSizeForThread[i];
        inputIndexForThread[i] = outputIndexForThread[i];
        }
      else
        {
        inputSizeForThread[InputImageDimension - 1] = outputSizeForThread[i];
        inputIndexForThread[InputImageDimension - 1] = outputIndexForThread[i];
        }
      }
      inputSizeForThread[m_ProjectionDimension] = inputSize[m_ProjectionDimension];
      inputIndexForThread[m_ProjectionDimension] = inputIndex[m_ProjectionDimension];
    }
  inputRegionForThread.SetSize(inputSizeForThread);
  inputRegionForThread.SetIndex(inputIndexForThread);
  SizeValueType projectionSize = inputSize[m_ProjectionDimension];

  // we define an iterator.
  using InputIteratorType = ImageLinearConstIteratorWithIndex<InputImageType>;
  InputIteratorType inputIte(input, inputRegionForThread);
  inputIte.SetDirection(m_ProjectionDimension);
  inputIte.GoToBegin();

  // for each (x,y) coordinate of input.
  while (!inputIte.IsAtEnd())
    {
    // Get current index for both Input and Output.
    OutputIndexType outputIndex;
    outputIndex.Fill(0);
    InputIndexType inputIndex = inputIte.GetIndex();
    for (size_t i = 0; i < InputImageDimension; i++)
      {
      if (i != m_ProjectionDimension)
        {
        outputIndex[i] = inputIndex[i];
        }
      }

    // Define the depth range to process to search. 
    unsigned int highDepth;
    unsigned int lowDepth;
    if (initialisationMap.IsNotNull())
      {
      int previousDepth = static_cast<int>(initialisationMap->GetPixel(outputIndex));
      highDepth = static_cast<int>(previousDepth - m_Range[0]);
      highDepth = std::max<int>(highDepth, 0);
      highDepth = std::min<int>(highDepth, projectionSize - 1);
      lowDepth = static_cast<int>(previousDepth + m_Range[1]);
      lowDepth = std::max<int>(lowDepth, 0);
      lowDepth = std::min<int>(lowDepth, projectionSize - 1);
      }
    else
      {
      highDepth = 0;
      lowDepth = projectionSize - 1;
      }

    // Accumulate the values and corresponding depth in vectors.
    std::vector<InputPixelType> valueList;
    std::vector<InputIndexValueType> depthList;
    while (!inputIte.IsAtEndOfLine())
      {
      if (inputIte.GetIndex()[m_ProjectionDimension] >= highDepth &&
          inputIte.GetIndex()[m_ProjectionDimension] <= lowDepth)
        {
        valueList.push_back(inputIte.Get());
        depthList.push_back(inputIte.GetIndex()[m_ProjectionDimension]);
        }
      ++inputIte;
      }

    // Get peak depth position.
    InputIndexValueType depthValue = GetPeak(valueList, depthList);

    // Set output index value with detected depth value.
    output->SetPixel(outputIndex, static_cast<OutputPixelType>(depthValue));

    // Update progress.
    progress.CompletedPixel();

    // Go to the next (x,y) coordinate.
    inputIte.NextLine();
    }
}

} // namespace itk

#endif
