#ifndef __itkDepthMapProjectionFilter_hxx
#define __itkDepthMapProjectionFilter_hxx

#include "itkDepthMapProjectionFilter.h"
#include "itkImageLinearConstIteratorWithIndex.h"

#include "itkProgressReporter.h"

namespace itk
{

template <class TInputImage, class TMapImage, class TOutputImage>
DepthMapProjectionFilter<TInputImage, TMapImage, TOutputImage>
::DepthMapProjectionFilter()
{
  Self::SetPrimaryInputName("Input");
  Self::AddRequiredInputName("Map", 1);

  m_Range.Fill(1);
  m_Shift = 0;
  m_Type = "max";
  m_ProjectionDimension = InputImageDimension - 1;
}

template <class TInputImage, class TMapImage, class TOutputImage>
void 
DepthMapProjectionFilter<TInputImage, TMapImage, TOutputImage>
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
  InputImageConstPointer input = this->GetInput();

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

template <class TInputImage, class TMapImage, class TOutputImage>
void 
DepthMapProjectionFilter<TInputImage, TMapImage, TOutputImage>
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

template <class TInputImage, class TMapImage, class TOutputImage>
void 
DepthMapProjectionFilter<TInputImage, TMapImage, TOutputImage>
::DynamicThreadedGenerateData(const OutputRegionType &outputRegionForThread)
{
  // Use the output image to report the progress.
  ProgressReporter progress(this, this->GetNumberOfWorkUnits(), outputRegionForThread.GetNumberOfPixels());

  // Get some values, to simplify future manipulation.
  InputImageConstPointer input = this->GetInput();
  InputRegionType inputRegion = input->GetLargestPossibleRegion();
  InputSizeType inputSize = inputRegion.GetSize();
  InputIndexType inputIndex = inputRegion.GetIndex();

  OutputImagePointer output = this->GetOutput();
  OutputRegionType outputRegion = output->GetLargestPossibleRegion();
  OutputSizeType outputSizeForThread = outputRegionForThread.GetSize();
  OutputIndexType outputIndexForThread = outputRegionForThread.GetIndex();

  MapImageConstPointer map = this->GetMap();
  MapRegionType mapRegion = map->GetLargestPossibleRegion();
  MapSizeType mapSize = mapRegion.GetSize();
  MapIndexType mapIndex = mapRegion.GetIndex();

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

  // we define an iterator
  using InputIteratorType = ImageLinearConstIteratorWithIndex<InputImageType>;
  InputIteratorType inputIte(input, inputRegionForThread);
  inputIte.SetDirection(m_ProjectionDimension);
  inputIte.GoToBegin();

  // for each (x,y) couple
  while (!inputIte.IsAtEnd())
    {
    // move the ouput iterator and set the output value
    OutputIndexType outputIndex;
    MapIndexType mapIndex;
    InputIndexType inputIndex = inputIte.GetIndex();

    for (size_t i = 0; i < OutputImageDimension; i++)
      {
      if (i != m_ProjectionDimension)
        {
        outputIndex[i] = inputIndex[i];
        mapIndex[i] = inputIndex[i];
        }
      else
        {
        outputIndex[i] = 0;
        mapIndex[i] = 0;
        }
      }

    int currentDepth = map->GetPixel(mapIndex) + m_Shift;
    int highDepthValue = currentDepth - m_Range[0];
    highDepthValue = std::max<int>(highDepthValue, 0);
    highDepthValue = std::min<int>(highDepthValue, projectionSize - 1);
    int lowDepthValue = currentDepth + m_Range[1];
    lowDepthValue = std::max<int>(lowDepthValue, 0);
    lowDepthValue = std::min<int>(lowDepthValue, projectionSize - 1);

    // accumulate along the dimention
    std::vector<InputPixelType> accumulatedData;
    while (!inputIte.IsAtEndOfLine())
      {
      if (inputIte.GetIndex()[m_ProjectionDimension] >= highDepthValue &&
      inputIte.GetIndex()[m_ProjectionDimension] <= lowDepthValue)
        {
        accumulatedData.push_back(inputIte.Get());
        }
      ++inputIte;
      }

    // project according to methode
    OutputPixelType result = 0;
    if (!accumulatedData.empty())
      {
      if (m_Type.compare("max") == 0)
        {
        typename std::vector<InputPixelType>::iterator ite;
        ite = std::max_element(accumulatedData.begin(), accumulatedData.end());
        result = *ite;
        }
      else if (m_Type.compare("avg") == 0)
        {
        int sum = std::accumulate(accumulatedData.begin(), accumulatedData.end(), 0);
        result = sum / accumulatedData.size();
        }
      }

    // Set value at pixel
    output->SetPixel(outputIndex, static_cast<OutputPixelType>(result));

    // one more line done !
    progress.CompletedPixel();

    // continue with the next one
    inputIte.NextLine();
    }
}

} // namespace itk

#endif
