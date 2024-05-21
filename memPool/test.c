
#include "qibufpool.h"
#include "qibufpool_static.h"

#define  STATIC_POOL_TEST  0
#define  DYN_POOL_TEST   1

#if STATIC_POOL_TEST
int main(int argc, char* argv[])
{
    QI_S32 ret = 0;


    QI_U32 blkSize = 10;
    QI_U32 blkNum = 100;
    //int static_pool[blkNum * blkSize];
    QI_VOID *static_pool = NULL;
    ret = QIStaMemPoolInit((QI_VOID **)&static_pool, blkNum * blkSize, blkSize);
    if(ret < 0)
    {
        printf("test: init static pool failed!\n");
        return -1;
    }

    QIStaMemGetInfo((QI_VOID *)static_pool);

    int * alloc_test = (int *)QIStaMemAlloc((QI_VOID *)static_pool);
    if(alloc_test == NULL)
    {
        printf("test: malloc static pool failed!\n");
        goto err;
    }
    QIStaMemGetInfo((QI_VOID *)static_pool);

    *alloc_test = 1024;

    printf("test: alloc_test = %d\n", *alloc_test);

    ret = QIStaMemClr((QI_VOID *)static_pool, (QI_VOID *)alloc_test);
    if(ret < 0)
    {
        printf("test: clear static pool mem failed!\n");
        goto err;
    }

    printf("test: after clear --> alloc_test = %d\n", *alloc_test);

    QIStaMemPoolDeInit((QI_VOID *)static_pool);


    return 0;

err:
    QIStaMemPoolDeInit((QI_VOID *)static_pool);

    return -1;
}

#endif


#if DYN_POOL_TEST

int main(int argc, char* argv[])
{
    QI_S32 ret = 0;


    QI_U32 blkSize = 10;
    QI_U32 blkNum = 100;
    //int dyn_pool[blkNum * blkSize];
    QI_VOID *dyn_pool = NULL;
    ret = QIMemPoolInit((QI_VOID **)&dyn_pool, blkNum * blkSize, blkSize);
    if(ret < 0)
    {
        printf("test: init dyn pool failed!\n");
        return -1;
    }

    int *dyn_ptr = (int *)QIMemPoolAlloc((QI_VOID *)dyn_pool, 999);
    if(dyn_ptr == NULL)
    {
        printf("dyn pool test: malloc failed!\n");
        goto err;
    }

    QIMemPoolGetInfo((QI_VOID *)dyn_pool);

    *dyn_ptr = 2048;
    printf("test: dyn_ptr = %d\n", *dyn_ptr);

    ret = QIMemPoolClr((QI_VOID *)dyn_pool, (QI_VOID *)dyn_ptr, 999);
    if(ret < 0)
    {
        printf("test: mem clear failed!\n");
        goto err;
    }
    printf("test: after clear   dyn_ptr = %d\n", *dyn_ptr);

    QIMemPoolGetInfo((QI_VOID *)dyn_pool);

    ret = QIMemPoolDeInit((QI_VOID *)dyn_pool);
    if(ret < 0)
    {
        printf("test:mem deinit failed!\n");
        return -1;
    }

    

    return 0;

err:
    QIMemPoolDeInit((QI_VOID *)dyn_pool);
    return -1;
}

#endif