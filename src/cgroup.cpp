#include <errno.h>
#include <filesystem>
#include <fstream>
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

    const std::filesystem::path &
    Cgroup::getSubSystemDir(Cgroup::SubSystem ss)
    {
        switch(ss)
        {
            case Cgroup::SubSystem::CPUSET:
                return Cgroup::CgroupFsDirs[0];
                break;
            case Cgroup::SubSystem::CPUACCT:
                return Cgroup::CgroupFsDirs[1];
                break;
            case Cgroup::SubSystem::MEMORY:
                return Cgroup::CgroupFsDirs[2];
                break;
            case Cgroup::SubSystem::PIDS:
                return Cgroup::CgroupFsDirs[3];
                break;
            default:
                throw std::invalid_argument(
                  "UnKnow SubSystem");
                break;
        }
    }

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

    bool Cgroup::attach(pid_t pid)
    {
        logger.log("attach pid" + std::to_string(pid));
        for(const auto &subDir: CgroupFsDirs)
        {
            std::filesystem::path path = CgroupFsBase;
            path /= subDir;
            path /= CGroupPath;
            path /= "tasks";
            logger.log("open " + path.string());
            try
            {
                std::ofstream ofs(path,
                                  std::ios_base::out
                                    | std::ios_base::ate);
                ofs << pid;
            }
            catch(std::filesystem::filesystem_error &fse)
            {
                logger.err(fse.what());
                return false;
            }
        }
        logger.log("attached");
        return true;
    }

    bool Cgroup::setMemLimit(long long lim)
    {
        logger.log("setMemLimit to " + std::to_string(lim));
        std::filesystem::path path = CgroupFsBase;
        path /= "memory";
        path /= CGroupPath;
        path /= "memory.limit_in_bytes";
        try
        {
            std::ofstream ofs(path);
            ofs << lim * 1024 * 1024;
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return false;
        }
        return true;
    }

    long long Cgroup::getMemUsage()
    {
        logger.log("getMemUsage");
        std::filesystem::path path = CgroupFsBase;
        path /= "memory";
        path /= CGroupPath;
        path /= "memory.max_usage_in_bytes";
        try
        {
            long long     m;
            std::ifstream ifs(path);
            ifs >> m;
            return m / 1024 / 1024;
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            // return false;
            return -1;
        }
    }
    std::vector<pid_t>
    Cgroup::getPidInGroup(Cgroup::SubSystem ss) const
    {
        std::filesystem::path path = CgroupFsBase;
        path /= Cgroup::getSubSystemDir(ss);
        path /= CGroupPath;
        path /= "tasks";
        std::error_code ec;
        if(std::filesystem::is_regular_file(path, ec))
        {
            std::ifstream      tasksifs(path);
            std::vector<pid_t> vec;
            pid_t              pid;
            while(tasksifs >> pid)
            {
                vec.push_back(pid);
            }
            tasksifs.close();
            return vec;
        }
        else
        {
            throw std::filesystem::filesystem_error(
              "SubSystem not exist",
              path,
              ec);
        }
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