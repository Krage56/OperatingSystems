#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>
extern char *optarg;
extern int optind, opterr, optopt;
#define _GNU_SOURCE

const size_t path_max_size = 256;

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
    int c = 0;
    while ((c = getopt(argc, argv, "ab:c")) != -1)
    {
        switch (c)
        {
        case 'a':
            printf("%s\n", "a-option entered");
        case 'b':
            if (optarg)
            {
                printf("%s\n", optarg);
            }
            else
            {
                printf("%s\n", "b-option entered");
            }
        }
    }
    // int counter = 1;

    // if (argc == 1)
    //     listdir(".");
    // else if (argc > 1)
    // {
    // }
    // while (++counter <= argc)
    // {
    //     listdir(argv[counter - 1]);
    // }

    return 0;
}