#include "fy_mpi_sys.h"
#include "type_def.h"
#include "sample_comm.h"
#include "fy_mpi_def.h"


int     fy_mpi_sys_init()
{
     int result = 0;

    VB_CONF_S stVbConf;
    //SIZE_S stSize;
    FY_U32 u32BlkSize = 0;

    memset(&stVbConf, 0, sizeof(VB_CONF_S));
    stVbConf.u32MaxPoolCnt = 1;

    u32BlkSize = 1920 * 1440 * 3/2;
    stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt = 4;

    result = SAMPLE_COMM_SYS_Init(&stVbConf);

    if(result != FY_SUCCESS)
    {
        FY_MPI_PRT("sys init failure!\n");
        return -1;   
    }

    return 0;
}

void    fy_mpi_sys_exit()
{
    SAMPLE_COMM_SYS_Exit();
}