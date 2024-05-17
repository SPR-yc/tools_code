#include "qibufpool.h"

//内存块信息结构体
typedef struct{
    QI_VOID *addr;   //本blk管理的内存起始地址
    QI_U32 index;    //本blk在内存池中的编号
    QI_BOOL usedFlg; //本blk内存是否被使用标志位
}pool_blk_st;


//内存池信息结构体
typedef struct pool_info_st{
    QI_VOID *pool;    //内存池起始地址
    QI_U32 poolSize;  //内存池总大小
    QI_U32 blkSize;   //每个内存块大小
    QI_S32 blkNum;    //内存块总个数
    pthread_mutex_t pool_mutex;
    pool_blk_st *blkArr;  //本内存池中管理blk的数组指针
    struct pool_info_st *next;  //内存池指针 多内存池链表
}pool_info_st;

pool_info_st *g_poolInfo_p = QI_NULL;  //保存内存池信息


//根据内存池地址，在内存池链表中获取内存池信息结构体
static pool_info_st *findGoalPool(QI_VOID *pool)
{
    if(pool == NULL)
    {
        return NULL;
    }

    pool_info_st *pool_cur = g_poolInfo_p;
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
        printf("not found pool matched [%p]!\n", pool);
        return NULL;
    }

    return pool_cur;
}

QI_S32 QIMemPoolInit(QI_VOID *pool, QI_U32 poolSize, QI_U32 blkSize)
{
    QI_S32 ret = -1;

    //申请内存池信息结构体 并初始化
    pool_info_st *tmp_pool_ptr = NULL;
    tmp_pool_ptr = (pool_info_st *)malloc(sizeof(pool_info_st));
    if(tmp_pool_ptr == NULL)
    {
        printf("%s  line:%d  malloc failed!\n", __FUNCTION__, __LINE__);
        return -1;
    }
    memset(tmp_pool_ptr, 0, sizeof(pool_info_st));
    tmp_pool_ptr->pool = pool;
    tmp_pool_ptr->poolSize = poolSize;
    tmp_pool_ptr->blkSize = blkSize;
    tmp_pool_ptr->blkNum = poolSize / blkSize;
    pthread_mutex_init(&tmp_pool_ptr->pool_mutex, NULL);
    tmp_pool_ptr->next = NULL;

    //申请内存块管理数组，并初始化每个blk信息
    tmp_pool_ptr->blkArr = (pool_blk_st *)malloc(tmp_pool_ptr->blkNum * sizeof(pool_blk_st));
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


    // 将新增内存池加入内存池链表
    if(g_poolInfo_p == NULL)
    {
        g_poolInfo_p = tmp_pool_ptr;  //第一次初始化内存池
    }else{ //多个内存池之间，链接起来
        pool_info_st *pool_next = g_poolInfo_p->next;
        while(pool_next != NULL)
        {
            pool_next = pool_next->next;
        }

        pool_next = tmp_pool_ptr;
    }

    return 0;

err:
    pthread_mutex_destroy(&tmp_pool_ptr->pool_mutex);
    free(tmp_pool_ptr);

    return -1;
}

//从 g_poolInfo_p 遍历内存池，找到指定的内存池pool，将它反初始化
QI_S32 QIMemPoolDeInit(QI_VOID *pool)
{
    QI_S32 ret = -1;

    if(pool == NULL || g_poolInfo_p == NULL)
    {
        printf("%s  line:%d  pool list is null or para invalid!\n", __FUNCTION__, __LINE__);
        //QLogE("bufpool", "pool is null or para invalid!\n");
        return -1;
    }

    pool_info_st *pool_cur = g_poolInfo_p; //遍历内存池链表，找到目标
    pool_info_st *pool_pre = g_poolInfo_p; 
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

    free(pool_cur->blkArr); //注销本内存池中blk管理数组
    pthread_mutex_destroy(&pool_cur->pool_mutex);  //注销本内存池的锁
    
    pool_pre->next = pool_cur->next; //将本内存池从链表删除
    free(pool_cur);  //释放内存池信息结构体
    pool_cur = NULL;

    return 0;
}


