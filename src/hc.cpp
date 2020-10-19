#include "hc.hpp"

#include "./util/argvPraser.hpp"

#include <iostream>
namespace HengCore
{

int main(int argc, char *argv[])
{

    auto praser =
      ArgvPraser::OptionPraser<HengCore::Config>();
    praser.add("tl", &Config::timeLimit);
    praser.add("ml", &Config::memLimit);
    praser.add("ol", &Config::outPutLimit);
    praser.add("uid", &Config::uid);
    praser.add("gid", &Config::gid);
    praser.add("pidl", &Config::maxPid);
    praser.add("stdout", &Config::stdout);
    praser.add("stderr", &Config::stderr);
    praser.add("args", &Config::args);
    praser.add("bin", &Config::bin, true);
    auto arg = praser.prase(argc, argv);
    std::cout << arg.toJson() << std::endl;
    return 0;
}
}  // namespace HengCore

int main(int argc, char *argv[])
{
    return HengCore::main(argc, argv);
}