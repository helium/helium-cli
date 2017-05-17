#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


cli_func
cli_find(const char * needle, struct cli_command * commands)
{
    for (struct cli_command * cmd = commands; cmd->name; cmd++)
    {
        if (strcmp(cmd->name, needle) == 0)
        {
            return cmd->func;
        }
    }
    return NULL;
}


const char *
str_carbon_status(enum carbon_status status)
{
    switch (status)
    {
    case carbon_status_OK:
        return NULL;
    case carbon_status_ERR_COMMUNICATION:
        return "Error communication with Atom";
    case carbon_status_ERR_NOT_CONNECTED:
        return "Atom not connected to the network";
    case carbon_status_ERR_TIMEOUT:
        return "Timeout while trying to connect to network";
    case carbon_status_ERR_DROPPED:
        return "Atom dropped packet";
    case carbon_status_ERR_KEEP_AWAKE:
        return "Atom needs to stay awake";
    case carbon_status_ERR_FAILED:
        return "Network request failed";
    case carbon_status_ERR_NOT_FOUND:
        return "Network resource not found";
    }
}

int
load_file(const char * filename, uint8_t * data, size_t len)
{
    int result;
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        ERR_EXIT(-1, "Error opening file: %s\n", strerror(errno));
    }

    struct stat stat;
    if (fstat(fd, &stat) == -1)
    {
        ERR_EXIT(-1, "Error getting file stats: %s\n", strerror(errno));
    }

    ssize_t file_size = (ssize_t)stat.st_size;
    if (file_size > (ssize_t)len)
    {
        ERR_EXIT(-1,
                 "File size %zu is too big to be sent (max: %zu)\n",
                 file_size,
                 len);
    }

    if (read(fd, data, file_size) != file_size)
    {
        ERR_EXIT(-1, "Error reading data: %s\n", strerror(errno));
    }

    result = (int)file_size;

exit:
    if (fd >= 0)
    {
        close(fd);
    }
    return result;
}

int
save_file(const char * filename, uint8_t * data, size_t len)
{
    int fd     = open(filename, O_CREAT | O_WRONLY);
    int result = 0;

    if (fd == -1)
    {
        ERR_EXIT(-1, "Error opening output file: %s\n", strerror(errno));
    }

    if (write(fd, data, len) != (ssize_t)len)
    {
        ERR_EXIT(-1, "Error writing connection data: %s\n", strerror(errno));
    }

exit:
    if (fd >= 0)
    {
        close(fd);
    }
    return result;
}