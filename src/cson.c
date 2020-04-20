/**
 * @file cson.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief cson
 * @version 0.1
 * @date 2019-08-26
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "cson.h"
#include "cJSON.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"


/**
 * @brief 基本类型链表数据模型
 * 
 */
CsonModel csonBasicListModel[] =
{
    [0] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(char)},
    [1] = {CSON_TYPE_CHAR, NULL, 0},
    [2] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(short)},
    [3] = {CSON_TYPE_SHORT, NULL, 0},
    [4] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(int)},
    [5] = {CSON_TYPE_INT, NULL, 0},
    [6] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(long)},
    [7] = {CSON_TYPE_LONG, NULL, 0},
    [8] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(float)},
    [9] = {CSON_TYPE_FLOAT, NULL, 0},
    [10] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(double)},
    [11] = {CSON_TYPE_DOUBLE, NULL, 0},
    [12] = {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(char *)},
    [13] = {CSON_TYPE_STRING, NULL, 0},
};


/**
 * @brief cson
 * 
 */
struct {
    void *(*malloc)(int);
    void (*free)(void *);
} cson;


/**
 * @brief CSON初始化
 * 
 * @param malloc 内存分配函数
 * @param free 内存释放函数
 */
void csonInit(void *malloc, void *free)
{
    cson.malloc = (void *(*)(int))malloc;
    cson.free = (void (*)(void *))free;
    cJSON_InitHooks(&(cJSON_Hooks){(void *(*)(size_t))cson.malloc, cson.free});
}


/**
 * @brief 解析JSON整型
 * 
 * @param json JSON对象
 * @param key key
 * @return int 解析出的整型数
 */
int csonDecodeNumber(cJSON *json, char *key)
{
    cJSON *item = key ? cJSON_GetObjectItem(json, key) : json;
    if (item && item->type == cJSON_Number)
    {
        return item->valueint;
    }
    return 0;
}


/**
 * @brief 解析JSON浮点型
 * 
 * @param json JSON对象
 * @param key key
 * @return double 解析出的浮点型数
 */
double csonDecodeDouble(cJSON *json, char *key)
{
    cJSON *item = key ? cJSON_GetObjectItem(json, key) : json;
    if (item && item->type == cJSON_Number)
    {
        return item->valuedouble;
    }
    return 0.0;
}


/**
 * @brief 解析JSON字符串数据
 * 
 * @param json JSON对象
 * @param key key
 * @return char* 解析出的字符串
 */
char* csonDecodeString(cJSON *json, char *key)
{
    char *p = NULL;
    char *str = NULL;
    short strLen = 0;
    cJSON *item = key ? cJSON_GetObjectItem(json, key) : json;
    if (item && item->type == cJSON_String)
    {
        str = item->valuestring;
        if (item->valuestring)
        {
            strLen = strlen(str);
            p = cson.malloc(strLen + 1);
            if (p)
            {
                memcpy((void *)p, (void *)str, strLen);
                *(p + strLen) = 0;
                return p;
            }
        }
    }
    return NULL;
}


/**
 * @brief 解析JOSN布尔型数据
 * 
 * @param json JSON对象
 * @param key key
 * @return char 解析出的bool
 */
char csonDecodeBool(cJSON *json, char *key)
{
    cJSON *item = cJSON_GetObjectItem(json, key);
    if (item && item->type == cJSON_True)
    {
        return 1;
    }
    return 0;
}


/**
 * @brief 解析CsonList数据
 * 
 * @param json JSON对象
 * @param key key
 * @param model CsonList成员数据模型
 * @param modelSize SconList成员模型数量
 * @return void* CsonList对象
 */
void *csonDecodeList(cJSON *json, char *key, CsonModel *model, int modelSize)
{
    CsonList *list = NULL;
    cJSON *array = cJSON_GetObjectItem(json, key);

    if (array && array->type == cJSON_Array)
    {
        for (short i = 0; i < cJSON_GetArraySize(array); i++)
        {
            list = csonListAdd(list, csonDecodeObject(cJSON_GetArrayItem(array, i), model, modelSize));
        }
    }
    return list;
}


