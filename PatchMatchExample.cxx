#include <vtkSmartPointer.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkJPEGReader.h>
#include <vtkImageActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkCommand.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkExtractVOI.h>
#include <vtkImageCorrelation.h>
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageNormalize.h>

#include <cmath>

#include "vtkInpaintingCriminisi.h"

int main(int argc, char *argv[])
{
  if(argc != 2)
    {
    std::cerr << "Required arguments: image.jpg" << std::endl;
    return EXIT_FAILURE;
    }
  std::string imageFilename = argv[1];

  vtkSmartPointer<vtkImageReader2Factory> readerFactory =
    vtkSmartPointer<vtkImageReader2Factory>::New();
  vtkImageReader2* imageReader = readerFactory->CreateImageReader2(imageFilename.c_str());
  imageReader->SetFileName(imageFilename.c_str());
  imageReader->Update();

  vtkImageData* originalImage = imageReader->GetOutput();
  int extent[6];
  originalImage->GetExtent(extent);
  std::cout << "Extent: " << extent[0] << " " << extent[1] << " " << extent[2] << " " << extent[3] << std::endl;
  std::cout << "Original type: " << originalImage->GetScalarTypeAsString() << std::endl;

  vtkSmartPointer<vtkExtractVOI> extractPatch =
    vtkSmartPointer<vtkExtractVOI>::New();
  extractPatch->SetInputConnection(imageReader->GetOutputPort());
  int demoCenter[2] = {20, 20};
  int kernelRadius = 5;
  extractPatch->SetVOI(demoCenter[0] - kernelRadius, demoCenter[0] + kernelRadius,
                       demoCenter[1] - kernelRadius, demoCenter[1] + kernelRadius, 0, 0);
  extractPatch->Update();

  // Create a red image in the location of the patch
  vtkSmartPointer<vtkImageCanvasSource2D> drawing =
    vtkSmartPointer<vtkImageCanvasSource2D>::New();
  drawing->SetScalarTypeToUnsignedChar();
  drawing->SetNumberOfScalarComponents(3);
  drawing->SetExtent(demoCenter[0] - kernelRadius, demoCenter[0] + kernelRadius,
                    demoCenter[1] - kernelRadius, demoCenter[1] + kernelRadius, 0, 0);
  drawing->SetDrawColor(255, 0, 0, 0);
  drawing->FillBox(demoCenter[0] - kernelRadius, demoCenter[0] + kernelRadius,
                   demoCenter[1] - kernelRadius, demoCenter[1] + kernelRadius);
  drawing->Update();

  vtkSmartPointer<vtkImageActor> patchLocationActor =
    vtkSmartPointer<vtkImageActor>::New();
  patchLocationActor->SetInputData(drawing->GetOutput());

  vtkImageData* queryPatch = extractPatch->GetOutput();
  std::cout << "Patch type: " << queryPatch->GetScalarTypeAsString() << std::endl;

  vtkSmartPointer<vtkImageActor> originalActor =
    vtkSmartPointer<vtkImageActor>::New();
  originalActor->SetInputData(originalImage);

  vtkSmartPointer<vtkImageActor> patchActor =
    vtkSmartPointer<vtkImageActor>::New();
  patchActor->SetInputData(queryPatch);

  vtkSmartPointer<vtkInpaintingCriminisi> inpainting =
    vtkSmartPointer<vtkInpaintingCriminisi>::New();
  int bestMatch[2];
  inpainting->FindBestPatchMatch(extractPatch->GetOutput(), imageReader->GetOutput(), bestMatch);

  std::cout << "bestMatch: " << bestMatch[0] << " " << bestMatch[1] << std::endl;

  vtkSmartPointer<vtkExtractVOI> extractBestMatch =
    vtkSmartPointer<vtkExtractVOI>::New();
  extractBestMatch->SetInputConnection(imageReader->GetOutputPort());
  extractBestMatch->SetVOI(bestMatch[0] - kernelRadius, bestMatch[0] + kernelRadius,
                           bestMatch[1] - kernelRadius, bestMatch[1] + kernelRadius, 0, 0);
  extractBestMatch->Update();

  // Create a red image in the location of the patch
  vtkSmartPointer<vtkImageCanvasSource2D> bestMatchDrawing =
    vtkSmartPointer<vtkImageCanvasSource2D>::New();
  bestMatchDrawing->SetScalarTypeToUnsignedChar();
  bestMatchDrawing->SetNumberOfScalarComponents(3);
  bestMatchDrawing->SetExtent(bestMatch[0] - kernelRadius, bestMatch[0] + kernelRadius,
                              bestMatch[1] - kernelRadius, bestMatch[1] + kernelRadius, 0, 0);
  bestMatchDrawing->SetDrawColor(0, 255, 0, 0);
  bestMatchDrawing->FillBox(bestMatch[0] - kernelRadius, bestMatch[0] + kernelRadius,
                            bestMatch[1] - kernelRadius, bestMatch[1] + kernelRadius);
  bestMatchDrawing->Update();

  vtkSmartPointer<vtkImageActor> bestMatchLocationActor =
    vtkSmartPointer<vtkImageActor>::New();
  bestMatchLocationActor->SetInputData(bestMatchDrawing->GetOutput());

  vtkSmartPointer<vtkImageActor> bestMatchActor =
    vtkSmartPointer<vtkImageActor>::New();
  bestMatchActor->SetInputData(extractBestMatch->GetOutput());

  // Compute the correlation
  unsigned char imageRange[2];
  vtkUnsignedCharArray::SafeDownCast(originalImage->GetPointData()->GetScalars())->GetValueRange(imageRange);

  unsigned char patchRange[2];
  vtkUnsignedCharArray::SafeDownCast(queryPatch->GetPointData()->GetScalars())->GetValueRange(patchRange);

  // Compute the correlation
  vtkSmartPointer<vtkImageShiftScale> imageNormalizeFilter =
    vtkSmartPointer<vtkImageShiftScale>::New();
  imageNormalizeFilter->SetShift(-imageRange[0]);
  imageNormalizeFilter->SetScale(255/(imageRange[1]-imageRange[0]));
  imageNormalizeFilter->SetInputData(originalImage);
  imageNormalizeFilter->Update();

  vtkSmartPointer<vtkImageShiftScale> patchNormalizeFilter =
    vtkSmartPointer<vtkImageShiftScale>::New();
  patchNormalizeFilter->SetShift(-patchRange[0]);
  patchNormalizeFilter->SetScale(255/(patchRange[1]-patchRange[0]));
  patchNormalizeFilter->SetInputData(queryPatch);
  patchNormalizeFilter->Update();

  vtkSmartPointer<vtkImageCorrelation> correlationFilter =
    vtkSmartPointer<vtkImageCorrelation>::New();
  correlationFilter->SetInput1Data(imageNormalizeFilter->GetOutput());
  correlationFilter->SetInput2Data(patchNormalizeFilter->GetOutput());
  correlationFilter->Update();
  std::cout << "Correlation type: " << correlationFilter->GetOutput()->GetScalarTypeAsString() << std::endl;

  float correlationRange[2];
  vtkFloatArray::SafeDownCast(correlationFilter->GetOutput()->GetPointData()->GetScalars())->GetValueRange(correlationRange);
  std::cout << "correlationRange: " << correlationRange[0] << " " << correlationRange[1] << std::endl;

  vtkSmartPointer<vtkImageShiftScale> correlationCastFilter =
    vtkSmartPointer<vtkImageShiftScale>::New();
  correlationCastFilter->SetInputConnection(correlationFilter->GetOutputPort( ));
  correlationCastFilter->SetShift(-correlationRange[0]);
  correlationCastFilter->SetScale(255 / (correlationRange[1] - correlationRange[0]));
  correlationCastFilter->SetOutputScalarTypeToUnsignedChar();
  correlationCastFilter->Update();

  vtkSmartPointer<vtkImageActor> correlationActor =
    vtkSmartPointer<vtkImageActor>::New();
  correlationActor->SetInputData(correlationCastFilter->GetOutput());

  // There will be one render window
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetSize(1200, 300);

  // And one interactor
  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  // Define viewport ranges
  // (xmin, ymin, xmax, ymax)
  double originalViewport[4] = {0.0, 0.0, 0.25, 1.0};
  double patchViewport[4] = {0.25, 0.0, 0.5, 1.0};
  double correlationViewport[4] = {0.5, 0.0, 0.75, 1.0};
  double bestMatchViewport[4] = {0.75, 0.0, 1.0, 1.0};

  // Setup both renderers
  vtkSmartPointer<vtkRenderer> originalRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(originalRenderer);
  originalRenderer->SetViewport(originalViewport);
  originalRenderer->SetBackground(.6, .5, .4);
  originalRenderer->AddActor(originalActor);
  originalRenderer->AddActor(patchLocationActor);
  originalRenderer->AddActor(bestMatchLocationActor);
  originalRenderer->ResetCamera();

  vtkSmartPointer<vtkRenderer> patchRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(patchRenderer);
  patchRenderer->SetViewport(patchViewport);
  patchRenderer->SetBackground(.4, .5, .6);
  patchRenderer->AddActor(patchActor);
  patchRenderer->ResetCamera();

  vtkSmartPointer<vtkRenderer> correlationRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(correlationRenderer);
  correlationRenderer->SetViewport(correlationViewport);
  correlationRenderer->SetBackground(.4, .5, .6);
  correlationRenderer->AddActor(correlationActor);
  correlationRenderer->ResetCamera();

  vtkSmartPointer<vtkRenderer> bestMatchRenderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderWindow->AddRenderer(bestMatchRenderer);
  bestMatchRenderer->SetViewport(bestMatchViewport);
  bestMatchRenderer->SetBackground(.4, .5, .6);
  bestMatchRenderer->AddActor(bestMatchActor);
  bestMatchRenderer->ResetCamera();

  renderWindow->Render();

  vtkSmartPointer<vtkInteractorStyleImage> style =
    vtkSmartPointer<vtkInteractorStyleImage>::New();
  interactor->SetInteractorStyle(style);

  interactor->Start();

  imageReader->Delete();
  return EXIT_SUCCESS;
}