#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/fcntl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>

namespace fs = std::filesystem;

// error
#define THROW_ERROR(msg) do { \
    std::cerr << "ERROR: " << (msg) << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
    throw std::runtime_error(msg); \
} while(0)

int pid;

extern ssize_t calculateStaticMemoryUsageInByte(const std::string &);

ssize_t getMemoryRLimit(ssize_t memoryLimitInMiB) { return memoryLimitInMiB * 1024 * 1024; }

size_t getMaxRSSInByte(long ru_maxrss) { return ru_maxrss * 1024; }

void write_cgroup_file(const std::string& path, const std::string& value) {
    std::ofstream file(path);
    if (!file.is_open()) {
        THROW_ERROR("Failed to open cgroup file: " + path);
    }
    file << value;
    if (file.fail()) {
        THROW_ERROR("Failed to write to cgroup file: " + path);
    }
}

std::string read_cgroup_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        THROW_ERROR("Failed to open cgroup file: " + path);
    }
    std::string content;
    file >> content;
    if (file.fail() && !file.eof()) {
        THROW_ERROR("Failed to read from cgroup file: " + path);
    }
    return content;
}

void cleanUp(const std::string& path) {
    try {
        // kill all threads in cgroup
        std::string procs_file = path + "/cgroup.procs";
        if (fs::exists(procs_file)) {
            std::ifstream file(procs_file);
            if (file.is_open()) {
                pid_t pid;
                while (file >> pid) {
                    if (pid > 0 && pid != getpid()) {
                        kill(pid, SIGKILL);
                    }
                }
            }
        }
        
        // remove cgroup dir
        fs::remove_all(path);
    } catch (...) {
    }
}

/**
 * argv[1]: QString("\"%1\" %2").arg(executableFile, arguments) in `judgingthread.cpp` 执行命令
 * argv[2]: 重定向输入文件（如果有）
 * argv[3]: 重定向输出文件（如果有）
 * argv[4]: 重定向错误流文件（如果有）
 * argv[5]: 时间限制（毫秒）
 * argv[6]: 空间限制（MiB），若为负数表示无限制
 */

auto main(int /*argc*/, char *argv[]) -> int {
    int timeLimitInMS = 0;
    ssize_t memoryLimitInMiB = 0;
    sscanf(argv[5], "%d", &timeLimitInMS);
    sscanf(argv[6], "%zd", &memoryLimitInMiB);

    // 匹配 "" 来解析出文件名
    std::string fileName(argv[1]);
    fileName = fileName.substr(1);
    fileName = fileName.substr(0, fileName.find("\""));

    if (memoryLimitInMiB > 0) {
        ssize_t staticMemoryUsageInByte = calculateStaticMemoryUsageInByte(fileName);
        if (staticMemoryUsageInByte == -1) {
            return 1;
        }
        if (staticMemoryUsageInByte > memoryLimitInMiB * 1024 * 1024) {
            printf("0\n%zd\n", staticMemoryUsageInByte);
            return 0;
        }
    }

    std::string cgroup_name = "cgroup_runner_" + std::to_string(getpid());
    std::string cgroup_path = "/sys/fs/cgroup/" + cgroup_name;

    if (!fs::create_directory(cgroup_path)) {
        THROW_ERROR("Failed to create cgroup directory: " + cgroup_path);
    }

    if (memoryLimitInMiB > 0) {
        unsigned long long memoryLimitInByte = static_cast<unsigned long long>(memoryLimitInMiB) * 1024 * 1024;
        write_cgroup_file(cgroup_path + "/memory.max", std::to_string(memoryLimitInByte));
        write_cgroup_file(cgroup_path + "/memory.swap.max", "0"); // 禁用swap
    }

    write_cgroup_file(cgroup_path + "/pids.max", "1024");

    struct timespec start_time, end_time, present_time;
    // start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    pid_t child_pid = fork();
    if (child_pid == -1) {
        THROW_ERROR("Failed to fork");
    }

    if (child_pid == 0) { // child
            write_cgroup_file(cgroup_path + "/cgroup.procs", std::to_string(getpid()));

            if (strlen(argv[2]) > 0) {
                freopen(argv[2], "r", stdin);
            }

            if (strlen(argv[3]) > 0) {
                freopen(argv[3], "w", stdout);
            }

            if (strlen(argv[4]) > 0) {
                freopen(argv[4], "w", stderr);
            }

            rlimit timeRLimit{};
            int timeLimitInSec = (timeLimitInMS -1) / 1000 + 1;
            timeRLimit = (rlimit){(rlim_t)timeLimitInSec, (rlim_t)(timeLimitInSec + 1)};
            setrlimit(RLIMIT_CPU, &timeRLimit);

            if (execlp("bash", "bash", "-c", argv[1], NULL) == -1) {
                return 1;
            }
    } else { // father
        //wait
        int status;
        pid_t waited_pid = waitpid(child_pid, &status, 0);

        // end time
        clock_gettime(CLOCK_MONOTONIC, &end_time);
    
        // clac time in ms
        long timeUsageInMS = (end_time.tv_sec - start_time.tv_sec) * 1000;
        timeUsageInMS += (end_time.tv_nsec - start_time.tv_nsec) / 1000000;
    
        // fetch peak memory usage
        unsigned long long peakMemoryInByte = 0;
        try {
            peakMemoryInByte = std::stoull(read_cgroup_file(cgroup_path + "/memory.peak"));
        } catch (...) {

        }
        
        int exit_code = 0;
        bool print_flag = false;

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 1) {
                exit_code = 1;
            } else {
                print_flag = true;
                if (WEXITSTATUS(status) != 0) {
                    exit_code = 2;
                } else {
                    exit_code  = 0;
                }
            }
        } else if (WIFSIGNALED(status)) {
            print_flag = true;

            int signal_num = WTERMSIG(status);
            switch (signal_num) {
                case SIGXCPU: exit_code = 3;break;
                case SIGKILL: exit_code = 4;break;
                case SIGABRT: exit_code = 4;break;
                default: exit_code = 2;
            }
        }

        cleanUp(cgroup_path);
        
        if (print_flag) {
            printf("%d\n", timeLimitInMS);
			printf("%zu\n", peakMemoryInByte);
        }
        return exit_code;
    }

    return 0;
}
 