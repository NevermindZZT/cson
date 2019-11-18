/**
 * @file cson.h
 * @author Letter (NevermindZZT@gmail.cn)
 * @brief cson
 * @version 0.1
 * @date 2019-08-26
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __CSON_H__
#define __CSON_H__

#include "stddef.h"
#include "cJSON.h"


#define     CSON_VERSION        "1.0.3"         /**< CSON版本 */

/**
 * @defgroup CSON cson
 * @brief json tools for C
 * @addtogroup CSON
 * @{
 */


/**
 * @brief CSON数据类型定义
 * 
 */
typedef enum
{
    CSON_TYPE_OBJ = 0,
    CSON_TYPE_CHAR,
    CSON_TYPE_SHORT,
    CSON_TYPE_INT,
    CSON_TYPE_LONG,
    CSON_TYPE_FLOAT,
    CSON_TYPE_DOUBLE,
    CSON_TYPE_BOOL,
    CSON_TYPE_STRING,
    CSON_TYPE_STRUCT,
    CSON_TYPE_LIST,
    CSON_TYPE_ARRAY,
    CSON_TYPE_JSON,
} CsonType;


/**
 * @brief CSON数据模型定义
 * 
 */
typedef struct cson_model
{
    CsonType type;                      /**< 数据类型 */
    char *key;                          /**< 元素键值 */
    short offset;                       /**< 元素偏移 */
    union
    {
        struct
        {
            struct cson_model *model;   /**< 子结构体模型 */
            short size;                 /**< 子结构体模型大小 */
        } sub;                          /**< 子结构体 */
        struct
        {
            CsonType eleType;           /**< 数组元素类型 */
            short size;                 /**< 数组大小 */
        } array;                        /**< 数组 */
        int objSize;                    /**< 对象大小 */
        CsonType basicListType;         /**< 基础数据链表类型 */
    } param;
} CsonModel;


/**
 * @brief Cson链表
 * 
 */
typedef struct cson_list
{
    struct cson_list *next;             /**< 下一个元素 */
    void *obj;                          /**< 对象 */
} CsonList;


extern CsonModel csonBasicListModel[];  /**< 基础类型链表数据模型 */

#define CSON_MODEL_CHAR_LIST        &csonBasicListModel[0]      /**< char型链表数据模型 */
#define CSON_MODEL_SHORT_LIST       &csonBasicListModel[2]      /**< short型链表数据模型 */
#define CSON_MODEL_INT_LIST         &csonBasicListModel[4]      /**< int型链表数据模型 */
#define CSON_MODEL_LONG_LIST        &csonBasicListModel[6]      /**< long型链表数据模型 */
#define CSON_MODEL_FLOAT_LIST       &csonBasicListModel[8]      /**< float型链表数据模型 */
#define CSON_MODEL_DOUBLE_LIST      &csonBasicListModel[10]     /**< double型链表数据模型 */
#define CSON_MODEL_STRING_LIST      &csonBasicListModel[12]     /**< string型链表数据模型 */

#define CSON_BASIC_LIST_MODEL_SIZE  2                           /**< 基础类型链表数据模型大小 */

/**
 * @brief 对象数据模型
 * 
 * @param type 对象类型
 */
#define CSON_MODEL_OBJ(type) \
        {CSON_TYPE_OBJ, NULL, 0, .param.objSize=sizeof(type)}

/**
 * @brief char型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_CHAR(type, key) \
        {CSON_TYPE_CHAR, #key, offsetof(type, key)}

/**
 * @brief short型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_SHORT(type, key) \
        {CSON_TYPE_SHORT, #key, offsetof(type, key)}

/**
 * @brief int型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_INT(type, key) \
        {CSON_TYPE_INT, #key, offsetof(type, key)}

/**
 * @brief long型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_LONG(type, key) \
        {CSON_TYPE_LONG, #key, offsetof(type, key)}

/**
 * @brief float型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_FLOAT(type, key) \
        {CSON_TYPE_FLOAT, #key, offsetof(type, key)}

/**
 * @brief double型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_DOUBLE(type, key) \
        {CSON_TYPE_DOUBLE, #key, offsetof(type, key)}

/**
 * @brief bool型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_BOOL(type, key) \
        {CSON_TYPE_CHAR, #key, offsetof(type, key)}

/**
 * @brief 字符串型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_STRING(type, key) \
        {CSON_TYPE_STRING, #key, offsetof(type, key)}

/**
 * @brief 结构体型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 * @param submodel 子结构体模型
 * @param subsize 子结构体模型大小
 */
