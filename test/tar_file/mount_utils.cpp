#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/vfs.h>

#include <linux/magic.h>

#include "sample/mount_utils.h"

int MountUtils::MountTmpFs(const char* target, int size_megabyte)
{
    char param[1024];
    umount(target);
    snprintf(param, sizeof(param), "size=%dM", size_megabyte);
    return mount("tmpfs", target, "tmpfs", 0, param);
}

int MountUtils::UmoutTmpfs(const char* target)
{
    return umount(target);
}

int MountUtils::IsTmpFs(const char* target)
{
    struct statfs sfs;
    if (0 != statfs(target, &sfs)) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    return sfs.f_type == TMPFS_MAGIC;
}
