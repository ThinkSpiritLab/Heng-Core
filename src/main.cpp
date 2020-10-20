#include "config.hpp"
#include "hc.hpp"

#include "util/argvPraser.hpp"

#include <iostream>

#include "json.hpp"
using Cfg       = HengCore::Config::Config;
using Excutable = HengCore::Excutable;
int main(int argc, char *argv[])
{
    auto praser = ArgvPraser::OptionPraser<Cfg>();
    praser.add("tl", &Cfg::timeLimit);
    praser.add("ml", &Cfg::memLimit);
    praser.add("ol", &Cfg::outPutLimit);
    praser.add("uid", &Cfg::uid);
    praser.add("u", &Cfg::uid);
    praser.add("gid", &Cfg::gid);
    praser.add("g", &Cfg::gid);
    praser.add("pidl", &Cfg::maxPid);
    praser.add("p", &Cfg::maxPid);
    praser.add("stdout", &Cfg::stdout);
    praser.add("o", &Cfg::stdout);
    praser.add("stderr", &Cfg::stderr);
    praser.add("e", &Cfg::stderr);
    praser.add("args", &Cfg::args);
    praser.add("a", &Cfg::args);
    praser.add("bin", &Cfg::bin, true);
    // std::cout << "#" << argv[0] << std::endl;
    auto      arg = praser.prase(argc, argv);
    Excutable excutable(arg);
    std::cout << nlohmann::json(arg).dump(4) << std::endl;
    excutable.exec();
    // excutable.killChild();
    std::cout
      << nlohmann::json(excutable.getResult()).dump(4)
      << std::endl;
    return 0;
}