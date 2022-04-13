/* Copyright 2016 - 2017 Marc Volker Dickmann
 * Project: LibBMP
 */
#include <stdio.h>
#include <stdlib.h>
#include "libbmp.h"

#define BMP_MAGIC 19778


/*
 * BMP_HEADER
 */

void
bmp_header_init_df(bmp_header_t *header, const int width, const int height)
{
	header->bfSize = (sizeof (bmp_pixel_t) * width + BMP_GET_PADDING(width))
	                  * abs(height);
	header->bfReserved = 0;
	header->bfOffBits = 54;
	header->biSize = 40;
	header->biWidth = width;
	header->biHeight = height;
	header->biPlanes = 1;
	header->biBitCount = 24;
	header->biCompression = 0;
	header->biSizeImage = 0;
	header->biXPelsPerMeter = 0;
	header->biYPelsPerMeter = 0;
	header->biClrUsed = 0;
	header->biClrImportant = 0;
}


bmp_error_e
bmp_header_write(const bmp_header_t *header, FILE *img_file)
{
	unsigned short magic = BMP_MAGIC;
	
	if (header == NULL)
		return BMP_HEADER_NOT_INITIALIZED; 
	else 
		if (img_file == NULL)
			return BMP_FILE_NOT_OPENED;
	
	fwrite(&magic, sizeof(magic), 1, img_file);
	fwrite(header, sizeof(bmp_header_t), 1, img_file);
	return BMP_OK;
}


bmp_error_e
bmp_header_read(bmp_header_t *header, FILE *img_file)
{
	unsigned short magic;
	
	if (img_file == NULL)
		return BMP_FILE_NOT_OPENED;
	
	// Check if its an bmp file by comparing the magic nbr:
	if (fread(&magic, sizeof (magic), 1, img_file) != 1 || magic != BMP_MAGIC)
		return BMP_INVALID_FILE;
	if (fread(header, sizeof(bmp_header_t), 1, img_file) != 1)
		return BMP_ERROR;
	return BMP_OK;
}


/*
 * BMP_PIXEL
 */

void
bmp_pixel_init(bmp_pixel_t *pxl,
               unsigned char red, unsigned char green, unsigned char blue)
{
	pxl->red = red;
	pxl->green = green;
	pxl->blue = blue;
}


/*
 * BMP_IMG
 */


// VVD TODO: allocate linear space and fix pointers to it
void
bmp_img_alloc(bmp_img_t *img)
{
	size_t y, h = abs(img->img_header.biHeight);
	
	img->img_pixels = malloc(sizeof(bmp_pixel_t*) * h);
	for (y = 0; y < h; y++)
		img->img_pixels[y] = malloc(sizeof(bmp_pixel_t) * img->img_header.biWidth);
}


void
bmp_img_init_df(bmp_img_t *img, const int  width, const int  height)
{
	bmp_header_init_df(&img->img_header, width, height);
	bmp_img_alloc(img);
}


void
bmp_img_free(bmp_img_t *img)
{
	size_t y, h = abs(img->img_header.biHeight);
	
	for (y = 0; y < h; y++)
		free(img->img_pixels[y]);
	free(img->img_pixels);
}


bmp_error_e
bmp_img_write(const bmp_img_t *img, const char *filename)
{
	FILE *img_file = fopen(filename, "wb");
	bmp_error_e err;
	size_t y, h, offset;
	const unsigned char padding[3] = { 0 };
	
	if (img_file == NULL)
		return BMP_FILE_NOT_OPENED;
	
	err = bmp_header_write(&img->img_header, img_file);
	if (err != BMP_OK)
	{
		fclose(img_file);
		return err;
	}
	
	// Select the mode (bottom-up or top-down):
	h = abs(img->img_header.biHeight);
	offset = (img->img_header.biHeight > 0 ? h - 1 : 0);
	
	// Write the content:
	for (y = 0; y < h; y++)
	{
		// Write a whole row of pixels to the file:
		fwrite(img->img_pixels[abs(offset - y)], sizeof(bmp_pixel_t), 
		       img->img_header.biWidth, img_file);
		// Write the padding for the row
		fwrite(padding, sizeof(unsigned char), 
		       BMP_GET_PADDING(img->img_header.biWidth), img_file);
	}
	
	fclose(img_file);
	return BMP_OK;
}


bmp_error_e
bmp_img_read(bmp_img_t *img, const char *filename)
{
	FILE *img_file = fopen(filename, "rb");
	bmp_error_e err;
	size_t y, h, offset, padding;
	
	if (img_file == NULL)
		return BMP_FILE_NOT_OPENED;
	
	err = bmp_header_read(&img->img_header, img_file);
	if (err != BMP_OK)
	{
		fclose(img_file);
		return err;
	}
	
	bmp_img_alloc(img);
	
	// Select the mode (bottom-up or top-down):
	h = abs(img->img_header.biHeight);
	offset = (img->img_header.biHeight > 0 ? h - 1 : 0);
	padding = BMP_GET_PADDING(img->img_header.biWidth);
	
	// Read the content:
	for (y = 0; y < h; y++)
	{
		// Read a whole row of pixels from the file:
		if (fread(img->img_pixels[abs(offset - y)], sizeof (bmp_pixel_t), 
		          img->img_header.biWidth, img_file) != img->img_header.biWidth)
		{
			fclose (img_file);
			return BMP_ERROR;
		}
		
		// Skip the padding:
		fseek(img_file, padding, SEEK_CUR);
	}
	
	fclose(img_file);
	return BMP_OK;
}
