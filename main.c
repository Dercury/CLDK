/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  test dlog
 *
 *        Version:  1.0
 *        Created:  2016年10月01日 19时18分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dercury (Jim), dercury@qq.com
 *   Organization:  Perfect World
 *
 * =====================================================================================
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dlog.h"

ModuleLogCfg g_module_cfg[] = 
{
    /* Name Size LogNum LogTime LogModule */
    {"DLOG_C", 30*1024, 1, 1, 1},
    {"DLOG_H", 2*1024, 1, 1, 1},
    {"MAIN_C", 2*1024, 0, 0, 0},
};

void read_file_to_log(char* filename, uint32_t module_id)
{
    FILE* fp = NULL;
    char line[SINGLE_LOG_MAX_LEN] = {0};

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("\r\n open file<%s> fail", filename);
        return;
    }
    
    while (!feof(fp))
    {
        memset(line, 0, SINGLE_LOG_MAX_LEN);
        fgets(line, SINGLE_LOG_MAX_LEN, fp);
        if (FUNC_OK != dlog_write(module_id, "%s", line))
        {
            printf("\r\n dlog write line'%s' fail", line);
            break;
        }
    }

    fclose(fp);

    return;
}

void read_log_to_file(uint32_t module_id, char* filename)
{
    uint32_t buf_len = g_module_cfg[module_id].log_size * 2;
    char* buf = malloc(buf_len);
    FILE* fp = NULL;

    assert(NULL != buf);
    memset(buf, 0, buf_len);

    do
    {
        if (FUNC_OK != dlog_read(module_id, buf, buf_len))
        {
            printf("\r\n dlog read module'%u' fail", module_id);
            break;
        }

        if ((fp = fopen(filename, "w")) == NULL)
        {
            printf("\r\n open file<%s> fail", filename);
            break;
        }

        fputs(buf, fp);
        fclose(fp);
    } while (0);

    free(buf);

    return;
}

int main(int argc, char** argv)
{
    uint32_t func_ret = 0;
    uint32_t log_size = 1024 * 1024;
    void* log_addr = malloc(log_size);
    uint32_t module_num = sizeof(g_module_cfg) / sizeof(g_module_cfg[0]);

    assert (NULL != log_addr);

    func_ret = dlog_init(log_addr, log_size, module_num, g_module_cfg);
    if (FUNC_OK != func_ret)
    {
        printf("\r\n<%d> func_ret=%u", __LINE__, func_ret);
        free(log_addr);
        return func_ret;
    }

    read_file_to_log("dlog.c", 0);
    read_file_to_log("dlog.h", 1);
    read_file_to_log("main.c", 2);

    read_log_to_file(0, "dlog_c.log");
    read_log_to_file(1, "dlog_h.log");
    read_log_to_file(2, "main_c.log");
    dlog_clear(2);
    read_log_to_file(2, "main_h.log");

    dlog_uninit();
    read_log_to_file(1, "dlog_i.log");
    free(log_addr);

    return 0;
}

