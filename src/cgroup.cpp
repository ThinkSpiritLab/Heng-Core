#include "cgroup.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
namespace HengCore
{
namespace Cgroup
{
    const mode_t      Cgroup::CgroupFsMode = 0755;
    const std::string Cgroup::CgroupFsBase =
      "/sys/fs/cgroup/";
    const std::vector<std::string> Cgroup::CgroupFsDirs = {
        "cpuset/",
        "cpuacct/",
        "memory/",
        "pids/"
    };

    Cgroup::Cgroup(const std::string &name): name(name)
    {
        for(const auto &subDir: CgroupFsDirs)
        {
            mkdir((CgroupFsBase + subDir).c_str(),
                  CgroupFsMode);
        }
    }

    Cgroup::~Cgroup()
    {
        for(const auto &subDir: CgroupFsDirs)
        {
            rmdir((CgroupFsBase + subDir).c_str());
        }
    }
}  // namespace Cgroup
}  // namespace HengCore