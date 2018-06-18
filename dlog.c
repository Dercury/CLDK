/*
 * =====================================================================================
 *
 *       Filename:  dlog.c
 *
 *    Description:  log record function source
 *
 *        Version:  1.0
 *        Created:  2016年10月01日 17时04分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dercury (Jim), dercury@qq.com
 *   Organization:  Perfect World
 *
 * =====================================================================================
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dlog.h"

/******************************************************************************/
typedef struct tagModuleLogDesc
{
    char name[MODULE_NAME_LEN];
    int log_size;

    void* base_addr;    /* log base addr */
    void* cur_addr;
    uint32_t log_cnt;   /* count num */

    char is_full;   /* whether log is full */
    char log_num;   /* whether record count num */
    char log_time;  /* whether record time */
    char log_module;/* whether record module name */
} ModuleLogDesc;

ModuleLogDesc g_module_log_desc[MODULE_MAX_NUM] = {0};

/******************************************************************************/
typedef struct tagLogDesc
{
    void* log_addr;
    uint32_t log_size;
    
    uint32_t module_num;
    void* module_addr[MODULE_MAX_NUM];  /* module desc addr */
} LogDesc;

LogDesc g_log_desc = {0};

/******************************************************************************/
#define LOG_TAG_DWORD_NUM   4

const uint32_t g_log_tag[LOG_TAG_DWORD_NUM] = {0xFEDCBA98, 0x76543210, 0x01234567, 0x89ABCDEF};
uint32_t g_log_tag_size = sizeof(g_log_tag);

/******************************************************************************/
#define INVALID 0
#define VALID   1

#define DECIMAL_STR_LEN 11
#define TIME_STR_LEN    22
#define SEPARATOR_LEN   6

#define LOG_PREFIX_LEN  (DECIMAL_STR_LEN + TIME_STR_LEN + MODULE_NAME_LEN + SEPARATOR_LEN)

#define LOG_TAIL_TAG_ADDR(log_addr, log_size)   ((void*)((char*)(log_addr) + (log_size) - g_log_tag_size))
#define LOG_DESC_ADDR(log_addr, log_size)       ((void*)((char*)(log_addr) + (log_size) - g_log_tag_size - sizeof(LogDesc)))

