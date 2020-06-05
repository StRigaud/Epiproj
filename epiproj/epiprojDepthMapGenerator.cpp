
#include <iostream>

#include "itkImageIOBase.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkMultiscaleVolumeToDepthMapFilter.h"
#include "itkVarianceImageFilter.h"

int main(int argc, char **argv)
{

  if (argc < 4)
  {
    std::cerr << "Epiproj - Stephane Rigaud {stephane.rigaud@pasteur.fr}";
    std::cerr << ", Compiled : " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cerr << "Usage: " << argv[0] << std::endl;
    std::cerr << "\tInputFileName  (string) - path to input file." << std::endl;
    std::cerr << "\tOutputFileName (string) - path to output file." << std::endl;
    std::cerr << "\tSigma (float)           - smoothing parameters." << std::endl;
    std::cerr << "Options: " << std::endl;
    std::cerr << "\tType (string)     - Computation on maximum (max) or variance (var) intensity." << std::endl;
    std::cerr << "\tLevel (int)       - Number of scaling level. (=5)" << std::endl;
    std::cerr << "\tPeak (int)        - Detecting peak. (=0)" << std::endl;
    std::cerr << "\tTolerance (float) - Intensity ratio (=0.1)." << std::endl;
    std::cerr << "\tDelta (int)       - Degree of freedom per step. (=1)" << std::endl;
    return EXIT_FAILURE;
  }

  /*
   * Parameters  
   */
  std::string inputFileName = argv[1];
  std::string outputFileName = argv[2];
  float sigma = std::atoi(argv[3]);
  
  /*
   * Optional parameters
   */
  std::string processing = "max";
  if (argc >= 5)
  {
    processing = argv[4];
  }
  unsigned int scalingFactor = 5;
  if (argc >= 6)
  {
    scalingFactor = std::atoi(argv[5]);
  }
  unsigned int peak = 0;
  if (argc >= 7)
  {
    peak = std::atoi(argv[6]);
  }
  float tolerance = 0.1;
  if (argc >= 8)
  {
    tolerance = std::atoi(argv[7]);
  }
  unsigned int delta = 1;
  if (argc >= 9)
  {
    delta = std::atoi(argv[8]);
  }

  /*
   *  Define typedef.
   */
  const unsigned int Dimension = 3;
  using InputImageType = itk::Image<float, Dimension>;
  using InternatImageType = itk::Image<float, Dimension - 1>;
  using OutputImageType = itk::Image<unsigned char, Dimension - 1>;
  using ImageReaderType = itk::ImageFileReader<InputImageType>;
  using ImageWriterType = itk::ImageFileWriter<OutputImageType>;
  using VarianceImageFilterType = itk::VarianceImageFilter<InputImageType, InputImageType>;
  using DepthMapImageFilterType = itk::MultiscaleVolumeToDepthMapFilter<InputImageType, InternatImageType>;
  using GaussianFilterType = itk::SmoothingRecursiveGaussianImageFilter<InternatImageType, InternatImageType>;
  using CastImageFilterType = itk::CastImageFilter<InternatImageType, OutputImageType>;

  /*
   * Input verification.  
   */
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(inputFileName.c_str(), itk::ImageIOFactory::ReadMode);
  imageIO->SetFileName(inputFileName);
  imageIO->ReadImageInformation();
  if (imageIO->GetNumberOfDimensions() != 3)
  {
    std::cerr << "Error: Expected input should be of dimension " << Dimension;
    std::cerr << ", instead of dimension " << imageIO->GetNumberOfDimensions() << std::endl;
    return EXIT_FAILURE;
  }

  /*
   *  Filters declaration.
   */
  ImageReaderType::Pointer reader = ImageReaderType::New();
  DepthMapImageFilterType::Pointer depthMapFilter = DepthMapImageFilterType::New();
  GaussianFilterType::Pointer gaussianFilter = GaussianFilterType::New();
  CastImageFilterType::Pointer castFilter = CastImageFilterType::New();
  ImageWriterType::Pointer writer = ImageWriterType::New();

  /*
   *  Define pipeline.
   */
  reader->SetFileName(inputFileName);
  reader->SetImageIO(imageIO);
  if (processing.compare("var") == 0)
  {
    VarianceImageFilterType::InputSizeType kernel;
    kernel.Fill(15);
    kernel[Dimension - 1] = 0;
    VarianceImageFilterType::Pointer varianceFilter = VarianceImageFilterType::New();
    varianceFilter->SetInput(reader->GetOutput());
    varianceFilter->SetRadius(kernel);
    try
    {
      varianceFilter->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
    depthMapFilter->SetInput(varianceFilter->GetOutput());
  }
  else if (processing.compare("max") == 0)
  {
    depthMapFilter->SetInput(reader->GetOutput());
  }
  else
  {
    depthMapFilter->SetInput(reader->GetOutput());
  }
  depthMapFilter->SetNumberOfLevels(scalingFactor);
  depthMapFilter->SetSigma(delta);
  depthMapFilter->SetPeak(peak);
  depthMapFilter->SetTolerance(tolerance);
  gaussianFilter->SetInput(depthMapFilter->GetOutput());
  gaussianFilter->SetSigma(sigma);
  castFilter->SetInput(gaussianFilter->GetOutput());
  writer->SetFileName(outputFileName);
  writer->SetInput(castFilter->GetOutput());

  /*
   *  Update and execute pipeline.
   */
  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject &excp)
  {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
  }

  /** That's all folks! **/
  return EXIT_SUCCESS;
}