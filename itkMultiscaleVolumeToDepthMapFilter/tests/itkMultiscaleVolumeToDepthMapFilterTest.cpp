#include <chrono>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMultiscaleVolumeToDepthMapFilter.h"

int main(int argc, char **argv)
{
  if (argc < 4)
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " InputImage OutputImage NumberOfLevels [Sigma | Peak | Tolerance]" << std::endl;
    return EXIT_FAILURE;
    }

  using PixelType = unsigned char;
  using VolumeType = itk::Image<PixelType, 3>;
  using ImageType = itk::Image<PixelType, 3>;

  using VolumeReaderType = itk::ImageFileReader<VolumeType>;
  VolumeReaderType::Pointer reader = VolumeReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  auto start = std::chrono::high_resolution_clock::now();

  using FilterType = itk::MultiscaleVolumeToDepthMapFilter<VolumeType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(reader->GetOutput());
  filter->SetNumberOfLevels(std::atoi(argv[3]));
  if (argc >= 5)
    {
    filter->SetSigma(std::atoi(argv[4]));
    }
  if (argc >= 6)
    {
    filter->SetPeak(std::atoi(argv[5]));
    }
  if (argc >= 7)
    {
    filter->SetTolerance(std::atoi(argv[6]));
    }
  try
    {
    filter->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> elapsed = finish - start;

  using ImageWriterType = itk::ImageFileWriter<ImageType>;
  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetInput(filter->GetOutput());
  writer->SetFileName(argv[2]);
  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Elapsed time: " << elapsed.count() << " s" << std::endl;
  return EXIT_SUCCESS;
}
