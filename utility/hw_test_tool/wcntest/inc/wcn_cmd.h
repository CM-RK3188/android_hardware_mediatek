
#ifndef __WCN_CMD_H__
#define __WCN_CMD_H__

enum wcn_cmd_parser_state
{
    WCN_CMD_STAT_NONE = 0,
    WCN_CMD_STAT_WORD
};

typedef enum wcn_cfg_parser_state wcn_cfg_parser_state_t;

#define COMMENT_CHAR '#'
#define DELIMIT_CHAR '='
#define isspace(a) ((a) == 0x20)


struct wcn_cmd 
{
    char *name;
    int para_size;
    int (*handler)(struct wcn_cmd *cmd);
    char *para[3];
    char *description;
};

enum wcn_sta 
{
    WCN_STA_NON,
    WCN_STA_CMD,
    WCN_STA_VAL_1,
    WCN_STA_VAL_2
};

enum wcn_event_type 
{
    WCN_EVENT_VAL,   //a value string
    WCN_EVENT_CMD    //a cmd string, the cmd need 0 para
};

#endif //__WCN_CMD_H__