/**
 * @brief 解析数组
 * 
 * @param json json对象
 * @param key key
 * @param base 数组基址
 * @param elementType 数组元素类型
 * @param arraySize 数组大小
 */
void csonDecodeArray(cJSON *json, char *key, void * base, CsonType elementType, short arraySize)
{
    cJSON *array = cJSON_GetObjectItem(json, key);
    cJSON *item;
    char *str;
    short strLen;

    if (array && array->type == cJSON_Array)
    {
        for (short i = 0; i < cJSON_GetArraySize(array); i++)
        {
            item = cJSON_GetArrayItem(array, i);
            switch (elementType)
            {
            case CSON_TYPE_CHAR:
                *(char *)((int)base + (i * sizeof(char))) = (char)item->valueint;
                break;
            case CSON_TYPE_SHORT:
                *(short *)((int)base + (i * sizeof(short))) = (short)item->valueint;
                break;
            case CSON_TYPE_INT:
                *(int *)((int)base + (i * sizeof(int))) = (int)item->valueint;
                break;
            case CSON_TYPE_LONG:
                *(long *)((int)base + (i * sizeof(long))) = (long)item->valueint;
                break;
            case CSON_TYPE_FLOAT:
                *(float *)((int)base + (i * sizeof(float))) = (float)item->valuedouble;
                break;
            case CSON_TYPE_DOUBLE:
                *(double *)((int)base + (i * sizeof(double))) = (double)item->valuedouble;
                break;
            case CSON_TYPE_STRING:
                strLen = strlen(item->valuestring);
                str = cson.malloc(strLen + 1);
                memcpy(str, item->valuestring, strLen);
                *(str + strLen) = 0;
                *(int *)((int)base + (i * sizeof(int))) = (int)str;
                break;
            default:
                break;
            }
        }
    }
}


/**
 * @brief 解析JSON对象
 * 
 * @param json JSON对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return void* 解析得到的对象
 */
void *csonDecodeObject(cJSON *json, CsonModel *model, int modelSize)
{
    CSON_ASSERT(json, return NULL);

    if (json->type == cJSON_NULL) {
        return NULL;
    }

    short objSize = 0;
    for (short i = 0; i < modelSize; i++)
    {
        if (model[i].type == CSON_TYPE_OBJ)
        {
            objSize = model[i].param.objSize;
        }
    }
    void *obj = cson.malloc(objSize);
    CSON_ASSERT(obj, return NULL);

    for (short i = 0; i < modelSize; i++)
    {
        switch (model[i].type)
        {
        case CSON_TYPE_CHAR:
            *(char *)((int)obj + model[i].offset) = (char)csonDecodeNumber(json, model[i].key);
            break;
        case CSON_TYPE_SHORT:
            *(short *)((int)obj + model[i].offset) = (short)csonDecodeNumber(json, model[i].key);
            break;
        case CSON_TYPE_INT:
            *(int *)((int)obj + model[i].offset) = (int)csonDecodeNumber(json, model[i].key);
            break;
        case CSON_TYPE_LONG:
            *(long *)((int)obj + model[i].offset) = (long)csonDecodeNumber(json, model[i].key);
            break;
        case CSON_TYPE_FLOAT:
            *(float *)((int)obj + model[i].offset) = (float)csonDecodeDouble(json, model[i].key);
            break;
        case CSON_TYPE_DOUBLE:
            *(double *)((int)obj + model[i].offset) = csonDecodeDouble(json, model[i].key);
            break;
        case CSON_TYPE_BOOL:
            *(char *)((int)obj + model[i].offset) = (char)csonDecodeNumber(json, model[i].key);
            break;
        case CSON_TYPE_STRING:
            *(int *)((int)obj + model[i].offset) = (int)csonDecodeString(json, model[i].key);
            break;
        case CSON_TYPE_LIST:
            *(int *)((int)obj + model[i].offset) = (int)csonDecodeList(json,
            model[i].key, model[i].param.sub.model, model[i].param.sub.size);
            break;
        case CSON_TYPE_STRUCT:
            *(int *)((int)obj + model[i].offset) = (int)csonDecodeObject(
                cJSON_GetObjectItem(json, model[i].key), 
                model[i].param.sub.model, model[i].param.sub.size);
            break;
        case CSON_TYPE_ARRAY:
            csonDecodeArray(json, model[i].key, (void *)((int)obj + model[i].offset), 
                model[i].param.array.eleType, model[i].param.array.size);
            break;
        case CSON_TYPE_JSON:
            *(int *)((int)obj + model[i].offset) = (int)cJSON_PrintUnformatted(
                cJSON_GetObjectItem(json, model[i].key));
            break;
        default:
            break;
        }
    }
    return obj;
}


