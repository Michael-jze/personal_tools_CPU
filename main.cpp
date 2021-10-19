#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
using namespace std;

#define PROC "/proc"
#define SAME_PROG_NR 256
#define CPU_NR 64

// 计算总体的CPU_IDLE
struct idle_slices{
    unsigned long idle_slice;
    unsigned long all_slice;
};

// 分别记录每个以及总体cpu上次和本次的idle时间片
idle_slices all_cpu_idle_last, all_cpu_idle_now;
idle_slices each_cpu_idle_last[CPU_NR], each_cpu_idle_now[CPU_NR];

// 计算单个进程的CPU和IO使用率
struct prog_cpu_io{
    int pid; // 进程的pid
    unsigned long prog_cpu_slices; // 进程的CPU时间片数
    double io_read_kb; // 进程读入的KB数
    double io_write_kb; // 进程写出的KB数
};

// 记录同意部署路径的所有进程的CPU和IO积累量
prog_cpu_io prog_mon_info_last[SAME_PROG_NR], prog_mon_info_now[SAME_PROG_NR];

// 两次采样的时间点
timeval time_last, time_now;

string hostname = ""; // 主机名
string mod_name = ""; // 模块名

unsigned int cpu_nr = 0; // CPU数量
DIR* pdir;

int prog_on_which_cpu[CPU_NR] = {0};
int global_cpu_idx_prog_on = 0;

unsigned int get_cpu_nr(){
    unsigned int cpu_num = 0;
    char buf[1024];
    FILE* fp = fopen("/proc/stat","r");
    while (fgets(buf, sizeof(buf), fp))
    {
        if(0 == strncmp(buf, "cpu", 3)){
            cpu_num++;
        } else {
            if(--cpu_num == 0){
                cpu_num = 1;
            }
            break;
        }
    }
    fclose(fp);
    return cpu_num;
}

// 默认参数
char* bin_path = NULL; // 部署路径
int delay = 2; // 默认监控周期 2秒

int prog_nr_now = -1; // 目前的同意部署路径的进程数

// 获取监控信息
void get_monitor_info(){
    char buffer[4096+1];
    char* p;
    int fd, len;
    dirent* pdirent;
    prog_nr_now = -1;
    FILE* fp;
    
// for fscanf
    int tmp_int;
    char tmp_str[64];
    unsigned long user_slice = 0, nice_slice = 0, sys_slice, idle_slice;
    unsigned long utime = 0, stime = 0;
    int which_cpu_proc_assigned_to = -1;
    global_cpu_idx_prog_on = 0;

// 清空prog_on_which_cpu
    int cpu_idx = 0;
    while(cpu_idx < CPU_NR){
        prog_on_which_cpu[cpu_idx] = 0;
        cpu_idx++;
    }
    cpu_idx = 0;
    
// 获取bin_path对应的程序的CPU利用率
    char pathname[32] = {0};
    // 此时pdir指向/proc, 遍历所有/proc/pid目录
    while ((pdirent = readdir(pdir)) != NULL)
    {
        if (strcmp(&pdirent->d_name[0],"0")>0 && (strcmp(&pdirent->d_name[0],"9")<=0))
        {
            strcpy(pathname, pdirent->d_name);
            strcat(pathname,"/exe");
            if(0!= access(pathname,F_OK)){
                continue;
            }
            // 读取/proc/pid/exe到buffer
            len = readlink(pathname, buffer, 4096);
            buffer[len] = '\0';

            // 如果找到同名部署路径,读取其stat
            if(strcmp(buffer, bin_path) == 0){
                *(strrchr(pathname, '/')) = '\0'; // 为下面的strcat连接点
                strcat(pathname,"/stat");

                if(NULL == (fp = fopen(pathname, "r"))){
                    fclose(fp);
                    continue;
                }

                // 读取/proc/pid/stat
                // TODO
            }
        }
        
    }
        

}

int main(){


    return 0;
}