#undef __STRICT_ANSI__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <windows.h>

double get_us(void)
{
    LARGE_INTEGER freq, now;
    if(!QueryPerformanceFrequency(&freq)) return 0.0;
    if(!QueryPerformanceCounter(&now)) return 0.0;

    return (double)now.QuadPart / (double)freq.QuadPart * 1E+6;
}

#include "stb_vorbis.c"

int main(int argc, const char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Please specify an input file.\n");
        return 1;
    }

    stb_vorbis *stream = stb_vorbis_open_filename(argv[1], NULL, NULL);
    if(!stream) {
        fprintf(stderr, "Failed to open a Vorbis stream.\n");
        return 1;
    }

    FILE *sox = popen("play -t raw -r 44100 -c 2 -e s -b 16 - 2>/dev/null", "w");
    if(!sox) {
        fprintf(stderr, "Failed to connect to sox.\n");
        return 1;
    }

    short pcmout[4096];

    while(1) {
        int result = stb_vorbis_get_samples_short_interleaved(stream, 2, pcmout, 4096);

        if(result > 0) {
            fwrite(pcmout, result * sizeof(short) * 2, 1, sox);
        } else {
            break;
        }
    }

    return 0;
}
