#include "vtkEMReader.h"

#include "vtkDataObject.h"
#include "vtkObjectFactory.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <sys/stat.h>
#include <assert.h>

//*****************************************************************************
vtkStandardNewMacro(vtkEMReader);
//----------------------------------------------------------------------------
vtkEMReader::vtkEMReader()
{
  this->FileName = NULL;
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkEMReader::~vtkEMReader()
{
	this->SetFileName(NULL);
}

void vtkEMReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "FileName: "
     << (this->FileName? this->FileName:"(none)") << "\n";
}

int vtkEMReader::ReadFileHeader(const char* filename, tom_io_em_header* header, FILE** fhandle)
{
	if(filename == NULL)
		return 0;
	FILE* f;
	if(!(f = fopen(filename, "rb")))
		return 0;
	
	assert(sizeof(tom_io_em_header) == 512);
	
	if (fread(header, sizeof(tom_io_em_header), 1, f) != 1)
	{
        fclose(f);
        return 0;
    }
    
    // get file size
    struct stat st;
    stat(filename, &st);
    int64_t fsize = st.st_size;
    
    int64_t element_size = 4; // float
    if (fsize-(int64_t)sizeof(tom_io_em_header) != (int64_t)element_size* (int64_t)header->dims[0] * header->dims[1]*header->dims[2])
    {
        fclose(f);
        return 0;
    }
    
    if (fhandle)
    {
//    	fseek(f, sizeof(tom_io_em_header)-1, SEEK_SET);
        *fhandle = f;
    }
    else
        fclose(f);
    
    return 1;
}

int vtkEMReader::RequestInformation (
   vtkInformation*,
   vtkInformationVector**,
   vtkInformationVector* outputVector)
{
   vtkInformation* outInfo = outputVector->GetInformationObject(0);

   // read file to find available extent
   tom_io_em_header header;
   if (this->ReadFileHeader(this->GetFileName(), &header, NULL) != 1)
   {
     vtkErrorMacro("Wrong file format!");
     return 0;
   }

   int extent[6] = {0, header.dims[0]-1, 0, header.dims[1]-1, 0, header.dims[2]-1};
   double spacing[3] = {1, 1, 1};
   double origin[3] = {0, 0, 0};

   outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);
   outInfo->Set(vtkDataObject::SPACING(), spacing, 3);
   outInfo->Set(vtkDataObject::ORIGIN(), origin, 3);
   vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, 1);
   return 1;
}


int vtkEMReader::RequestData(
   vtkInformation*,
   vtkInformationVector**,
   vtkInformationVector* outputVector)
{
   vtkImageData* image = vtkImageData::GetData(outputVector);
   
   // read file to find available extent
   FILE *f;
   tom_io_em_header header;
   if (this->ReadFileHeader(this->GetFileName(), &header, &f) != 1)
   {
     vtkErrorMacro("Wrong file format!");
     return 0;
   }
   
   size_t count = header.dims[0]*header.dims[1]*header.dims[2];
   float* p = new float[count];
   if (p == 0)
   {
     vtkErrorMacro("Fail to allocate memory!");
     fclose(f);
     return 0;
   }
   if (fread(p, sizeof(float), count, f) != count)
   {
     vtkErrorMacro("Fail to read data!");
     delete[] p;
     fclose(f);
     return 0;
   }
   
   vtkFloatArray* data = vtkFloatArray::New();
   data->SetArray(p, count, 0);
   
   image->SetDimensions(header.dims[0], header.dims[1], header.dims[2]);
   image->GetPointData()->SetScalars(data);
//   image->AllocateScalars();
   vtkDataArray* scalars = image->GetPointData()->GetScalars();
   scalars->SetName("Density"); 
   
   fclose(f);
   return 1;
}