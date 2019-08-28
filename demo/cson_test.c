/**
 * @file cson_test.c
 * @author Letter (NevermindZZT@gmail.cn)
 * @brief 
 * @version 0.1
 * @date 2019-08-27
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "ulog.h"
#include "cson.h"


struct subtest
{
    int id;
    char *test;
};

struct test
{
    char id;
    short num;
    int max;
    double value;
    char *name;
    struct subtest *sub;
    CsonList *list;
    char *str[2];
    CsonList *charList;
    CsonList *strList;
};

CsonModel subModel[] = 
{
    CSON_MODEL_OBJ(struct subtest),
    CSON_MODEL_INT(struct subtest, id),
    CSON_MODEL_STRING(struct subtest, test)
};


CsonModel model[] = 
{
    CSON_MODEL_OBJ(struct test),
    CSON_MODEL_CHAR(struct test, id),
    CSON_MODEL_SHORT(struct test, num),
    CSON_MODEL_INT(struct test, max),
    CSON_MODEL_DOUBLE(struct test, value),
    CSON_MODEL_STRING(struct test, name),
    CSON_MODEL_STRUCT(struct test, sub, subModel, sizeof(subModel)/sizeof(CsonModel)),
    CSON_MODEL_LIST(struct test, list, subModel, sizeof(subModel)/sizeof(CsonModel)),
    CSON_MODEL_ARRAY(struct test, str, CSON_TYPE_STRING, 2),
    CSON_MODEL_LIST(struct test, charList, CSON_MODEL_INT_LIST, CSON_BASIC_LIST_MODEL_SIZE),
    CSON_MODEL_LIST(struct test, strList, CSON_MODEL_STRING_LIST, CSON_BASIC_LIST_MODEL_SIZE)
};




void csonTest(void)
{
    char *jsonStr = "{\"id\": 1, \"num\": 300, \"max\": 1000, \"value\": 10.3, \"name\": \"letter\", "
    "\"sub\": {\"id\": 20, \"test\": \"hello world\"},"
    " \"list\": [{\"id\": 21, \"test\": \"hello cson\"}, {\"id\": 22, \"test\": \"hello letter\"}],"
    "\"str\": [\"array1\", \"array2\"],"
    "\"charList\": [1, 12, 52], "
    "\"strList\": [\"str1\", \"str2\"]}";

    struct test *st = csonDecode(jsonStr, model, sizeof(model)/sizeof(CsonModel));
    ulogDebug("josn 0x%08x, id: %d, num: %d, max: %d, value: %lf, name: %s\r\nsub: id: %d, test: %s",
        st, st->id, st->num, st->max, st->value, st->name, st->sub->id, st->sub->test);
    ulogDebug("str: %s %s", st->str[0], st->str[1]);
    CsonList *p = st->list;
    while (p)
    {
        struct subtest *sst = p->obj;
        if (p->obj)
        {
            ulogDebug("list: id: %d, test: %s", sst->id, sst->test);
        }
        p = p->next;
    }
    p = st->charList;
    while (p)
    {
        int *sst = p->obj;
        if (p->obj)
        {
            ulogDebug("list: id: %d", *sst);
        }
        p = p->next;
    }
    p = st->strList;
    while (p)
    {
        char **sst = p->obj;
        if (p->obj)
        {
            ulogDebug("list: id: %s", *sst);
        }
        p = p->next;
    }

    char *root = csonEncode(st, model, sizeof(model)/sizeof(CsonModel), 512, 0);
    printf("encode: %s", root);

    csonFreeJson(root);
    csonFree(st, model, sizeof(model)/sizeof(CsonModel));
}
SHELL_EXPORT_CMD(csonTest, csonTest, test);


/** 项目结构体 */
struct project
{
    int id;
    char *name;
};

/** 仓库结构体 */
struct hub
{
    int id;
    char *user;
    struct project *cson;
};

/** 项目结构体数据模型 */
CsonModel projectModel[] = 
{
    CSON_MODEL_OBJ(struct project),
    CSON_MODEL_INT(struct project, id),
    CSON_MODEL_STRING(struct project, name),
};

/** 仓库结构体数据模型 */
CsonModel hubModel[] = 
{
    CSON_MODEL_OBJ(struct hub),
    CSON_MODEL_INT(struct hub, id),
    CSON_MODEL_STRING(struct hub, user),
    CSON_MODEL_STRUCT(struct hub, cson, projectModel, sizeof(projectModel)/sizeof(CsonModel))
};


void csonDemo(void)
{
    char *jsonDemo = "{\"id\": 1, \"user\": \"Letter\", \"cson\": {\"id\": 2, \"name\": \"cson\"}}";
    
    /** 解析json */
    struct hub *pHub = csonDecode(jsonDemo, hubModel, sizeof(hubModel)/sizeof(CsonModel));
    printf("hub: id: %d, user: %s, project id: %d, project name: %s\r\n",
        pHub->id, pHub->user, pHub->cson->id, pHub->cson->name);

    /** 序列化对象 */
    char *formatJson = csonEncodeFormatted(pHub, hubModel, sizeof(hubModel)/sizeof(CsonModel));
    printf("format json: %s\r\n", formatJson);

    /** 释放结构体对象 */
    csonFree(pHub, hubModel, sizeof(hubModel)/sizeof(CsonModel));

    /** 释放序列化生成的json字符串 */
    csonFreeJson(formatJson);
}
SHELL_EXPORT_CMD(csonDemo, csonDemo, test);

