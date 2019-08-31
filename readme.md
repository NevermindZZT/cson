# CSON

![version](https://img.shields.io/badge/version-1.0.0-brightgreen.svg)
![build](https://img.shields.io/badge/build-2019.8.27-brightgreen.svg)

基于[cJSON](https://github.com/kbranigan/cJSON),运行于C语言平台的json-struct模型解析工具

- [CSON](#cson)
  - [简介](#简介)
  - [使用](#使用)
    - [声明结构体](#声明结构体)
    - [定义数据模型](#定义数据模型)
    - [使用CSON解析](#使用cson解析)
  - [结构体数据类型](#结构体数据类型)
  - [数据模型映射](#数据模型映射)
  - [API](#api)
    - [初始化](#初始化)
    - [反序列化](#反序列化)
    - [序列化](#序列化)
    - [空间释放](#空间释放)
      - [释放结构体对象](#释放结构体对象)
      - [释放json字符串](#释放json字符串)
  - [注意](#注意)

## 简介

[CSON](https://github.com/NevermindZZT/cson)是一个简单的cJSON的二次封装，相比于使用原生cJSON一层一层解析的方式，CSON采用模型映射的方式，使用模型将结构体的特征进行描述，然后根据模型，将json数据直接解析成结构体，免去使用原生cJSON需要多次调用API的复杂性，可以很大程度减少代码冗余，增加代码逻辑性。

CSON的模型映射借鉴了高级语言(比如说Java)的反射机制，通过建立数据模型，记录结构体的元素，类型，偏移，然后直接在内存层面进行操作，对C语言提供类似于[gson](https://github.com/google/gson)这样的高效json解析工具

## 使用

CSON通过数据模型将结构体和json建立映射关系，因此，你需要做的就是在声明结构体的时候同时，使用数据模型对结构体进行描述，之后，只需要直接调用CSON的api即可

### 声明结构体

```C
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
```

### 定义数据模型

对每一个需要使用cson的结构体，都需要定义相对应的数据模型

```C
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
```

### 使用CSON解析

只需要定义好数据模型，就可以使用CSON读json进行序列化和反序列化

```C
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
```

运行结果：

```plain
hub: id: 1, user: Letter, project id: 2, project name: cson
format json: {
        "id":   1,
        "user": "Letter",
        "cson": {
                "id":   2,
                "name": "cson"
        }
}
```

可以看到，无论是解析json，还是序列化结构体到json，在使用CSON的情况下，都只需要一行代码就可以解决，同样的操作，在使用原生cJSON的情况下，你可能需要多次判断，解析元素

## 结构体数据类型

CSON采用数据模型对结构体进行解析，在方便json操作的同时，也给结构体的定义带来了一些限制，目前，CSON所支持在结构体中定义的数据类型包括：

1. 整数(char, short, int, long)
2. 浮点数(float, double)
3. 字符串(char *)
4. 基本类型数组(char[], short[], int[], long[], float[], double[], *char[])
5. 子结构体(指针形式)
6. 链表(CsonList)

其中，为了方便解析，CSON定义了一个专用的链表(CsonList)，用于对json中复杂结构的数组映射

CSON支持的数据类型基本包括绝大多数使用场景，对于一些之前就定义好的结构体，可能需要稍微做一点修改

## 数据模型映射

CSON采用数据模型建立结构体同json之间的映射，数据模型通过结构体数组进行定义，数据模型定义如下：

```C
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
```

对于每一个需要使用CSON的结构体，都需要定义一个数据模型，每一个数据模型都需要包含一条结构体描述`CSON_MODEL_OBJ(type)`以及若干个数据描述，取决于结构体的成员数量

一般情况下，你只需要使用CSON提供的宏进行数据模型条目的定义，数据模型宏与对应的数据类型对应如下：

| 数据模型宏                                         | 数据类型 | 备注                                               |
| -------------------------------------------------- | -------- | -------------------------------------------------- |
| CSON_MODEL_OBJ(type)                               | 结构体   | 用于描述整个结构体，每一个数据模型都需要包含此条目 |
| CSON_MODEL_CHAR(type, key)                         | char     |                                                    |
| CSON_MODEL_SHORT(type, key)                        | short    |                                                    |
| CSON_MODEL_INT(type, key)                          | int      |                                                    |
| CSON_MODEL_LONG(type, key)                         | long     |                                                    |
| CSON_MODEL_FLOAT(type, key)                        | float    |                                                    |
| CSON_MODEL_DOUBLE(type, key)                       | double   |                                                    |
| CSON_MODEL_BOOL(type, key)                         | bool     | C没有bool,对应为char                               |
| CSON_MODEL_STRING(type, key)                       | char *   |                                                    |
| CSON_MODEL_STRUCT(type, key, submodel, subsize)    | 子结构体 | 子结构体必须是结构体指针的形式                     |
| CSON_MODEL_LIST(type, key, submodel, subsize)      | CsonList | CSON定义的链表                                     |
| CSON_MODEL_ARRAY(type, key, elementType, arraySize | 数组     | 支持基本数据类型, 数组的每一个元素必须合法         |

## API

CSON源文件有完整的注释，可以通过Doxygen等工具导出完整的API文档，以下是几个关键API的说明

### 初始化

初始化CSON，提供内存分配和内存释放函数，对于标准C库可以使用`malloc`和`free`

```C
void csonInit(void *malloc, void *free)
```

- 参数
  - `malloc` 内存分配函数
  - `free` 内存释放函数

### 反序列化

解析json，将json字符串反序列化成结构体对象

```C
void *csonDecode(const char *jsonStr, CsonModel *model, int modelSize)
```

- 参数
  - `jsonStr` json字符串
  - `model` 描述目标结构体的数据模型
  - `modelSize` 数据模型大小
- 返回
  - `void *` 反序列化得到的结构体对象

### 序列化

编码结构体，将结构体对象序列化成json字符串

```C
char* csonEncode(void *obj, CsonModel *model, int modelSize, int bufferSize, int fmt)
```

- 参数
  - `obj` 源结构体对象
  - `model` 描述源结构体的数据模型
  - `modelSize` 数据模型大小
  - `bufferSize` 可分配给json字符串的空间大小，需要根据内容估计大小
  - `fmt` 是否格式化json字符串
- 返回
  - `char *` 序列化得到的json字符串

### 空间释放

CSON提供了两个释放内存的函数，用于释放CSON生成的结构体对象和json字符串

#### 释放结构体对象

```C
void csonFree(void *obj, CsonModel *model, int modelSize)
```

- 参数
  - `obj` 待释放的结构体对象
  - `model` 待释放的结构体数据模型
  - `modelSize` 待释放的结构体数据模型大小

#### 释放json字符串

```C
void csonFreeJson(const char *jsonStr)
```

- 参数
  - `jsonStr` 待释放的json字符串

## 注意

- 数据模型根据结构体不同而不同，数据模型的数量=结构体成员数量+1，多出来的一条是定义结构体`CSON_MODEL_OBJ(type)`
- 数组类型映射时会处理给进去的数组大小，所以请确保每一个数组元素都是合法的
- 基本数据类型链表采用类似子结构体的方式，CSON默认定义了基本数据类型链表元素的数据模型，通过类似`CSON_MODEL_LIST(struct test, strList, CSON_MODEL_STRING_LIST, CSON_BASIC_LIST_MODEL_SIZE)`进行定义即可