/**
 * @brief 解析JSON字符串
 * 
 * @param jsonStr json字符串
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return void* 解析得到的对象
 */
void *csonDecode(const char *jsonStr, CsonModel *model, int modelSize)
{
    void *obj;
    cJSON *json = cJSON_Parse(jsonStr);
    CSON_ASSERT(json, return NULL);
    obj = csonDecodeObject(json, model, modelSize);
    cJSON_Delete(json);
    return obj;
}



/**
 * @brief 数字编码编码JSON
 * 
 * @param json json对象
 * @param key key
 * @param num 数值
 */
void csonEncodeNumber(cJSON *json, char *key, double num)
{
    if (key)
    {
        cJSON_AddNumberToObject(json, key, num);
    }
    else
    {
        json->type = cJSON_Number;
        json->valuedouble = num;
        json->valueint = (int)num;
    }
}


/**
 * @brief 字符串编码编码JSON
 * 
 * @param json json对象
 * @param key key
 * @param str 字符串
 */
void csonEncodeString(cJSON *json, char *key, char *str)
{
    if (key)
    {
        cJSON_AddStringToObject(json, key, str);
    }
    else
    {
        json->type = cJSON_String;
        json->valuestring = str;
    }
}


/**
 * @brief CsonList编码成JSON对象
 * 
 * @param list CsonList对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return cJSON* 编码得到的JOSN对象
 */
cJSON* csonEncodeList(CsonList *list, CsonModel *model, int modelSize)
{
    cJSON *root = cJSON_CreateArray();
    cJSON *item;
    CsonList *p = list;

    while (p)
    {
        if (p->obj)
        {
            item = csonEncodeObject(p->obj, model, modelSize);
            cJSON_AddItemToArray(root, item);
        }
        p = p->next;
    }
    return root;
}


/**
 * @brief 数组编码成JSON对象
 * 
 * @param base 数组基址
 * @param elementType 数组元素类型
 * @param arraySize 数组大小
 * @return cJSON* 编码得到的JOSN对象
 */
cJSON* csonEncodeArray(void *base, CsonType elementType, short arraySize)
{
    cJSON *root = cJSON_CreateArray();
    cJSON *item;

    for (short i = 0; i < arraySize; i++)
    {
        switch (elementType)
        {
        case CSON_TYPE_CHAR:
            item = cJSON_CreateNumber(*(char *)((int)base + (i * sizeof(char))));
            break;
        case CSON_TYPE_SHORT:
            item = cJSON_CreateNumber(*(short *)((int)base + (i * sizeof(short))));
            break;
        case CSON_TYPE_INT:
            item = cJSON_CreateNumber(*(int *)((int)base + (i * sizeof(int))));
            break;
        case CSON_TYPE_LONG:
            item = cJSON_CreateNumber(*(long *)((int)base + (i * sizeof(int))));
            break;
        case CSON_TYPE_FLOAT:
            item = cJSON_CreateNumber(*(float *)((int)base + (i * sizeof(float))));
            break;
        case CSON_TYPE_DOUBLE:
            item = cJSON_CreateNumber(*(double *)((int)base + (i * sizeof(double))));
            break;
        case CSON_TYPE_STRING:
            item = cJSON_CreateString((char *)*(int *)((int)base + (i * sizeof(int))));
            break;
        default:
            break;
        }
        cJSON_AddItemToArray(root, item);
    }
    return root;
}


