#include <chrono>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDepthMapProjectionFilter.h"

int main(int argc, char **argv)
{
  if (argc < 4)
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " InputImage DepthImage OutputImage " << std::endl;
    return EXIT_FAILURE;
   }

  using PixelType = unsigned char;
  using ImageType = itk::Image<PixelType, 3>;
  using ImageReaderType = itk::ImageFileReader<ImageType>;
  using ImageWriterType = itk::ImageFileWriter<ImageType>;
  using FilterType = itk::DepthMapProjectionFilter<ImageType, ImageType, ImageType>;

  ImageReaderType::Pointer reader1 = ImageReaderType::New();
  reader1->SetFileName(argv[1]);
  try
    {
    reader1->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  ImageReaderType::Pointer reader2 = ImageReaderType::New();
  reader2->SetFileName(argv[2]);
  try
    {
    reader2->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
   }

  auto start = std::chrono::high_resolution_clock::now();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(reader1->GetOutput());
  filter->SetMap(reader2->GetOutput());
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

  ImageWriterType::Pointer writer = ImageWriterType::New();
  writer->SetInput(filter->GetOutput());
  writer->SetFileName(argv[3]);
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