/******************************************************************************/
#include <assert.h>
#include <time.h>
void get_time_str(char* buf, uint32_t buf_len)
{
    time_t timep;
    struct tm *p;

    assert(NULL != buf);
    assert(buf_len >= TIME_STR_LEN);

    time(&timep);
    p = localtime(&timep);

    snprintf(buf, buf_len - 1, "%04d-%02d-%02d.%02d:%02d:%02d", (1900 + p->tm_year),
            (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    return;
}

/******************************************************************************/
uint32_t dlog_init_check_para(void* log_addr, uint32_t log_size, uint32_t module_num, ModuleLogCfg* module_cfg)
{
    uint32_t need_log_size = 0;
    uint32_t module_id = 0;

    if ((NULL == log_addr) || (NULL == module_cfg) || (0 == module_num) || (MODULE_MAX_NUM < module_num))
    {
        return FUNC_BAD_PARA;
    }

    need_log_size = 2 * g_log_tag_size + sizeof(LogDesc) + module_num * sizeof(ModuleLogDesc);
    for (module_id = 0; module_id < module_num; module_id++)
    {
        if (module_cfg[module_id].log_size < SINGLE_LOG_MAX_LEN)
        {
            return FUNC_BAD_PARA;
        }
        need_log_size += module_cfg[module_id].log_size;
    }

    if (need_log_size > log_size)
    {
        return FUNC_BAD_PARA;
    }

    return FUNC_OK;
}

/******************************************************************************/
/* If head tag and tail tag are both correct, log has been initialized. 
 * So there is no need to initialize it again. 
 */
uint32_t dlog_init_check_tag(void* log_addr, uint32_t log_size)
{
    uint32_t log_tag[LOG_TAG_DWORD_NUM] = {0};

    /* check head tag */
    memcpy((void*)log_tag, log_addr, g_log_tag_size);
    if (0 != memcmp((void*)log_tag, (void*)g_log_tag, g_log_tag_size))
    {
        return INVALID;
    }

    /* check tail tag */
    memcpy((void*)log_tag, LOG_TAIL_TAG_ADDR(log_addr, log_size), g_log_tag_size);
    if (0 != memcmp((void*)log_tag, (void*)g_log_tag, g_log_tag_size))
    {
        return INVALID;
    }

    return VALID;
}

/******************************************************************************/
void dlog_init_write_tag(void* log_addr, uint32_t log_size)
{
    memcpy(log_addr, (void*)g_log_tag, g_log_tag_size);
    memcpy(LOG_TAIL_TAG_ADDR(log_addr, log_size), (void*)g_log_tag, g_log_tag_size);

    return;
}

/******************************************************************************/
void dlog_init_read_desc(void* log_addr, uint32_t log_size)
{
    memcpy((void*)&g_log_desc, LOG_DESC_ADDR(log_addr, log_size), sizeof(g_log_desc));

    return;
}

/******************************************************************************/
void dlog_init_write_desc(void* log_addr, uint32_t log_size, uint32_t module_num)
{
    g_log_desc.log_addr = log_addr;
    g_log_desc.log_size = log_size;
    g_log_desc.module_num = module_num;
    memcpy(LOG_DESC_ADDR(log_addr, log_size), (void*)&g_log_desc, sizeof(g_log_desc));

    return;
}

/******************************************************************************/
void dlog_init_module_desc(void* log_addr, uint32_t log_size, uint32_t module_num, ModuleLogCfg* module_cfg)
{
    uint32_t module_id = 0;
    ModuleLogDesc module_desc;
    char* module_addr = NULL;
    char* module_desc_addr = (char*)LOG_DESC_ADDR(log_addr, log_size);

    for (module_id = 0; module_id < module_num; module_id++)
    {
        module_desc_addr -= sizeof(ModuleLogDesc);
        module_addr = module_desc_addr - module_cfg[module_id].log_size;

        memset((void*)&module_desc, 0, sizeof(ModuleLogDesc));

        memcpy((void*)module_desc.name, module_cfg[module_id].name, strlen(module_cfg[module_id].name));
        module_desc.log_size = module_cfg[module_id].log_size;

        module_desc.base_addr = module_addr;
        module_desc.cur_addr = module_addr;

        module_desc.log_num = module_cfg[module_id].log_num;
        module_desc.log_time = module_cfg[module_id].log_time;
        module_desc.log_module = module_cfg[module_id].log_module;

        memcpy((void*)module_desc_addr, &module_desc, sizeof(ModuleLogDesc));
        memcpy((void*)&g_module_log_desc[module_id], &module_desc, sizeof(ModuleLogDesc));

        g_log_desc.module_addr[module_id] = module_desc_addr;

        module_desc_addr = module_addr; 
    }

    return;
}

/******************************************************************************/
uint32_t dlog_init(void* log_addr, uint32_t log_size, uint32_t module_num, ModuleLogCfg* module_cfg)
{
    uint32_t func_ret = FUNC_OK;

    func_ret = dlog_init_check_para(log_addr, log_size, module_num, module_cfg);
    if (FUNC_OK != func_ret)
    {
        return func_ret;
    }

    func_ret = dlog_init_check_tag(log_addr, log_size);
    if (VALID == func_ret)
    {
        /* log has been initialized */
        dlog_init_read_desc(log_addr, log_size);
        return FUNC_OK;
    }

    dlog_init_module_desc(log_addr, log_size, module_num, module_cfg);
    dlog_init_write_desc(log_addr, log_size, module_num);

    /* write tag after init desc info to make sure init all ok */
    dlog_init_write_tag(log_addr, log_size);

    return FUNC_OK;
}

/******************************************************************************/
void dlog_get_log_prefix(ModuleLogDesc* module_desc, char* prefix_buf, uint32_t buf_len)
{
    uint32_t cur_len = 0;
    char time_str[TIME_STR_LEN] = {0};

    if (module_desc->log_num)
    {
        cur_len += snprintf(prefix_buf + cur_len, buf_len - cur_len - 1, "[%u]", module_desc->log_cnt + 1);
    }

    if (module_desc->log_time)
    {
        get_time_str(time_str, TIME_STR_LEN);
        cur_len += snprintf(prefix_buf + cur_len, buf_len - cur_len - 1, "[%s]", time_str);
    }
    
    if (module_desc->log_module)
    {
        cur_len += snprintf(prefix_buf + cur_len, buf_len - cur_len - 1, "[%s]", module_desc->name);
    }

    return;
}

/******************************************************************************/
void dlog_module_write_str(ModuleLogDesc* module_desc, char* log_str)
{
    uint32_t empty_len = module_desc->base_addr + module_desc->log_size - module_desc->cur_addr;
    uint32_t log_len = strlen(log_str);

    if (empty_len >= log_len)
    {
        memcpy(module_desc->cur_addr, log_str, log_len);
        module_desc->cur_addr += log_len;
    }
    else
    {
        memcpy(module_desc->cur_addr, log_str, empty_len);
        module_desc->is_full = VALID;
        module_desc->cur_addr = module_desc->base_addr;

        memcpy(module_desc->cur_addr, log_str + empty_len, log_len - empty_len);
        module_desc->cur_addr += log_len - empty_len;
    }

    return;
}

/******************************************************************************/
void dlog_module_write(uint32_t module_id, char* log_str)
{
    ModuleLogDesc* module_desc = &g_module_log_desc[module_id];
    char log_prefix[LOG_PREFIX_LEN] = {0};

    dlog_get_log_prefix(module_desc, log_prefix, LOG_PREFIX_LEN);
    dlog_module_write_str(module_desc, log_prefix);
    dlog_module_write_str(module_desc, log_str);

    module_desc->log_cnt++;

    /* write cur_addr to memory */
    memcpy(g_log_desc.module_addr[module_id], module_desc, sizeof(ModuleLogDesc));

    return;
}
/******************************************************************************/
uint32_t dlog_write(uint32_t module_id, char* format, ...)
{
    char* log_buf = NULL;
    va_list argptr;

    if ((module_id >= g_log_desc.module_num) || (NULL == format))
    {
        return FUNC_BAD_PARA;
    }

    log_buf = (char*)malloc(SINGLE_LOG_MAX_LEN);
    if (NULL == log_buf)
    {
        return FUNC_MALLOC_FAIL;
    }
    memset((void*)log_buf, 0, SINGLE_LOG_MAX_LEN);

    va_start(argptr, format);
    if (0 > vsnprintf(log_buf, SINGLE_LOG_MAX_LEN - 1, format, argptr))
    {
        va_end(argptr);
        free(log_buf);
        return FUNC_FAIL;
    }
    va_end(argptr);

    dlog_module_write(module_id, log_buf);

    free(log_buf);

    return FUNC_OK;
}

/******************************************************************************/
uint32_t dlog_read(uint32_t module_id, char* buf, uint32_t buf_len)
{
    uint32_t cur_len = 0;
    uint32_t copy_len = 0;
    uint32_t left_len = 0;
    ModuleLogDesc* module_desc = NULL;

    if ((module_id >= g_log_desc.module_num) || (NULL == buf) || (0 == buf_len))
    {
        return FUNC_BAD_PARA;
    }

    module_desc = &g_module_log_desc[module_id];

    cur_len = module_desc->cur_addr - module_desc->base_addr;
    if (module_desc->is_full)
    {
        left_len = module_desc->log_size - cur_len;
        if (left_len >= buf_len)
        {
            memcpy(buf, module_desc->cur_addr, buf_len);
        }
        else
        {
            memcpy(buf, module_desc->cur_addr, left_len);
            if ((buf_len - left_len) < cur_len)
            {
                memcpy(buf + left_len, module_desc->base_addr, buf_len - left_len);
            }
            else
            {
                memcpy(buf + left_len, module_desc->base_addr, cur_len);
            }
        }
    }
    else
    {
        copy_len = (buf_len > cur_len) ? cur_len : buf_len;
        memcpy(buf, module_desc->base_addr, copy_len);
    }

    return FUNC_OK;
}

/******************************************************************************/
uint32_t dlog_clear(uint32_t module_id)
{
    ModuleLogDesc* module_desc = NULL;

    if (module_id >= g_log_desc.module_num)
    {
        return FUNC_BAD_PARA;
    }

    module_desc = &g_module_log_desc[module_id];
    module_desc->cur_addr = module_desc->base_addr;
    module_desc->log_cnt = 0;
    module_desc->is_full = 0;

    memcpy(g_log_desc.module_addr[module_id], module_desc, sizeof(ModuleLogDesc));

    return FUNC_OK;
}

/******************************************************************************/
void dlog_uninit(void)
{
    if ((NULL != g_log_desc.log_addr) && (0 != g_log_desc.log_size))
    {
        memset(g_log_desc.log_addr, 0, g_log_desc.log_size);
    }
    memset(&g_log_desc, 0, sizeof(g_log_desc));
    memset(g_module_log_desc, 0, sizeof(g_module_log_desc));

    return;
}

/******************************************************************************/

