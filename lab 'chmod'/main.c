#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
extern char *optarg;
extern int optind, opterr, optopt;
#define _GNU_SOURCE
typedef enum
{
    false = 0,
    true = 1
} bool;

#define MASK_HELP 100000
#define MASK_GROUP 10000
#define MASK_ACTION 1000
#define MASK_PERM1 100
#define MASK_PERM2 10
#define MASK_PERM3 1

void show_man()
{
    printf("chmod [flags] [file]\n");
    printf("Flags must be [h][ugo][ar][rwx][rwx][rwx]:\n -h -- print the reference\n");
    printf("-u -- define attribute group flag 'user'\n");
    printf("-g -- define attribute group flag 'group'\n");
    printf("-o -- define attribute group flag 'other'\n");
    printf("-a -- add permision\n");
    printf("-r -- remove permision\n");
    printf("-r -- permision to read\n");
    printf("-w -- permision to write\n");
    printf("-x -- permision to execute\n");
}

void set_mod(const char *path, unsigned int seq)
{
    if (seq == MASK_HELP)
    {
        show_man();
        return;
    }
    mode_t mode = 0;
    unsigned int target = seq / MASK_GROUP;
    seq %= MASK_GROUP;
    int change = seq / MASK_ACTION;
    seq %= MASK_ACTION;
    unsigned int mask = MASK_PERM1;

    while (seq)
    {
        switch (seq / mask)
        {
        case 1:
            if (target == 1)
            {
                mode = mode | S_IRUSR;
            }
            else if (target == 2)
            {
                mode = mode | S_IRGRP;
            }
            else if (target == 3)
            {
                mode = mode | S_IROTH;
            }
            break;
        case 2:
            if (target == 1)
            {
                mode = mode | S_IWUSR;
            }
            else if (target == 2)
            {
                mode = mode | S_IWGRP;
            }
            else if (target == 3)
            {
                mode = mode | S_IWOTH;
            }
            break;
        case 3:
            if (target == 1)
            {
                mode = mode | S_IXUSR;
            }
            else if (target == 2)
            {
                mode = mode | S_IXGRP;
            }
            else if (target == 3)
            {
                mode = mode | S_IXOTH;
            }
            break;
        }
        seq = seq % mask;
        mask /= 10;
    }

    struct stat sb;
    if (stat(path, &sb) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    if (change == 1)
    {
        int r = chmod(path, (mode) | sb.st_mode);
        if (r < 0)
        {
            perror("Can't change rights for the file");
            exit(EXIT_FAILURE);
        }
    }
    else if (change == 2)
    {
        int r = chmod(path, sb.st_mode ^ mode);
        if (r < 0)
        {
            perror("Can't change rights for the file");
            exit(EXIT_FAILURE);
        }
    }
}

bool checker(unsigned int seq)
{
    if ((seq / MASK_HELP == 1) && (seq % MASK_HELP == 0))
    {
        return true;
    }
    else if (seq > MASK_HELP)
    {
        return false;
    }
    else if (((seq % MASK_HELP) / MASK_GROUP == 0) ||
             ((seq % MASK_HELP) / MASK_GROUP > 3))
    {
        return false;
    }
    else if ((((seq % MASK_HELP) % MASK_GROUP) / MASK_ACTION == 0) ||
             (((seq % MASK_HELP) % MASK_GROUP) / MASK_ACTION > 2))
    {
        return false;
    }
    unsigned int permisions = ((seq % MASK_HELP) % MASK_GROUP) % MASK_ACTION;
    if (permisions == 0)
    {
        return false;
    }
    unsigned int tmp = 0;
    bool isEmpty = true;
    while (permisions)
    {
        tmp = permisions % 10;
        if (tmp != 0)
        {
            isEmpty = false;
        }
        if (tmp > 3)
        {
            return false;
        }
        permisions /= 10;
    }
    if (isEmpty)
    {
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    int c = 0;
    unsigned int flags = 0;
    bool shutDown = false;
    unsigned int step = MASK_PERM1;
    //Закодируем флаги в числовой формат
    while (((c = getopt(argc, argv, "hugoadrwx")) != -1) && !shutDown)
    {
        switch (c)
        {
        case 'h':
            flags += (MASK_HELP);
            break;
        case 'u':
            flags += (1 * MASK_GROUP);
            break;
        case 'g':
            flags += (2 * MASK_GROUP);
            break;
        case 'o':
            flags += (3 * MASK_GROUP);
            break;
        case 'a':
            flags += (1 * MASK_ACTION);
            break;
        case 'r':
            if (((flags % MASK_HELP) % MASK_GROUP) / MASK_ACTION == 0)
            {
                flags += (2 * MASK_ACTION);
            }
            else
            {
                flags += (1 * step);
                step /= 10;
            }
            break;
        case 'w':
            flags += (2 * step);
            step /= 10;
            break;
        case 'x':
            flags += (3 * step);
            step /= 10;
            break;
        default:
            printf("Incorrect flag\n");
            shutDown = true;
            break;
        }
    }
    bool isConsistant = checker(flags);
    if (isConsistant && (argc > 2))
    {
        set_mod(argv[2], flags);
    }
    else
    {
        set_mod(NULL, flags);
    }
    return 0;
}