QI_VOID *QIMemPoolAlloc(QI_VOID *pool, QI_U32 memSize)
{
    if(pool == NULL || memSize <= 0 )
    {
        return NULL;
    }

    pool_info_st *pool_cur = findGoalPool(pool);
    if(pool_cur == NULL)
    {
        return NULL;
    }

    //申请空间 根据指定的申请大小和内存块大小计算得出需要的内存块数量，遍历内存块管理数组，找到没有被使用的空闲块开始计数，若计数满，则返回第一个内存块地址 并设置所有内存块的使用标记
    int blkNum_need = 0;
    if(memSize % pool_cur->blkSize != 0)  //考虑传入长度不是内存块长度整数倍的情况
    {
        blkNum_need = memSize / pool_cur->blkSize + 1;  //向下取整
    }else{
        blkNum_need = memSize / pool_cur->blkSize;
    }
    printf("%s, alloc size[%u], blkNum_need = %d\n", __FUNCTION__, memSize, blkNum_need);

    pthread_mutex_lock(&pool_cur->pool_mutex);
    int i = 0, j = 0;
    for(i = 0; i < pool_cur->blkNum; i++)
    {
        if(pool_cur->blkArr[i].usedFlg == QI_FALSE && (pool_cur->blkNum - i) >= blkNum_need)
        {
            j  = 0;  
            while(pool_cur->blkArr[i+j].usedFlg == QI_FALSE)
            {
                
                j++;
                if(j == blkNum_need)
                {
                    break;
                }
            }
        }else{
            printf("pool[%p] have not enough space to malloc!\n", pool);
            pthread_mutex_unlock(&pool_cur->pool_mutex);
            return NULL;
        }

        if(j == blkNum_need) //设置usedflg
        {
            int p = 0;
            for(p; p < j; p++)
            {
                pool_cur->blkArr[i+p].usedFlg = QI_TRUE;
            }
            break;
        }else{
            printf("pool[%p] have not enough space to malloc!\n", pool);
            pthread_mutex_unlock(&pool_cur->pool_mutex);
            return NULL;
        }
    }
    pthread_mutex_unlock(&pool_cur->pool_mutex);

    return pool_cur->blkArr[i].addr;  //返回第i个blk地址
}


QI_S32 QIMemPoolClr(QI_VOID *pool, QI_VOID* addrPtr, QI_U32 memSize)
{
    if(pool == NULL || addrPtr == NULL || g_poolInfo_p == NULL)
    {
        return -1;
    }

    pool_info_st *pool_cur = findGoalPool(pool);
    if(pool_cur == NULL)
    {
        return -1;
    }

    //确认指定地址是在本内存池内
    pthread_mutex_lock(&pool_cur->pool_mutex);
    int blkNum_need = 0;
    if(memSize % pool_cur->blkSize != 0)  //考虑传入内存长度不是内存块长度的整数倍情况
    {
        blkNum_need = memSize / pool_cur->blkSize + 1;  //向下取整
    }else{
        blkNum_need = memSize / pool_cur->blkSize;
    }
    printf("%s: clear size [%d], blkNum_need = %d \n", __FUNCTION__, memSize, blkNum_need);
     
    int i = 0;
    for(i; i < pool_cur->blkNum; i++)
    {
        if(pool_cur->blkArr[i].addr == addrPtr)
        {
            if(pool_cur->blkNum - i >= blkNum_need) //确保清空的长度没有超过内存池管理范围
            {
                memset(addrPtr, 0, memSize);
                pthread_mutex_unlock(&pool_cur->pool_mutex);
                return 0;
            }else{
                printf("clear length is not in the range of pool!\n");
                goto err;
            }
        }
    }

    if(i == pool_cur->blkNum - 1)
    {
        printf("the clear addr[%p] is not in pool[%p]!\n", addrPtr, pool);
        goto err;
    }

err:
    pthread_mutex_unlock(&pool_cur->pool_mutex);
    return -1;
}


//找到对应的内存池，在内存池找到要销毁空间的起始位置，根据memSize和blkSize计算出需要销毁的内存块个数，循环设置blk的usedFlg = QI_FALSE
QI_S32 QIMemPoolFree(QI_VOID *pool, QI_VOID *addrPtr, QI_U32 memSize)
{
    int ret = -1;
    if(pool == NULL || addrPtr == NULL || memSize == 0)
    {
        return -1;
    }

    pool_info_st *pool_cur = findGoalPool(pool); 
    if(pool_cur == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    QI_U16 blkNum_need = memSize / pool_cur->blkSize + 1;   //计算内存块数量
    int i = 0, j = 0;
    for(i; i < pool_cur->blkNum; i++)  
    {
        if(pool_cur->blkArr[i].addr == addrPtr) //定位内存块
        {
            for(j; j < blkNum_need; j++) //设置usedFlg
            {
                pool_cur->blkArr[i+j].usedFlg = QI_FALSE;
            }
        }
    }
    pthread_mutex_unlock(&pool_cur->pool_mutex);

    return 0;
}


QI_S32 QIMemPoolGetInfo(QI_VOID *pool)
{
    pool_info_st *pool_cur = findGoalPool(pool);
    if(pool_cur == NULL)
    {
        printf("%s, not find matched pool!\n", __FUNCTION__);
        return -1;
    }

    pthread_mutex_lock(&pool_cur->pool_mutex);
    printf("pool_size:%u, blk_size:%u, blk_num:%u \n", pool_cur->poolSize, pool_cur->blkSize, pool_cur->blkNum);
    pthread_mutex_unlock(&pool_cur->pool_mutex);

    return 0;
}


