
#include <chrono>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVolumeToDepthMapFilter.h"

int main(int argc, char **argv)
{
  if (argc < 3)
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " InputImage OutputImage [Dimension | Peak | Tolerance | initialisation | Workers]" << std::endl;
    return EXIT_FAILURE;
    }

  using PixelType = unsigned char;
  using VolumeType = itk::Image<PixelType, 3>;
  using VolumeReaderType = itk::ImageFileReader<VolumeType>;
  using VolumeToDepthMapFilterType = itk::VolumeToDepthMapFilter<VolumeType, VolumeType>;
  using ImageWriterType = itk::ImageFileWriter<VolumeType>;

  VolumeReaderType::Pointer reader = VolumeReaderType::New();
  reader->SetFileName(argv[1]);
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &excp)
    {
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  auto start = std::chrono::high_resolution_clock::now();
  VolumeToDepthMapFilterType::Pointer filter = VolumeToDepthMapFilterType::New();

  filter->SetInput(reader->GetOutput());
  if (argc >= 4)
    {
    filter->SetProjectionDimension(std::atoi(argv[3]));
    }
  if (argc >= 5)
    {
    filter->SetPeak(std::atoi(argv[4]));
    }
  if (argc >= 6)
    {
    filter->SetTolerance(std::atoi(argv[5]));
    }
  if (argc >= 7)
    {
    if (std::atoi(argv[6]) == 1)
      {
      VolumeType::Pointer Initialisation = VolumeType::New();
      VolumeType::IndexType start = {{0, 0, 0}};
      VolumeType::SizeType size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
      PixelType value = static_cast<unsigned char>(size[std::atoi(argv[3])] * 0.5);
      size[std::atoi(argv[3])] = 1;
      VolumeType::RegionType region;
      region.SetSize(size);
      region.SetIndex(start);
      Initialisation->SetRegions(region);
      Initialisation->Allocate();
      Initialisation->FillBuffer(value);
      filter->SetInitialisation(Initialisation);
      }
    }
  if (argc >= 8)
    {
    filter->SetNumberOfWorkUnits(std::atoi(argv[7]));
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
