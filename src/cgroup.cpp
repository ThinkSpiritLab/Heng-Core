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
    const std::filesystem::path Cgroup::CgroupFsBase =
      "/sys/fs/cgroup/";
    const std::vector<std::filesystem::path>
      Cgroup::CgroupFsDirs = { "cpu/",
                               "cpuacct/",
                               "memory/",
                               "pids/" };

    void Cgroup::writeTo(Cgroup::SubSystem subSystem,
                         const std::filesystem::path &file,
                         std::string content) const
    {
        std::filesystem::path path = CgroupFsBase;
        path /= Cgroup::getSubSystemDir(subSystem);
        path /= CGroupPath;
        path /= file;
        std::error_code ec;
        logger.log("Try write " + content + " to "
                   + path.string());
        if(std::filesystem::is_regular_file(path, ec))
        {
            logger.log("it's regular_file");
            std::ofstream ofs(path);
            ofs << content;
            ofs.close();
            logger.log("done");
        }
        else
        {
            logger.err("it's not regular_file");
            throw std::filesystem::filesystem_error(
              "SubSystem or file not exist",
              path,
              ec);
        }
    }

    void Cgroup::writeTo(Cgroup::SubSystem subSystem,
                         const std::filesystem::path &file,
                         long long content) const
    {
        writeTo(subSystem, file, std::to_string(content));
    }

    template <typename T>
    T Cgroup::readFrom(
      Cgroup::SubSystem            subSystem,
      const std::filesystem::path &file) const
    {
        std::filesystem::path path = Cgroup::CgroupFsBase;
        path /= Cgroup::getSubSystemDir(subSystem);
        path /= CGroupPath;
        path /= file;
        std::error_code ec;
        logger.log("Try read from " + path.string());
        if(std::filesystem::is_regular_file(path, ec))
        {
            T t;
            logger.log("it's regular_file");
            std::ifstream ifs(path);
            ifs >> t;
            ifs.close();
            logger.log("done");
            return t;
        }
        else
        {
            logger.err("it's not regular_file");
            throw std::filesystem::filesystem_error(
              "SubSystem or file not exist",
              path,
              ec);
        }
    }

    const std::filesystem::path &
    Cgroup::getSubSystemDir(Cgroup::SubSystem ss)
    {
        switch(ss)
        {
            case Cgroup::SubSystem::CPU:
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
        logger(name + "/cgroup"),
        masterPid(getpid())
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

    bool Cgroup::attach(pid_t pid) const
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
                ofs.close();
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

    bool Cgroup::setCpuLimit(long long lim) const
    {
        logger.log("setCpuLimit to " + std::to_string(lim));
        try
        {
            // 10ms, cfs_period_us --> when user program
            // forks, minimum usr time limit accuracy, not
            // record accuracy
            static const int cfs_period_us = 10'000;
            writeTo(Cgroup::SubSystem::CPU,
                    "cpu.cfs_period_us",
                    cfs_period_us);
            writeTo(Cgroup::SubSystem::CPU,
                    "cpu.cfs_quota_us",
                    lim * cfs_period_us);
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return false;
        }
        return true;
    }

    bool Cgroup::setMemLimit(long long lim) const
    {
        logger.log("setMemLimit to " + std::to_string(lim));
        try
        {
            writeTo(Cgroup::SubSystem::MEMORY,
                    "memory.limit_in_bytes",
                    lim);
            writeTo(Cgroup::SubSystem::MEMORY,
                    "memory.memsw.limit_in_bytes",
                    lim);
            writeTo(Cgroup::SubSystem::MEMORY,
                    "memory.swappiness",
                    0);
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return false;
        }
        return true;
    }

    bool Cgroup::setPidLimit(long long lim) const
    {
        logger.log("setPidLimit to " + std::to_string(lim));
        try
        {
            writeTo(Cgroup::SubSystem::PIDS,
                    "pids.max",
                    lim);
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return false;
        }
        return true;
    }

    long long Cgroup::getMemUsage() const
    {
        logger.log("getMemUsage");
        try
        {
            return Cgroup::readFrom<long long>(
              Cgroup::SubSystem::MEMORY,
              "memory.memsw.max_usage_in_bytes");
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return __LONG_LONG_MAX__;
        }
    }

    long long Cgroup::getTimeUsr() const
    {
        logger.log("getTimeUsr");
        try
        {
            return Cgroup::readFrom<long long>(
                     Cgroup::SubSystem::CPUACCT,
                     "cpuacct.usage_user")
                   / 1000 / 1000;
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return __LONG_LONG_MAX__;
        }
    }

    long long Cgroup::getTimeSys() const
    {
        logger.log("getTimeSys");
        try
        {
            return Cgroup::readFrom<long long>(
                     Cgroup::SubSystem::CPUACCT,
                     "cpuacct.usage_sys")
                   / 1000 / 1000;
        }
        catch(std::filesystem::filesystem_error &fse)
        {
            logger.err(fse.what());
            return __LONG_LONG_MAX__;
        }
    }

    std::vector<pid_t>
    Cgroup::getPidInGroup(Cgroup::SubSystem ss) const
    {
        std::filesystem::path path = CgroupFsBase;
        path /= Cgroup::getSubSystemDir(ss);
        path /= CGroupPath;
        path /= "cgroup.procs";
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
        if(getpid() == masterPid)
        {
            logger.log("remove cgroup "
                       + CGroupPath.string());
            for(const auto &subDir: CgroupFsDirs)
            {
                std::filesystem::path path = CgroupFsBase;
                path /= subDir;
                path /= CGroupPath;
                logger.log("remove " + path.string());
                try
                {
                    if(std::filesystem::is_directory(path))
                    {
                        std::filesystem::remove(path);
                    }
                }
                catch(
                  std::filesystem::filesystem_error &fse)
                {
                    logger.err(fse.what());
                }
            }
            logger.log("removed");
        }
    }
}  // namespace Cgroup
}  // namespace HengCore