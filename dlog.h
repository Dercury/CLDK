/*
 * =====================================================================================
 *
 *       Filename:  dlog.h
 *
 *    Description:  log record function header
 *
 *        Version:  1.0
 *        Created:  2016年10月01日 16时28分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dercury (Jim), dercury@qq.com
 *   Organization:  Perfect World
 *
 * =====================================================================================
 */

#ifndef DLOG_H
#define DLOG_H

#include <stdint.h>

/******************************************************************************/
enum
{
    FUNC_OK = 0,
    FUNC_FAIL,
    FUNC_BAD_PARA,
    FUNC_MALLOC_FAIL,
};

/******************************************************************************/
#define MODULE_MAX_NUM      10
#define MODULE_NAME_LEN     32
#define SINGLE_LOG_MAX_LEN  256

typedef struct tagModuleLogCfg
{
    char name[MODULE_NAME_LEN];
    uint32_t log_size;

    char log_num;
    char log_time;
    char log_module;
} ModuleLogCfg;

/******************************************************************************/

uint32_t dlog_init(void* log_addr, uint32_t log_size, uint32_t module_num, ModuleLogCfg* module_cfg);

/******************************************************************************/
uint32_t dlog_write(uint32_t module_id, char* format, ...);

/******************************************************************************/
uint32_t dlog_read(uint32_t module_id, char* buf, uint32_t buf_len);

/******************************************************************************/
uint32_t dlog_clear(uint32_t module_id);

/******************************************************************************/
void dlog_uninit(void);

/******************************************************************************/
#endif /* DLOG_H */

