#include <errno.h>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "cgroup.hpp"
#include "logger.hpp"
namespace HengCore
{
namespace Cgroup
{
    const mode_t                Cgroup::CgroupFsMode = 0755;
    const std::filesystem::path Cgroup::CgroupFsBase =
      "/sys/fs/cgroup/";
    const std::vector<std::filesystem::path>
      Cgroup::CgroupFsDirs = { "cpuset/",
                               "cpuacct/",
                               "memory/",
                               "pids/" };

    Cgroup::Cgroup(const std::string &name):
        CGroupPath(name),
        logger(name + "/cgroup")
    {
        logger.log("created cgroup " + CGroupPath.string());
        for(const auto &subDir: CgroupFsDirs)
        {
            std::filesystem::path path = CgroupFsBase;
            path /= subDir;
            path /= CGroupPath;
            logger.log("mkdir " + path.string());
            try
            {
                std::filesystem::create_directories(path);
            }
            catch(std::filesystem::filesystem_error &fse)
            {
                logger.err(fse.what());
            }
        }
        logger.log("Done");
    }

    Cgroup::~Cgroup()
    {
        logger.log("remove cgroup " + CGroupPath.string());
        for(const auto &subDir: CgroupFsDirs)
        {
            std::filesystem::path path = CgroupFsBase;
            path /= subDir;
            path /= CGroupPath;
            logger.log("remove " + path.string());
            try
            {
                std::filesystem::remove_all(path);
            }
            catch(std::filesystem::filesystem_error &fse)
            {
                logger.err(fse.what());
            }
        }
        logger.log("removed");
    }
}  // namespace Cgroup
}  // namespace HengCore