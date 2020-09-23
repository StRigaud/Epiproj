
#include <iostream>

#include "itkImageIOBase.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkDepthMapProjectionFilter.h"
#include "itkMedianImageFilter.h"

int main(int argc, char **argv)
{

  if (argc < 4)
  {
    std::cerr << "Epiproj - Stephane Rigaud {stephane.rigaud@pasteur.fr}";
    std::cerr << ", Compiled : " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cerr << "Usage: " << argv[0] << std::endl;
    std::cerr << "\tInputFileName  (string) - path to input file." << std::endl;
    std::cerr << "\tDepthFileName (string)  - path to depth map file." << std::endl;
    std::cerr << "\tOutputFileName (string) - path to output file." << std::endl;
    std::cerr << "Options: " << std::endl;
    std::cerr << "\tMedian (int)      - Median radius kernel. (=0)" << std::endl;
    std::cerr << "\tType (string)     - Projection type, maximum (max), average (avg) intensity." << std::endl;
    std::cerr << "\tupperRange (int)  - Upper range band. (=1)" << std::endl;
    std::cerr << "\tlowerRange (int)  - Lower range band. (=1)" << std::endl;
    std::cerr << "\tshift (int)       - Depth shift. (=0)" << std::endl;
    return EXIT_FAILURE;
  }

  /*
   * Parameters  
   */
  std::string inputFileName = argv[1];
  std::string depthFileName = argv[2];
  std::string outputFileName = argv[3];

  /*
   * Optional parameters
   */
  unsigned int radius = 0;
  if (argc >= 5)
  {
    radius = std::atoi(argv[4]);
  }
  std::string processing = "max";
  if (argc >= 6)
  {
    processing = argv[5];
  }
  unsigned int upperRange = 1;
  if (argc >= 7)
  {
    upperRange = std::atoi(argv[6]);
  }
  unsigned int lowerRange = 1;
  if (argc >= 8)
  {
    lowerRange = std::atoi(argv[7]);
  }
  unsigned int shift = 0;
  if (argc >= 9)
  {
    shift = std::atoi(argv[8]);
  }

  /*
   *  Define typedef.
   */
  const unsigned int Dimension = 3;
  using InputImageType = itk::Image<float, Dimension>;
  using InternatImageType = itk::Image<float, Dimension - 1>;
  using OutputImageType = itk::Image<unsigned short, Dimension - 1>;
  using ImageReaderType = itk::ImageFileReader<InputImageType>;
  using DepthMapReaderType = itk::ImageFileReader<InternatImageType>;
  using ImageWriterType = itk::ImageFileWriter<OutputImageType>;
  using DepthMapProjectionFilterType = itk::DepthMapProjectionFilter<InputImageType, InternatImageType, OutputImageType>;
  using ArrayType = typename DepthMapProjectionFilterType::ArrayType;
  using MedianFilterType = itk::MedianImageFilter<InputImageType, InputImageType>;

  /*
   * Input verification.  
   */
  itk::ImageIOBase::Pointer inputImageIO = itk::ImageIOFactory::CreateImageIO(inputFileName.c_str(), itk::ImageIOFactory::ReadMode);
  inputImageIO->SetFileName(inputFileName);
  inputImageIO->ReadImageInformation();
  if (inputImageIO->GetNumberOfDimensions() != Dimension)
  {
    std::cerr << "Error: Expected input 1 should be of dimension " << Dimension;
    std::cerr << ", instead of dimension " << inputImageIO->GetNumberOfDimensions() << std::endl;
    return EXIT_FAILURE;
  }

  itk::ImageIOBase::Pointer depthImageIO = itk::ImageIOFactory::CreateImageIO(depthFileName.c_str(), itk::ImageIOFactory::ReadMode);
  depthImageIO->SetFileName(depthFileName);
  depthImageIO->ReadImageInformation();
  if (depthImageIO->GetNumberOfDimensions() != (Dimension - 1))
  {
    std::cerr << "Error: Expected input 2 should be of dimension " << Dimension - 1;
    std::cerr << ", instead of dimension " << depthImageIO->GetNumberOfDimensions() << std::endl;
    return EXIT_FAILURE;
  }

  /*
   *  Filters declaration.
   */
  ImageReaderType::Pointer reader = ImageReaderType::New();
  DepthMapReaderType::Pointer reader2 = DepthMapReaderType::New();
  DepthMapProjectionFilterType::Pointer projectionFilter = DepthMapProjectionFilterType::New();
  ImageWriterType::Pointer writer = ImageWriterType::New();

  /*
   *  Define pipeline.
   */
  reader->SetFileName(inputFileName);
  reader->SetImageIO(inputImageIO);
  reader2->SetFileName(depthFileName);
  reader2->SetImageIO(depthImageIO);

  if (radius)
  {
    MedianFilterType::InputSizeType kernel;
    kernel.Fill(radius);
    MedianFilterType::Pointer median = MedianFilterType::New();
    median->SetInput(reader->GetOutput());
    median->SetRadius(kernel);
    try
    {
      median->Update();
    }
    catch (itk::ExceptionObject &excp)
    {
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }
    projectionFilter->SetInput(median->GetOutput());
  }
  else
  {
    projectionFilter->SetInput(reader->GetOutput());
  }

  projectionFilter->SetMap(reader2->GetOutput());
  projectionFilter->SetType(processing);
  projectionFilter->SetShift(shift);
  ArrayType rangeArray;
  rangeArray[0] = upperRange;
  rangeArray[1] = lowerRange;
  projectionFilter->SetRange(rangeArray);

  writer->SetFileName(outputFileName);
  writer->SetInput(projectionFilter->GetOutput());

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