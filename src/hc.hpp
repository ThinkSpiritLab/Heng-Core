#include <sstream>
#include <string>

namespace HengCore
{
struct Config
{
    long long   timeLimit   = -1;
    long long   memLimit    = -1;
    long long   outPutLimit = -1;
    long long   uid         = 1;
    long long   gid         = 1;
    long long   maxPid      = -1;
    std::string stdout      = "/dev/null";
    std::string stderr      = "/dev/null";
    std::string args        = "";
    std::string bin         = "";

    std::string toJson()
    {
        std::stringstream buffer;
#define outPut(x) \
    buffer << "\"" << #x << "\":\"" << x << "\"";
        outPut(timeLimit);
        outPut(memLimit);
        outPut(outPutLimit);
        outPut(uid);
        outPut(maxPid);
        outPut(stdout);
        outPut(stderr);
        outPut(args);
        outPut(bin);
#undef outPut
        return buffer.str();
    }
};
}  // namespace HengCore