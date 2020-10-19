#ifndef CGROUP_HPP
#define CGROUP_HPP

#include <string>
#include <sys/types.h>
#include <vector>
namespace HengCore
{
namespace Cgroup
{

    class Cgroup
    {
    private:
        const std::string name;

    public:
        enum SubSystem
        {
            CPUSET  = 0,
            CPUACCT = 1,
            MEMORY  = 2,
            PIDS    = 3
        };
        static const mode_t                   CgroupFsMode;
        static const std::string              CgroupFsBase;
        static const std::vector<std::string> CgroupFsDirs;
        Cgroup(const std::string &name);
        bool      setMemLimit(long long lim);
        long long getMemUsage();
        bool      attach(pid_t pid);
        ~Cgroup();
    };

}  // namespace Cgroup
}  // namespace HengCore
#endif