#include "qibufpool_static.h"

typedef struct{
    QI_VOID *addr;   //本blk管理的内存起始地址
    QI_U32 index;    //本blk在内存池中的编号
    QI_BOOL usedFlg; //本blk内存是否被使用标志位
}sta_pool_blk_st;

typedef struct sta_pool_info{
    QI_VOID *pool;    //内存池起始地址
    QI_U32 poolSize;  //内存池总大小
    QI_U32 blkSize;   //每个内存块大小
    QI_U32 blkNum;    //内存块总个数
    QI_U32 blkUsed;   //已使用的内存块个数
    pthread_mutex_t pool_mutex;
    sta_pool_blk_st *blkArr;  //本内存池中管理blk的数组指针
    struct sta_pool_info *next;  //内存池指针 多内存池链表
}sta_pool_info_st;

sta_pool_info_st *g_sta_pool_info_p = NULL;


static sta_pool_info_st *findStaPool(QI_VOID *pool)
{
    sta_pool_info_st *pool_cur = g_sta_pool_info_p;
    while(pool_cur != NULL)
    {
        if(pool_cur->pool == pool)
        {
            break;
        }
        pool_cur = pool_cur->next;
    }

    if(pool_cur == NULL)
    {
        printf("%s, not find matched pool!\n", __FUNCTION__);
        return NULL;
    }

    return pool_cur;
}

