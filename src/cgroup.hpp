#ifndef CGROUP_HPP
#define CGROUP_HPP

#include <filesystem>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "logger.hpp"

namespace HengCore
{
namespace Cgroup
{

    class Cgroup
    {
    private:
        const std::filesystem::path CGroupPath;
        Logger::Logger              logger;
        const pid_t                 masterPid;
        enum class SubSystem
        {
            CPU     = 0,
            CPUACCT = 1,
            MEMORY  = 2,
            PIDS    = 3
        };

        static const std::filesystem::path &
        getSubSystemDir(SubSystem ss);

        void writeTo(SubSystem                    subSystem,
                     const std::filesystem::path &file,
                     std::string content) const;

        void writeTo(SubSystem                    subSystem,
                     const std::filesystem::path &file,
                     long long content) const;

        template <typename T>
        T readFrom(SubSystem                    subSystem,
                   const std::filesystem::path &file) const;

    public:
        static const std::filesystem::path CgroupFsBase;
        static const std::vector<std::filesystem::path>
          CgroupFsDirs;

        Cgroup(const std::string &name);
        bool      setCpuLimit(long long lim) const;
        bool      setMemLimit(long long lim) const;
        bool      setPidLimit(long long lim) const;
        long long getMemUsage() const;
        long long getTimeUsr() const;
        long long getTimeSys() const;
        bool      attach(pid_t pid) const;

        std::vector<pid_t>
        getPidInGroup(SubSystem ss = SubSystem::PIDS) const;

        ~Cgroup();
    };

}  // namespace Cgroup
}  // namespace HengCore
#endif