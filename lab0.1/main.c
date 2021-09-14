#include <stdio.h>
#include <dirent.h>
#include <string.h>
int listdir(const char *path)
{
    struct dirent *entry;
    DIR *dp;

    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)))
    {
        if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0))
        {
            puts(entry->d_name);
        }
    }
    closedir(dp);
    return 0;
}

int main(int argc, char **argv)
{
    int counter = 1;

    if (argc == 1)
        listdir(".");

    while (++counter <= argc)
    {
        // printf("\nListing %s...\n", argv[counter - 1]);
        listdir(argv[counter - 1]);
    }

    return 0;
}