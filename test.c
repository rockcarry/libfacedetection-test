#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "bmpfile.h"
#include "facedetectcnn.h"

static uint32_t get_tick_count(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int main(int argc, char *argv[])
{
    uint8_t  buffer[0x20000] = {};
    int32_t *result = NULL, i;
    char    *bmpfile= (char*)"test.bmp";
    BMP      mybmp  = {};
    uint32_t tick, time;

    if (argc >= 2) bmpfile = argv[1];
    if (0 != bmp_load(&mybmp, bmpfile)) {
        printf("failed to load bmpfile %s !\n", bmpfile);
        goto done;
    }
    printf("mybmp.width : %d\n", mybmp.width );
    printf("mybmp.height: %d\n", mybmp.height);
    printf("mybmp.stride: %d\n", mybmp.stride);

    tick = get_tick_count();
    for (i=0; i<100; i++) {
        result = (int32_t*)facedetect_cnn(buffer, (uint8_t*)mybmp.pdata, mybmp.width, mybmp.height, mybmp.stride);
    }
    time = (int32_t)get_tick_count() - (int32_t)tick;
    printf("nface: %d, time: %d\n", *result, time);

    for (i=0; i<*result; i++) {
        int16_t *p = (int16_t*)(buffer + 4) + 142 * i;
        int16_t  face_s = p[0];
        int16_t  face_x = p[1];
        int16_t  face_y = p[2];
        int16_t  face_w = p[3];
        int16_t  face_h = p[4];
        bmp_rectangle(&mybmp, face_x, face_y, face_x + face_w, face_y + face_h, 0, 255, 0);
        printf("%d, score: %d, x: %d, y: %d, w: %d, h: %d\n", 1, face_s, face_x, face_y, face_w, face_h);
    }
    bmp_save(&mybmp, (char*)"out.bmp");

done:
    bmp_free(&mybmp);
    return 0;
}
