#ifndef _VTK_EM_READER_H_
#define _VTK_EM_READER_H_

// ---------------------------------------
// Define the EM file header
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

/* These are integer numbers defining the raw datatype.
 * The numerical value corresponds to the parameter in the em-header.
 * This is arbitrary. Don't rely on the numerical values of the defines.
 * Only make them >= 1. See also iotype_datasize */
#define TOM_IO_TYPE_UNKNOWN                    ((int)0)
#define TOM_IO_TYPE_INT8                       ((int)1)
#define TOM_IO_TYPE_INT16                      ((int)2)
#define TOM_IO_TYPE_INT32                      ((int)4)
#define TOM_IO_TYPE_FLOAT                      ((int)5)
#define TOM_IO_TYPE_COMPLEX32                  ((int)8)
#define TOM_IO_TYPE_DOUBLE                     ((int)9)
#define TOM_IO_TYPE_COMPLEX64                  ((int)10)
#define TOM_IO_TYPE_INT64                      ((int)11)

/****************************************************************************//**
 * \brief Structure for the em-header.
 *
 * A binary compatible structure to the header of the em-file.
 * It contains the exactly same bits as stored in the file, exept the case
 * when the integers are swaped.
 *******************************************************************************/
typedef struct tom_io_em_header {
    int8_t machine;             /**< Byte 1: Machine Coding
                                            (OS-9;      0),
                                            (VAX;       1),
                                            (Convex;    2),
                                            (SGI;       3),
                                            (Mac;       5),
                                            (PC;        6). */
    int8_t byte2;               /**< General purpose. On OS-9 system: 0 old version 1 is new version. */
    int8_t byte3;               /**< Not used in standard EM-format, if this byte is 1 the header is abandoned. */
    int8_t type;                /**< Data Type Coding. */
    uint32_t dims[3];           /**< Three long integers (3x4 bytes) are image size in x, y, z Dimension. */
    int8_t comment[80];         /**< 80 Characters as comment. */
    int32_t emdata[40];         /**< 40 long integers (4 x 40 bytes) are user defined parameters. */
    int8_t  userdata[256];      /**< 256 Byte with userdata, i.e. the username. */
} tom_io_em_header;

// ---------------------------------------
#include "vtkImageAlgorithm.h"

class vtkInformationVector;
class vtkInformation;

class VTK_IO_EXPORT vtkEMReader : public vtkImageAlgorithm
{
public:
    static vtkEMReader* New();
    vtkTypeMacro(vtkEMReader, vtkImageAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);
    
//    virtual int CanReadFile(const char* name);

protected:
   vtkEMReader();
   ~vtkEMReader();
   
   int ReadFileHeader(const char* filename, tom_io_em_header* header, FILE** fhandle);

   int RequestInformation(
		vtkInformation*,
		vtkInformationVector**,
		vtkInformationVector*);

   int RequestData(
		vtkInformation*,
		vtkInformationVector**,
		vtkInformationVector*);
	
   char* FileName;

private:
   vtkEMReader(const vtkEMReader&);
   void operator=(const vtkEMReader&);
};

#endif