#include <pspaudio.h>
#include <options.h>
#include <stdio.h>

options_t options = {1, 1, 1};

void options_load(void)
{
    FILE* in = fopen("options.bin", "rb");
    if (in == NULL)
    {
        options_save();
        return;
    }

    fread(&options, sizeof(options_t), 1, in);

    fclose(in);
}

void options_save(void)
{
    FILE* out = fopen("options.bin", "wb+");
    if (out == NULL)
        return;

    fwrite(&options, sizeof(options_t), 1, out);
    char pad = 0;
    fwrite(&pad, 1, 1, out);

    fclose(out);
}
