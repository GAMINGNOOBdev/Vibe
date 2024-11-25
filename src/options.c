#include <pspaudio.h>
#include <options.h>
#include <stdio.h>

options_t globalOptions = {1 , PSP_AUDIO_VOLUME_MAX};

void loadOptions()
{
    FILE* in = fopen("options.bin", "rb");
    if (in == NULL)
    {
        saveOptions();
        return;
    }

    fread(&globalOptions, sizeof(options_t), 1, in);

    fclose(in);
}

void saveOptions()
{
    FILE* out = fopen("options.bin", "wb+");
    if (out == NULL)
        return;

    fwrite(&globalOptions, sizeof(options_t), 1, out);
    char pad = 0;
    fwrite(&pad, 1, 1, out);

    fclose(out);
}

options_t* getOptions()
{
    return &globalOptions;
}