QI_S32 QIStaMemPoolInit(QI_VOID *pool, QI_U32 poolSize, QI_U32 blkSize)
{
    QI_S32 ret = -1;

    //申请内存池信息结构体 并初始化
    sta_pool_info_st *tmp_pool_ptr = NULL;
    tmp_pool_ptr = (sta_pool_info_st *)malloc(sizeof(sta_pool_info_st));
    if(tmp_pool_ptr == NULL)
    {
        printf("%s  line:%d  malloc failed!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    memset(tmp_pool_ptr, 0, sizeof(sta_pool_info_st));
    tmp_pool_ptr->pool = pool;
    tmp_pool_ptr->poolSize = poolSize;
    tmp_pool_ptr->blkSize = blkSize;
    tmp_pool_ptr->blkNum = poolSize / blkSize;
    tmp_pool_ptr->blkUsed = 0;
    pthread_mutex_init(&tmp_pool_ptr->pool_mutex, NULL);
    tmp_pool_ptr->next = NULL;

    //申请内存块管理数组，并初始化每个blk信息
    tmp_pool_ptr->blkArr = (sta_pool_blk_st *)malloc(tmp_pool_ptr->blkNum * sizeof(sta_pool_blk_st));
    if(tmp_pool_ptr->blkArr == NULL)
    {
        printf("%s  line:%d  malloc failed!\n", __FUNCTION__, __LINE__);
        goto err;
    }

    int i = 0;
    QI_VOID *tmp_ptr = pool;
    for(i = 0; i < tmp_pool_ptr->blkNum; i++) //循环初始化本内存池各blk信息
    {
        tmp_ptr += i * blkSize;
        tmp_pool_ptr->blkArr[i].addr = tmp_ptr;
        tmp_pool_ptr->blkArr[i].index = i;
        tmp_pool_ptr->blkArr[i].usedFlg = QI_FALSE;
    }

    if(g_sta_pool_info_p == NULL)
    {
        g_sta_pool_info_p = tmp_pool_ptr;
    }else{
        sta_pool_info_st* tmpPtr = g_sta_pool_info_p->next;
        while(tmpPtr != NULL)
        {
            tmpPtr = tmpPtr->next;
        }
        tmpPtr = tmp_pool_ptr;
    }

    return 0;

err:
    pthread_mutex_destroy(&tmp_pool_ptr->pool_mutex);
    free(tmp_pool_ptr);

    return -1;
}


QI_S32 QIStaMemPoolDeInit(QI_VOID *pool)
{
    int ret = -1;

    if(pool == NULL || g_sta_pool_info_p == NULL)
    {
        printf("%s  line:%d  pool list is null or para invalid!\n", __FUNCTION__, __LINE__);
        //QLogE("bufpool", "pool is null or para invalid!\n");
        return -1;
    }

    sta_pool_info_st *pool_cur = g_sta_pool_info_p; //遍历内存池链表，找到目标
    sta_pool_info_st *pool_pre = g_sta_pool_info_p; 
    while(pool_cur != NULL)
    {
        if(pool_cur->pool == pool)
        {
            break;
        }

        pool_pre = pool_cur;
        pool_cur = pool_cur->next;
    }

    if(pool_cur == NULL)
    {
        printf("not found pool matched [%p]!\n", pool);
        return -1;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    free(pool_cur->blkArr); //注销本内存池中blk管理数组
    pthread_mutex_unlock(&pool_cur->pool_mutex);
    pthread_mutex_destroy(&pool_cur->pool_mutex);  //注销本内存池的锁
    pool_pre->next = pool_cur->next; //将本内存池从链表删除
    free(pool_cur);  //释放内存池信息结构体
    pool_cur = NULL;

    return 0;
}

//静态内存池每次申请一个固定的内存块大小
QI_VOID *QIStaMemAlloc(QI_VOID *pool)
{
    if(pool == NULL || g_sta_pool_info_p == NULL)
    {
        return NULL;
    }


    sta_pool_info_st *pool_cur = findStaPool(pool);
    if(pool_cur == NULL)
    {
        return NULL;
    }

    printf("malloc find pool: pool_size:%u, blk_size:%u, blk_num:%u \n", pool_cur->poolSize, pool_cur->blkSize, pool_cur->blkNum);

    if(pool_cur->blkUsed == pool_cur->blkNum)
    {
        printf("no space to malloc! \n");
        return NULL;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    int i = 0;
    for(i; i < pool_cur->blkNum; i++)
    {
        if(pool_cur->blkArr[i].usedFlg == QI_FALSE)
        {
            pool_cur->blkArr[i].usedFlg = QI_TRUE;
            pool_cur->blkUsed++;
            break;
        }
    }
    pthread_mutex_unlock(&pool_cur->pool_mutex);

    return pool_cur->blkArr[i].addr;
}

QI_S32 QIStaMemClr(QI_VOID *pool, QI_VOID *addrPtr)
{
    if(pool == NULL || addrPtr == NULL || g_sta_pool_info_p == NULL)
    {
        return -1;
    }

    sta_pool_info_st *pool_cur = findStaPool(pool);
    if(pool_cur == NULL)
    {
        printf("%s: not find matched pool!\n", __FUNCTION__);
        return -1;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    int i = 0;
    for(i; i < pool_cur->blkNum; i++)
    {
        if(pool_cur->blkArr[i].addr == addrPtr)  //验证目标地址在指定pool中
        {
            memset(addrPtr, 0, pool_cur->blkSize);  //清空当前blk内存
            pthread_mutex_unlock(&pool_cur->pool_mutex);
            return 0;
        }
    }

    if(i == pool_cur->blkNum - 1)
    {
        printf("clear addr[%p] is not in pool[%p]!\n", addrPtr, pool);
        pthread_mutex_unlock(&pool_cur->pool_mutex);
        return -1;
    }
    
}

//静态内存池，每次注销一个块的空间
QI_S32 QIStaMemFree(QI_VOID *pool, QI_VOID *addrPtr)
{
    if(pool == NULL || addrPtr == NULL)
    {
        printf("%s line:%d   err!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    sta_pool_info_st *pool_cur = findStaPool(pool);
    if(pool_cur == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    int i = 0;
    for(i; i < pool_cur->blkNum; i++)
    {
        if(pool_cur->blkArr[i].addr == addrPtr)
        {
            pool_cur->blkArr[i].usedFlg = QI_FALSE;
            pool_cur->blkUsed--;
            break;
        }
    }
    pthread_mutex_unlock(&pool_cur->pool_mutex);

    return 0;
}

QI_S32 QIStaMemGetInfo(QI_VOID *pool)
{
    if(pool == NULL || g_sta_pool_info_p == NULL)
    {
        return -1;
    }

    sta_pool_info_st *pool_cur = findStaPool(pool);
    if(pool_cur == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    printf("pool_size:%u, blk_size:%u, blk_num:%u, blk_used:%u \n", pool_cur->poolSize, pool_cur->blkSize, pool_cur->blkNum, pool_cur->blkUsed);
    pthread_mutex_unlock(&pool_cur->pool_mutex);

    return 0;
}