#define CSON_MODEL_STRUCT(type, key, submodel, subsize) \
        {CSON_TYPE_STRUCT, #key, offsetof(type, key), .param.sub.model=submodel, .param.sub.size=subsize}

/**
 * @brief list型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 * @param submodel 子结构体模型
 * @param subsize 子结构体模型大小
 */
#define CSON_MODEL_LIST(type, key, submodel, subsize) \
        {CSON_TYPE_LIST, #key, offsetof(type, key), .param.sub.model=submodel, .param.sub.size=subsize}

/**
 * @brief list型数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 * @param elementType 数组元素类型
 * @param arraySize 数组大小
 */
#define CSON_MODEL_ARRAY(type, key, elementType, arraySize) \
        {CSON_TYPE_ARRAY, #key, offsetof(type, key), .param.array.eleType=elementType, .param.array.size=arraySize}
        
/**
 * @brief 子json数据模型
 * 
 * @param type 对象模型
 * @param key 数据键值
 */
#define CSON_MODEL_JSON(type, key) \
        {CSON_TYPE_JSON, #key, offsetof(type, key)}

/**
 * @brief CSON断言
 * 
 * @param expr 表达式
 * @param action 断言失败执行动作
 */
#define CSON_ASSERT(expr, action) \
        if (!(expr)) { \
            printf(#expr " assert failed at file: %s, line: %d", __FILE__, __LINE__); \
            action; \
        }

/**
 * @brief CSON初始化
 * 
 * @param malloc 内存分配函数
 * @param free 内存释放函数
 */
void csonInit(void *malloc, void *free);

/**
 * @brief 解析JSON对象
 * 
 * @param json JSON对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return void* 解析得到的对象
 */
void *csonDecodeObject(cJSON *json, CsonModel *model, int modelSize);

/**
 * @brief 解析JSON对象
 * 
 * @param json JSON对象
 * @param model 数据模型
 * @return void* 解析得到的对象
 */
#define csonDecodeObjectEx(json, model) \
        csonDecodeObject(json, model, sizeof(model) / sizeof(CsonModel))

/**
 * @brief 解析JSON字符串
 * 
 * @param jsonStr json字符串
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return void* 解析得到的对象
 */
void *csonDecode(const char *jsonStr, CsonModel *model, int modelSize);

/**
 * @brief 解析JSON字符串
 * 
 * @param jsonStr json字符串
 * @param model 数据模型
 * @return void* 解析得到的对象
 */
#define csonDecodeEx(jsonStr, model) \
        csonDecode(jsonStr, model, sizeof(model) / sizeof(CsonModel));

/**
 * @brief 编码成json字符串
 * 
 * @param obj 对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return char* 编码得到的json字符串
 */
cJSON* csonEncodeObject(void *obj, CsonModel *model, int modelSize);

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
char* csonEncode(void *obj, CsonModel *model, int modelSize, int bufferSize, int fmt);

/**
 * @brief 编码成json字符串
 * 
 * @param obj 对象
 * @param model 数据模型
 * @param bufferSize 分配给json字符串的空间大小
 * @param fmt 是否格式化json字符串
 * @return char* 编码得到的json字符串
 */
#define csonEncodeEx(obj, model, bufferSize, fmt) \
        csonEncode(obj, model, sizeof(model)/sizeof(CsonModel), bufferSize, fmt)

/**
 * @brief 编码成json字符串
 * 
 * @param obj 对象
 * @param model 数据模型
 * @param modelSize 数据模型数量
 * @return char* 编码得到的json字符串
 */
char* csonEncodeUnformatted(void *obj, CsonModel *model, int modelSize);

/**
 * @brief 编码成json字符串
 * 
 * @param obj 对象
 * @param model 数据模型
 * @return char* 编码得到的json字符串
 */
#define csonEncodeUnformattedEx(obj, model) \
        csonEncodeUnformatted(obj, model, sizeof(model) / sizeof(CsonModel))

/**
 * @brief 释放CSON解析出的对象
 * 
 * @param obj 对象
 * @param model 对象模型
 * @param modelSize 对象模型数量
 */
void csonFree(void *obj, CsonModel *model, int modelSize);

/**
 * @brief 释放CSON解析出的对象
 * 
 * @param obj 对象
 * @param model 对象模型
 */
#define csonFreeEx(obj, model) \
        csonFree(obj, model, sizeof(model) / sizeof(CsonModel))

/**
 * @brief 释放cson编码生成的json字符串
 * 
 * @param jsonStr json字符串
 */
void csonFreeJson(const char *jsonStr);

/**
 * @brief CSON链表添加节点
 * 
 * @param list 链表
 * @param obj 节点对象
 * @return CsonList 链表
 */
CsonList* csonListAdd(CsonList *list, void *obj);

/**
 * @brief CSON新字符串
 * 
 * @param src 源字符串
 * @return char* 新字符串
 * @note 此函数用于复制字符串，建议对结构体中字符串成员赋值时，使用此函数，
 *       方便使用`csonFree`进行内存释放
 */
char* csonNewString(const char *src);

/**
 * @}
 */

#endif
