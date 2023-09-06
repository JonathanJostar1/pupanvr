#ifndef __FY_MPI_FB_H__
#define __FY_MPI_FB_H__


int     fy_mpi_fb_init();
void    fy_mpi_fb_exit();
int     fy_mpi_fb_get_framebufferInfo(char **fbp, void* pStVinfo, void* pStFixinfo);

#endif