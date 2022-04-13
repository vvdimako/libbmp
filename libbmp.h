#ifndef __LIBBMP_H__
#define __LIBBMP_H__

#define BMP_GET_PADDING(a) ((a) % 4)

typedef enum bmp_error
{
	BMP_FILE_NOT_OPENED = -4,
	BMP_HEADER_NOT_INITIALIZED,
	BMP_INVALID_FILE,
	BMP_ERROR,
	BMP_OK = 0
} bmp_error_e;

typedef struct _bmp_header
{
	unsigned int   bfSize;
	unsigned int   bfReserved;
	unsigned int   bfOffBits;
	
	unsigned int   biSize;
	int            biWidth;
	int            biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int   biCompression;
	unsigned int   biSizeImage;
	int            biXPelsPerMeter;
	int            biYPelsPerMeter;
	unsigned int   biClrUsed;
	unsigned int   biClrImportant;
} bmp_header_t;

typedef struct _bmp_pixel
{
	char blue;
	char green;
	char red;
} bmp_pixel_t;

// This is faster than a function call
#define BMP_PIXEL(r,g,b) ((bmp_pixel){(b),(g),(r)})

typedef struct _bmp_img
{
	bmp_header_t   img_header;
	bmp_pixel_t  **img_pixels;
} bmp_img_t;

// BMP_HEADER
void        bmp_header_init_df(bmp_header_t*, const int, const int);

bmp_error_e bmp_header_write(const bmp_header_t*, FILE*);

bmp_error_e bmp_header_read(bmp_header_t*, FILE*);

// BMP_PIXEL
void        bmp_pixel_init(bmp_pixel_t*,
                           const unsigned char,
                           const unsigned char,
                           const unsigned char);

// BMP_IMG
void        bmp_img_alloc(bmp_img_t*);
void        bmp_img_init_df(bmp_img_t*, const int, const int);
void        bmp_img_free(bmp_img_t*);

bmp_error_e bmp_img_write(const bmp_img_t*, const char*);

bmp_error_e bmp_img_read(bmp_img_t*, const char*);

#endif /* __LIBBMP_H__ */
