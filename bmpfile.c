#include <stdlib.h>
#include <stdio.h>
#include "bmpfile.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

/* 内部函数实现 */
static int ALIGN(int x, int y) {
    // y must be a power of 2.
    return (x + y - 1) & ~(y - 1);
}

//++ for bmp file ++//
// 内部类型定义
#pragma pack(1)
typedef struct {
    uint16_t  bfType;
    uint32_t  bfSize;
    uint16_t  bfReserved1;
    uint16_t  bfReserved2;
    uint32_t  bfOffBits;
    uint32_t  biSize;
    uint32_t  biWidth;
    uint32_t  biHeight;
    uint16_t  biPlanes;
    uint16_t  biBitCount;
    uint32_t  biCompression;
    uint32_t  biSizeImage;
    uint32_t  biXPelsPerMeter;
    uint32_t  biYPelsPerMeter;
    uint32_t  biClrUsed;
    uint32_t  biClrImportant;
} BMPFILEHEADER;
#pragma pack()

/* 函数实现 */
int bmp_load(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    uint8_t      *pdata  = NULL;
    int           i;

    fp = fopen(file, "rb");
    if (!fp) return -1;

    fread(&header, sizeof(header), 1, fp);
    pb->width  = header.biWidth;
    pb->height = header.biHeight;
    pb->stride = ALIGN(header.biWidth * 3, 4);
    pb->cdepth = 24;
    pb->pdata  = malloc(pb->stride * pb->height);
    if (pb->pdata) {
        pdata  = (uint8_t*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fread(pdata, pb->stride, 1, fp);
        }
    }

    fclose(fp);
    return pb->pdata ? 0 : -1;
}

int bmp_create(BMP *pb)
{
    pb->stride = ALIGN(pb->width * (pb->cdepth / 8), 4);
    pb->pdata  = calloc(1, pb->width * pb->stride);
    return pb->pdata ? 0 : -1;
}

int bmp_save(BMP *pb, char *file)
{
    BMPFILEHEADER header = {0};
    FILE         *fp     = NULL;
    uint8_t      *pdata;
    int           i;

    header.bfType     = ('B' << 0) | ('M' << 8);
    header.bfSize     = sizeof(header) + pb->stride * pb->height;
    header.bfOffBits  = sizeof(header);
    header.biSize     = 40;
    header.biWidth    = pb->width;
    header.biHeight   = pb->height;
    header.biPlanes   = 1;
    header.biBitCount = pb->cdepth;
    header.biSizeImage= pb->stride * pb->height;

    fp = fopen(file, "wb");
    if (fp) {
        fwrite(&header, sizeof(header), 1, fp);
        pdata = (uint8_t*)pb->pdata + pb->stride * pb->height;
        for (i=0; i<pb->height; i++) {
            pdata -= pb->stride;
            fwrite(pdata, pb->stride, 1, fp);
        }
        fclose(fp);
    }

    return fp ? 0 : -1;
}

void bmp_free(BMP *pb)
{
    if (pb->pdata) {
        free(pb->pdata);
        pb->pdata = NULL;
    }
    pb->width  = 0;
    pb->height = 0;
    pb->stride = 0;
    pb->cdepth = 0;
}

void bmp_setpixel(BMP *pb, int x, int y, int r, int g, int b)
{
    uint8_t *pbyte = (uint8_t*)pb->pdata;
    if (x >= pb->width || y >= pb->height) return;
    r = r < 0 ? 0 : r < 255 ? r : 255;
    g = g < 0 ? 0 : g < 255 ? g : 255;
    b = b < 0 ? 0 : b < 255 ? b : 255;
    pbyte[x * (pb->cdepth / 8) + 0 + y * pb->stride] = b;
    pbyte[x * (pb->cdepth / 8) + 1 + y * pb->stride] = g;
    pbyte[x * (pb->cdepth / 8) + 2 + y * pb->stride] = r;
}

void bmp_getpixel(BMP *pb, int x, int y, int *r, int *g, int *b)
{
    uint8_t *pbyte = (uint8_t*)pb->pdata;
    if (x >= pb->width || y >= pb->height) {
        *r = *g = *b = 0;
        return;
    }
    *r = pbyte[x * (pb->cdepth / 8) + 0 + y * pb->stride];
    *g = pbyte[x * (pb->cdepth / 8) + 1 + y * pb->stride];
    *b = pbyte[x * (pb->cdepth / 8) + 2 + y * pb->stride];
}

void bmp_rectangle(BMP *pb, int x1, int y1, int x2, int y2, int r, int g, int b)
{
    int i;
    for (i=x1; i<=x2; i++) {
        bmp_setpixel(pb, i, y1, r, g, b);
        bmp_setpixel(pb, i, y2, r, g, b);
    }
    for (i=y1; i<=y2; i++) {
        bmp_setpixel(pb, x1, i, r, g, b);
        bmp_setpixel(pb, x2, i, r, g, b);
    }
}

#if 0
int main(int argc, char *argv[])
{
    BMP bmp24 = {};
    BMP bmp32 = {};
    int x, y, r, g, b;
    char file[256];

    if (argc < 2) {
        printf("\n");
        printf("tool for convert 24bit bitmap to 32bit\n\n");
        printf("usage:\n");
        printf("  bmp24to32 bmpfile \n\n");
        return 0;
    }

    bmp_load(&bmp24, argv[1]);
    bmp32.width  = bmp24.width;
    bmp32.height = bmp24.height;
    bmp32.cdepth = 32;
    bmp_create(&bmp32);

    for (y=0; y<bmp24.height; y++) {
        for (x=0; x<bmp24.width; x++) {
            bmp_getpixel(&bmp24, x, y, &r, &g, &b);
            bmp_setpixel(&bmp32, x, y,  r,  g,  b);
        }
    }

    snprintf(file, sizeof(file), "%s.32", argv[1]);
    bmp_save(&bmp32, file);

    bmp_free(&bmp24);
    bmp_free(&bmp32);
}
#endif

#if 0
int main(void)
{
	BMP bmp = {0};
	int x, y;
	bmp.width  = 640;
	bmp.height = 480;
	bmp.cdepth = 24;
	bmp_create(&bmp);
	for (y=0; y<bmp.height; y++) {
		for (x=0; x<bmp.width; x++) {
			bmp_setpixel(&bmp, x, y, 0, 0, 255);
		}
	}
	bmp_save  (&bmp, "123.bmp");
	bmp_free  (&bmp);
}
#endif


