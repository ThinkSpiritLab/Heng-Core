#include "config.hpp"
#include "hc.hpp"

#include "util/argvPraser.hpp"

#include <iostream>

#include "json.hpp"
namespace HengCore
{
using Cfg = Config::Config;
int main(int argc, char *argv[])
{

    auto praser = ArgvPraser::OptionPraser<Cfg>();
    praser.add("tl", &Cfg::timeLimit);
    praser.add("ml", &Cfg::memLimit);
    praser.add("ol", &Cfg::outPutLimit);
    praser.add("uid", &Cfg::uid);
    praser.add("gid", &Cfg::gid);
    praser.add("pidl", &Cfg::maxPid);
    praser.add("stdout", &Cfg::stdout);
    praser.add("stderr", &Cfg::stderr);
    praser.add("args", &Cfg::args);
    praser.add("bin", &Cfg::bin, true);
    auto arg = praser.prase(argc, argv);
    std::cout << nlohmann::json(arg).dump(4) << std::endl;
    return 0;
}
}  // namespace HengCore

int main(int argc, char *argv[])
{
    int returnCode;
    {
        returnCode = HengCore::main(argc, argv);
    }
    return returnCode;
}