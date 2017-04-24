/*
 * graphics.cpp
 *
 *  Created on: April 16, 2017
 *      Author: nelaturi
 */

#include "graphics.hpp"

// vtk stuff 

#include <vtkMarchingCubes.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h> 
#include <vtkDecimatePro.h>
#include <vtkSTLWriter.h>
#include <vtkWindowToImageFilter.h> 
#include <vtkPNGWriter.h>

void writeSTL(vtkSmartPointer<vtkPolyData> polyData, std::string filename) {
    // write a polydata out to stl

    vtkSmartPointer<vtkSTLWriter> stlWriter =
            vtkSmartPointer<vtkSTLWriter>::New();
    stlWriter->SetFileName(filename.c_str());
    stlWriter->SetInput(polyData);
    stlWriter->Write();

}

void visualizeNonManfWithOriginal(vtkSmartPointer<vtkPolyData> nonManf,
        vtkSmartPointer<vtkPolyData> orig) {
    // visualize the non manf volume along with the original
    vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<
            vtkPolyDataMapper>::New();
    mapper1->SetInput(nonManf);
    mapper1->ScalarVisibilityOff();    // utilize actor's property I set

    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<
            vtkPolyDataMapper>::New();
    mapper2->SetInput(orig);
    mapper2->ScalarVisibilityOff();    // utilize actor's property I set

    vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
    actor1->GetProperty()->SetColor(1, 0, 0);
    actor1->SetMapper(mapper1);

    vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
    actor2->GetProperty()->SetColor(1, 1, 1);
    actor2->GetProperty()->SetOpacity(0.5);
    actor2->SetMapper(mapper2);

    vtkSmartPointer<vtkRenderWindow> renWin =
            vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    ren1->SetBackground(0.1, 0.4, 0.2);

    ren1->AddViewProp(actor1);
    ren1->AddViewProp(actor2);
    ren1->ResetCamera();
    renWin->AddRenderer(ren1);
    renWin->SetSize(501, 500); // intentional odd and NPOT  width/height

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<
            vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    renWin->Render(); // make sure we have an OpenGL context.

    // Screenshot
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
            vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renWin);
    windowToImageFilter->SetMagnification(3); //set the resolution of the output image (3 times the current resolution of vtk render window)
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
    windowToImageFilter->Update();

    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName("screenshot2.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();

    iren->Start();
}

void visualizeRenderWindow(vtkSmartPointer<vtkPolyData> polyData) {
    /// do standard vtk visualization using render winows
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<
            vtkPolyDataMapper>::New();
    mapper->SetInput(polyData);
    mapper->ScalarVisibilityOff();    // utilize actor's property I set

    // Visualize
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetColor(1, 1, 1);
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderWindow> renWin =
            vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
    ren1->SetBackground(0.1, 0.4, 0.2);

    ren1->AddViewProp(actor);
    ren1->ResetCamera();
    renWin->AddRenderer(ren1);
    renWin->SetSize(501, 500); // intentional odd and NPOT  width/height

    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<
            vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(renWin);

    renWin->Render(); // make sure we have an OpenGL context.
    iren->Start();

}

vtkSmartPointer<vtkPolyData> extractLevelSetAndSimplify(
        vtkSmartPointer<vtkImageData> imageData, float levelSet,
        float decimation) {
    /// extract level set from vtk image data
    // Create a 3D model using marching cubes
    vtkSmartPointer<vtkMarchingCubes> mc =
            vtkSmartPointer<vtkMarchingCubes>::New();
    mc->SetInput(imageData);
    mc->ComputeNormalsOn();
    mc->ComputeGradientsOn();
    mc->SetValue(0, levelSet);

    vtkSmartPointer<vtkDecimatePro> decimate =
            vtkSmartPointer<vtkDecimatePro>::New();
#if VTK_MAJOR_VERSION <= 5
    decimate->SetInputConnection(mc->GetOutputPort());
#else
    decimate->SetInputData(input);
#endif
    //decimate->SetTargetReduction(.99); //99% reduction (if there was 100 triangles, now there will be 1)
    decimate->SetTargetReduction(decimation);
    decimate->Update();
    return decimate->GetOutput();

}

vtkSmartPointer<vtkImageData> create3dVTKImage(byte* hostVol, int*dims) {

    ///create image data from the host array
    vtkSmartPointer<vtkImageData> imageData =
            vtkSmartPointer<vtkImageData>::New();

    imageData->SetDimensions(dims[0], dims[1], dims[2]);
    cout << "ImageData dimensions = " << dims[0] << "," << dims[1] << ","
            << dims[2] << endl;
#if VTK_MAJOR_VERSION <= 5
    imageData->SetNumberOfScalarComponents(1);
    imageData->SetScalarTypeToDouble();
#else
    imageData->AllocateScalars(VTK_DOUBLE, 1);
#endif
    //populate imageData array
    cout << "Copying to imageData .." << endl;
    for (int k = 0; k < dims[2]; k++) {
        for (int j = 0; j < dims[1]; j++) {
            for (int i = 0; i < dims[0]; i++) {
                double *voxel =
                        static_cast<double*>(imageData->GetScalarPointer(i, j,
                                k));
                voxel[0] = hostVol[j * dims[0] * dims[1] + i * dims[1] + k]
                        * 255.0;
            }
        }
    }

    return imageData;

}
