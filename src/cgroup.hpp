#ifndef CGROUP_HPP
#define CGROUP_HPP

#include <filesystem>
#include <string>
#include <sys/types.h>
#include <vector>

#include "logger.hpp"

namespace HengCore
{
namespace Cgroup
{

    class Cgroup
    {
    private:
        // const std::string     name;
        std::filesystem::path CGroupPath;
        Logger::Logger        logger;

    public:
        enum SubSystem
        {
            CPUSET  = 0,
            CPUACCT = 1,
            MEMORY  = 2,
            PIDS    = 3
        };
        static const mode_t                CgroupFsMode;
        static const std::filesystem::path CgroupFsBase;
        static const std::vector<std::filesystem::path>
          CgroupFsDirs;
        Cgroup(const std::string &name);
        bool      setMemLimit(long long lim);
        long long getMemUsage();
        bool      attach(pid_t pid);
        ~Cgroup();
    };

}  // namespace Cgroup
}  // namespace HengCore
#endif