cgdel(){
    cgpath=$1
    ls $cgpath | while read line
    do
	    rmdir $line
    done
    rmdir $cgpath
}

cgdel /sys/fs/cgroup/cpu/hengCore/
cgdel /sys/fs/cgroup/cpuacct/hengCore/
cgdel /sys/fs/cgroup/memory/hengCore/
cgdel /sys/fs/cgroup/pids/hengCore/