/**
 * @brief 编码JSON对象
 * 
 * @param obj 对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return cJSON* 编码得到的json对象
 */
cJSON* csonEncodeObject(void *obj, CsonModel *model, int modelSize)
{
    if (!obj) {
        return cJSON_CreateNull();
    }
    cJSON *root = cJSON_CreateObject();

    for (short i = 0; i < modelSize; i++)
    {
        switch (model[i].type)
        {
        case CSON_TYPE_CHAR:
            csonEncodeNumber(root, model[i].key, *(char *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_SHORT:
            csonEncodeNumber(root, model[i].key, *(short *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_INT:
            csonEncodeNumber(root, model[i].key, *(int *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_LONG:
            csonEncodeNumber(root, model[i].key, *(long *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_FLOAT:
            csonEncodeNumber(root, model[i].key, *(float *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_DOUBLE:
            csonEncodeNumber(root, model[i].key, *(double *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_BOOL:
            cJSON_AddBoolToObject(root, model[i].key, *(char *)((int)obj + model[i].offset));
            break;
        case CSON_TYPE_STRING:
            if ((char *)(*(int *)((int)obj + model[i].offset)))
            {
                csonEncodeString(root, model[i].key, (char *)(*(int *)((int)obj + model[i].offset)));
            }
            break;
        case CSON_TYPE_LIST:
            if ((CsonList *)*(int *)((int)obj + model[i].offset))
            {
                cJSON_AddItemToObject(root, model[i].key, 
                    csonEncodeList((CsonList *)*(int *)((int)obj + model[i].offset),
                        model[i].param.sub.model, model[i].param.sub.size));
            }
            break;
        case CSON_TYPE_STRUCT:
            if ((void *)(*(int *)((int)obj + model[i].offset)))
            {
                cJSON_AddItemToObject(root, model[i].key, csonEncodeObject(
                    (void *)(*(int *)((int)obj + model[i].offset)),
                    model[i].param.sub.model, model[i].param.sub.size));
            }
            break;
        case CSON_TYPE_ARRAY:
            cJSON_AddItemToObject(root, model[i].key, csonEncodeArray(
                (void *)((int)obj + model[i].offset),
                model[i].param.array.eleType, model[i].param.array.size));
            break;
        case CSON_TYPE_JSON:
            if ((char *)(*(int *)((int)obj + model[i].offset)))
            {
                cJSON_AddItemToObject(root, model[i].key, 
                    cJSON_Parse((char *)(*(int *)((int)obj + model[i].offset))));
            }
            break;
        default:
            break;
        }
    }
    return root;
}


/**
 * @brief 编码成json字符串
 * 
 * @param obj 对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @param bufferSize 分配给json字符串的空间大小
 * @param fmt 是否格式化json字符串
 * @return char* 编码得到的json字符串
 */
char* csonEncode(void *obj, CsonModel *model, int modelSize, int bufferSize, int fmt)
{
    cJSON *json = csonEncodeObject(obj, model, modelSize);
    CSON_ASSERT(json, return NULL);
    char *jsonStr = cJSON_PrintBuffered(json, bufferSize, fmt);
    cJSON_Delete(json);
    return jsonStr;
}


/**
 * @brief 编码成json字符串
 * 
 * @param obj 对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return char* 编码得到的json字符串
 */
char* csonEncodeUnformatted(void *obj, CsonModel *model, int modelSize)
{
    cJSON *json = csonEncodeObject(obj, model, modelSize);
    CSON_ASSERT(json, return NULL);
    char *jsonStr = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    return jsonStr;
}


/**
 * @brief 释放CSON解析出的对象
 * 
 * @param obj 对象
 * @param model 对象模型
 * @param modelSize 对象模型数量
 */
void csonFree(void *obj, CsonModel *model, int modelSize)
{
    CsonList *list, *p;

    for (short i = 0; i < modelSize; i++)
    {
        switch ((int)model[i].type)
        {
        case CSON_TYPE_CHAR:
        case CSON_TYPE_SHORT:
        case CSON_TYPE_INT:
        case CSON_TYPE_LONG:
        case CSON_TYPE_FLOAT:
        case CSON_TYPE_DOUBLE:
            break;
        case CSON_TYPE_STRING:
        case CSON_TYPE_JSON:
            cson.free((char *)(*(int *)((int)obj + model[i].offset)));
            break;
        case CSON_TYPE_LIST:
            list = (CsonList *)*(int *)((int)obj + model[i].offset);
            while (list)
            {
                p = list;
                list = list->next;
                if (p->obj)
                {
                    csonFree(p->obj, 
                        model[i].param.sub.model, model[i].param.sub.size);
                }
                cson.free(p);
            }
            break;
        case CSON_TYPE_STRUCT:
            csonFree((void *)(*(int *)((int)obj + model[i].offset)), 
                model[i].param.sub.model, model[i].param.sub.size);
            break;
        case CSON_TYPE_ARRAY:
            if (model[i].param.array.eleType == CSON_TYPE_STRING)
            {
                for (short j = 0; j< model[i].param.array.size; j++)
                {
                    if (*(int *)((int)obj + model[i].offset + (j << 2)))
                    {
                        cson.free((void *)*(int *)((int)obj + model[i].offset + (j << 2)));
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    cson.free(obj);
}


/**
 * @brief 释放cson编码生成的json字符串
 * 
 * @param jsonStr json字符串
 */
void csonFreeJson(const char *jsonStr)
{
    CSON_ASSERT(jsonStr, return);
    cson.free((void *)jsonStr);
}


/**
 * @brief CSON链表添加节点
 * 
 * @param list 链表
 * @param obj 节点对象
 * @return CsonList 链表
 */
CsonList* csonListAdd(CsonList *list, void *obj)
{
    if (!list)
    {
        list = cson.malloc(sizeof(CsonList));
        if (!list)
        {
            return NULL;
        }
        list->next = NULL;
        list->obj = NULL;
    }
    CsonList *p = list;
    while (p->next)
    {
        p = p->next;
    }
    if (!p->obj)
    {
        p->obj = obj;
        p->next = NULL;
    }
    else
    {
        CsonList *node = cson.malloc(sizeof(CsonList));
        if (node)
        {
            node->obj = obj;
            node->next = NULL;
            p->next = node;
        }
    }
    return list;
}


/**
 * @brief CSON链表删除节点
 * 
 * @param list 链表
 * @param obj 节点对象
 * @param freeMem 释放内存
 * @return CsonList 链表
 */
CsonList *csonListDelete(CsonList *list, void *obj, char freeMem)
{
    CSON_ASSERT(list, return NULL);

    CsonList head = {0};
    head.next = list;
    CsonList *p = &head;
    CsonList *tmp;
    while (p->next)
    {
        if (p->next->obj && p->next->obj == obj)
        {
            tmp = p->next;
            p->next = p->next->next ? p->next->next : NULL;
            if (freeMem)
            {
                cson.free(tmp->obj);
                cson.free(tmp);
            }
            break;
        }
        p = p->next;
    }
    return head.next;
}


/**
 * @brief CSON新字符串
 * 
 * @param src 源字符串
 * @return char* 新字符串
 * @note 此函数用于复制字符串，建议对结构体中字符串成员赋值时，使用此函数，
 *       方便使用`csonFree`进行内存释放
 */
char* csonNewString(const char *src)
{
    int len = strlen(src);
    char *dest = cson.malloc(len + 1);
    strcpy(dest, src);
    return dest;
}
