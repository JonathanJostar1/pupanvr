#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>

#include "png.h"

#include "TViewScreenSnap.h"
#include "view_lvgl.h"

bool TViewScreenSnap::screenSnap(const char* pngFileName)
{
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int line_length = 0;
    unsigned int bit_depth = 8;

    unsigned int i = 0;
    unsigned int k = 0;
    unsigned char *rawDataBuffer = NULL;

    FILE *fp = NULL;

    char* framebufferDevAddr = fbdev_get_framebufferMapAddr();
    if(!framebufferDevAddr)
    {
        return false;
    }

    fbdev_get_sizes(&width, &height, &line_length);

    if(width == 0 || height == 0 || line_length == 0)
    {
        return false;
    }

    png_structp png_ptr;
    png_infop info_ptr;

    fp = fopen(pngFileName, "wb");
    if(!fp)
    {
        printf("screenSnap open file %s failure!\n", pngFileName);
        return false;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL)
    {
        printf("png_create_write_struct failure!\n");
        fclose(fp);
        return false;
    }

    rawDataBuffer = (unsigned char*)malloc(line_length);
    if(!rawDataBuffer)
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  NULL);
        return false;
    }

     /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        free(rawDataBuffer);
        png_destroy_write_struct(&png_ptr,  NULL);
        return false;
    }

       /* Set error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* If we get here, we had a problem writing the file */
      fclose(fp);
      free(rawDataBuffer);
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return false;
   }

    //png_set_swap_alpha(png_ptr);

    png_init_io(png_ptr, fp);
  
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, PNG_COLOR_TYPE_RGBA,
       PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    png_set_packing(png_ptr);

    //frame BGRA      ->   RGBA
    png_set_bgr(png_ptr);

    png_write_info(png_ptr, info_ptr);

    unsigned char* pRawRowData = (unsigned char*)framebufferDevAddr;
	for(k = 0; k < height; k++) 
    {
        #if 1
            memcpy(rawDataBuffer, framebufferDevAddr + k * line_length, line_length);
        #else
            pRawRowData = (unsigned char*)(framebufferDevAddr + k * line_length);
            for(i = 0; i < line_length; i = i + 4)
            {  
                //frame BGRA      ->   RGBA
                rawDataBuffer[i + 0] = pRawRowData[i + 2];     //R              
                rawDataBuffer[i + 1] = pRawRowData[i + 1];     //G
                rawDataBuffer[i + 2] = pRawRowData[i + 0];     //B
                rawDataBuffer[i + 3] = pRawRowData[i + 3];     //A
            }
        
        #endif
		png_write_row(png_ptr, rawDataBuffer);
	}

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    free(rawDataBuffer);
    fclose(fp);
    return true;
}