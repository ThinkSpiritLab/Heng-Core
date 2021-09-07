cgdel(){
    path=$1
    ls $path | while read line
    do
	    rmdir $line
    done
    rmdir $path
}

cgdel /sys/fs/cgroup/cpu/hengCore/
cgdel /sys/fs/cgroup/cpuacct/hengCore/
cgdel /sys/fs/cgroup/memory/hengCore/
cgdel /sys/fs/cgroup/pids/hengCore/