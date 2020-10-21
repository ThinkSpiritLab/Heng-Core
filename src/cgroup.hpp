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
        enum class SubSystem
        {
            CPUSET  = 0,
            CPUACCT = 1,
            MEMORY  = 2,
            PIDS    = 3
        };

        static const std::filesystem::path &
        getSubSystemDir(SubSystem ss);

        void writeTo(SubSystem                    subSystem,
                     const std::filesystem::path &file,
                     std::string                  content);

        void writeTo(SubSystem                    subSystem,
                     const std::filesystem::path &file,
                     long long                    content);

    public:
        static const mode_t                CgroupFsMode;
        static const std::filesystem::path CgroupFsBase;
        static const std::vector<std::filesystem::path>
          CgroupFsDirs;

        Cgroup(const std::string &name);
        bool      setMemLimit(long long lim);
        bool      setPidLimit(long long lim);
        long long getMemUsage();
        bool      attach(pid_t pid);

        std::vector<pid_t>
        getPidInGroup(SubSystem ss = SubSystem::PIDS) const;

        ~Cgroup();
    };

}  // namespace Cgroup
}  // namespace HengCore
#endif