#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <linux/time.h>
#include "osd.h"
//#include "libvmm.h"
#include "gosd.h"

//#include "mpi_vpss.h"
#include "sample_comm.h"

typedef struct mem_desc {
	unsigned int base;
	void        *vbase;
	unsigned int size;
	unsigned int align;
} MEM_DESC;

#define LOGOV2_PRT
#ifdef LOGOV2_PRT
#define LOGOV2_DBG_PRT(...) \
do{ \
		printf("%s%s:%d ","[logov2-osd]",__func__, __LINE__); \
		printf(__VA_ARGS__); \
}while(0)
#else
#define LOGOV2_DBG_PRT(...)
#endif


#define DEFALT_ALIGN                         (8)


#define ERR_RETURN(name, ret_val) \
	do{\
	int _ret = ret_val;\
	if (_ret != FY_SUCCESS) \
	{\
		fprintf(stderr, name" failed (%d)\n", _ret);\
		return _ret;\
	}\
	}while(0)

typedef enum
{
	LOGOV2_PIXEL_1BIT = (1 << 0),
	LOGOV2_PIXEL_2BIT = (1 << 1),
	LOGOV2_PIXEL_4BIT = (1 << 3),
	LOGOV2_PIXEL_8BIT = (1 << 7),
	LOGOV2_PIXEL_16BIT = (1 << 15),
	LOGOV2_PIXEL_32BIT = (1 << 31),
}LOGOV2_PIXEL_DEPTH;

typedef struct
{
	FH_UINT16           frame_count; /**< 帧数 */
	FH_UINT16           frame_time; /**< 统计时间 (s)*/
} FH_FRAMERATE;

#define LOGOV2_MAGIC (201811)
#define LOGOV2_SUPPORT_TYPE (LOGOV2_PIXEL_1BIT|LOGOV2_PIXEL_2BIT|LOGOV2_PIXEL_4BIT|LOGOV2_PIXEL_8BIT|LOGOV2_PIXEL_16BIT|LOGOV2_PIXEL_32BIT)
#define ALIGNTO(addr, edge)  ((addr + edge - 1) & ~(edge - 1))

#define MAX(a,b)             (a > b ? a : b)
#define MIN(a,b)             (a < b ? a : b)
#define CLIP(data,min,max)   (data >= min ? (data <= max ? data : max) : min)

#define OSD_TYPE_ASCII  (1)
#define OSD_TYPE_GB2312 (2)

#define NameToStr(name)  (#name)
static char g_vi_type[16][32];


typedef struct
{
	unsigned int grp_idx;
	unsigned int chan;
	unsigned int isinit;
	unsigned int idx;
	unsigned int w;
	unsigned int h;
	unsigned int stride;
	unsigned int pixdepth;
	MEM_DESC logo_buf;
}LOGOV2_INFO;

#define CHECK_LOGOV2_ISINIT(osd_cfg) \
do{ \
	if(osd_cfg->isinit != LOGOV2_MAGIC) { \
		LOGOV2_DBG_PRT("Logov2-demo not init!\n");\
		return -1; \
	} \
}while(0)


#define CHECK_LOGOV2_COLORIDX(color_idx,pixdepth) \
do{ \
	if(color_idx > ((1 << pixdepth) - 1)) { \
		LOGOV2_DBG_PRT("color idx(%d) should less than %d\n",color_idx,(pixdepth - 1)); \
		return -1; \
	} \
}while(0)

#define CHECK_LOGOV2_MALLOC(ptr,size) \
do{ \
	if(ptr == NULL) { \
		LOGOV2_DBG_PRT("malloc temp memory(%d) fail!\n",size); \
		return -1; \
	} \
}while(0)

void stringformchartoint(unsigned int *text,char * s,unsigned int cnt)
{
	int i;
	for (i = 0; i < cnt; i++)
	{
		*(text + i) = (unsigned int)*(s + i);
	}
	return;
}

int _logov2_draw_line_x(char *addr,unsigned int stride,unsigned int pixdepth,
	unsigned int idx,unsigned int x,unsigned int y,unsigned int w)
{
	unsigned int val;
	int offset,i,mask;
	char *line;
	line   = addr + stride * y + x * pixdepth/8/4*4;//按4byte对齐
	offset = (x * pixdepth & 0x1f);//32取余
	mask   = (pixdepth >= 32) ? 0xffffffff : ((1 << pixdepth) - 1);
	val = *((unsigned int *)line);
	for(i = 0;i < w; i ++)
	{
		val &= ~(mask << offset);
		val |= (idx << offset);

		offset = ((offset + pixdepth) & 0x1f);//32取余
		if(offset == 0)
		{
			*(unsigned int *)(line) = val;
			line += 4;
			val = *((unsigned int *)line);
		}
	}
	if(offset != 0)
		*(unsigned int *)(line) = val;
	return 0;
}

int _logov2_draw_line_y(char *addr,unsigned int stride,unsigned int pixdepth,
	unsigned int idx,unsigned int x,unsigned int y,unsigned int h)
{
	unsigned int val;
	int offset,i,mask;
	char *line;
	line   = addr + stride * y + x * pixdepth/8/4*4;//按4byte对齐
	offset = (x * pixdepth & 0x1f);//32取余
	mask   = (pixdepth >= 32) ? 0xffffffff : ((1 << pixdepth) - 1);
	for(i = 0;i < h; i ++)
	{
		val     = *((unsigned int *)line);
		val    &= ~(mask << offset);
		val    |= (idx << offset);
		*(unsigned int *)(line) = val;
		line   += stride;
	}
	return 0;
}

int _logov2_get_chartype(unsigned int x)
{
	if(x<=0xff)
		return OSD_TYPE_ASCII;
	else
		return OSD_TYPE_GB2312;
}

unsigned int _logov2_get_gb2312_offset(unsigned int x)
{
	return ((94*((x>>8)-0xa0-1)+((x&0xff)-0xa0-1))*32);
}

int _logov2_get_step(int blkw,int blkh,unsigned int rotate,
	int *xstep,int *ystep,int *woffset,int *hoffset)
{
	switch(rotate)
	{
		case 0:
		{
			*xstep   = blkw;
			*ystep   = 0;
			*woffset = (blkw - 1);
			*hoffset = (blkh - 1);
			break;
		}
		case 1:
		{
			*xstep   = 0;
			*ystep   = -blkw;
			*woffset = (blkh - 1);
			*hoffset = -(blkw - 1);
			break;
		}
		case 2:
		{
			*xstep   = -blkw;
			*ystep   = 0;
			*woffset = -(blkw - 1);
			*hoffset = -(blkh - 1);
			break;
		}
		case 3:
		{
			*xstep   = 0;
			*ystep   = blkw;
			*woffset = -(blkh - 1);
			*hoffset = (blkw - 1);
			break;
		}
		default:
		{
			*xstep   = blkw;
			*ystep   = 0;
			*woffset = (blkw - 1);
			*hoffset = (blkh - 1);
			break;
		}
	}
	return 0;
}

int _logov2_get_char_step(int fontsize,unsigned int rotate,
	int *asccharx,int *ascchary,int *asccharw,int *asccharh,
	int *gb2312charx,int *gb2312chary,int *gb2312charw,int *gb2312charh)
{
	_logov2_get_step(fontsize/2,fontsize,rotate,asccharx,ascchary,asccharw,asccharh);
	_logov2_get_step(fontsize,fontsize,rotate,gb2312charx,gb2312chary,gb2312charw,gb2312charh);
	return 0;
}

int _logov2_check_location(int sx,int sy,int ex,int ey,int logow,int logoh)
{
	if(sx < 0 || sx >= logow || ex < 0 || ex >= logow ||
		sy < 0 || sy >= logoh || ey < 0 || ey >= logoh)
	{
		LOGOV2_DBG_PRT("location(%d,%d - %d,%d) out of logo(%dx%d)\n",
			sx,sy,ex,ey,logow,logoh);
		return -1;
	}
	return 0;
}

int _logov2_do_char_invert(LOGOV2_INFO *osd_cfg,int sx,int sy,int ex,int ey,
	unsigned int color_idx,unsigned int color_magic1,unsigned int color_magic2)
{
	char *startaddr,*curaddr;
	unsigned int pixdepth,mask,data;
	unsigned int color,pk_color1,pk_color2,cur_color;
	int x,y,w,h,i,j,offset,curoffset;
//	int charw,charh;
	x = MIN(sx,ex);
	y = MIN(sy,ey);
	w = MAX(sx,ex) - x + 1;
	h = MAX(sy,ey) - y + 1;

	pixdepth = osd_cfg->pixdepth;
	mask = (osd_cfg->pixdepth >= 32) ? 0xffffffff : ((1 << pixdepth) - 1);
	startaddr = osd_cfg->logo_buf.vbase + y * osd_cfg->stride + x*osd_cfg->pixdepth/8/4*4;//按4byte对齐
	offset = (x * osd_cfg->pixdepth & 0x1f);//32取余
	color     = (color_idx & mask);
	pk_color1 = (color_magic1 & mask);
	pk_color2 = (color_magic2 & mask);

	for(j = 0; j < h; j ++)
	{
		curaddr   = startaddr + osd_cfg->stride*j;
		curoffset = offset;
		data      = *(unsigned int *)(curaddr);
		for(i = 0; i < w; i ++)
		{
			cur_color = ((data >> curoffset) & mask);
			if(cur_color == pk_color1 || cur_color == pk_color2)
			{
				data &= ~(mask << curoffset);
				data |= (color) << curoffset;
			}

			curoffset = ((curoffset + osd_cfg->pixdepth) & 0x1f);//32取余
			if(curoffset == 0)
			{
				*(unsigned int *)(curaddr) = data;
				curaddr += 4;
				data = *(unsigned int *)(curaddr);
			}
		}
		if(curoffset != 0)
			*(unsigned int *)(curaddr) = data;
	}
	return 0;
}

//字缩放描边处理,输出结果字体WxH Byte,0代表背景,1代表字体,2/3代表描边
int _logov2_scaler_char(char *pFont,char *pDst,unsigned int chartype,
	unsigned int fontsize,unsigned int isaddedge,unsigned int edge2pixel)
{
	int i,j,m,sum;
	char s;
//	char *pTemp;
	unsigned int orgw,orgh,dstw,dsth;
	unsigned int val,x,y;
	char ch[16][16];
	char *scaler_data,*edge_data;

	edge_data = pDst;

	//字库点阵-->数组,方面后续读取
	if (chartype == OSD_TYPE_GB2312)
	{
		orgw = 16;
		orgh = 16;
		dstw = fontsize;
		dsth = fontsize;
		for (i = 0; i < 16; i++)
		{
			for (m = 0; m < 2; m ++)
			{
				s = pFont[i * 2 + m];
				for(j = 0;j < 8;j ++)
				{
					if((s & (1 << (7 - j))) != 0 )
						ch[i][m * 8 + j] = 1;
					else
						ch[i][m * 8 + j] = 0;
				}
			}
		}
	}
	else
	{
		orgw = 8;
		orgh = 16;
		dstw = fontsize / 2;
		dsth = fontsize;
		for (i = 0; i < 16; i++)
		{
			s = pFont[i];
			for(j = 0;j < 8;j ++)
			{
				if((s & (1 << (7 - j))) != 0 )
					ch[i][j] = 1;
				else
					ch[i][j] = 0;
			}
		}
	}

	//字体缩放
	scaler_data = malloc(dsth * dstw);
	CHECK_LOGOV2_MALLOC(scaler_data,dsth * dstw);
	for (i = 0; i < dsth ; i++)
	{
		for(j = 0;j < dstw;j ++)
		{
			val = 0;
			x = j * orgw / dstw;
			y = i * orgh / dsth;
			val += ch[y][x];
			val += ch[y][MIN(x,dstw - 1)];
			val += ch[MIN(y,dsth - 1)][x];
			val += ch[MIN(y,dsth - 1)][MIN(x,dstw - 1)];
			if (val >= 2)
			{
				*(scaler_data + i * dstw + j) = 1;
			}
			else
			{
				*(scaler_data + i * dstw + j) = 0;
			}
		}
	}

	//描边
	if(isaddedge)
	{
		for (i = 0; i < dsth ; i++)
		{
			for(j = 0;j < dstw;j ++)
			{
				if(i == 0 || j == 0 || i == (dsth - 1) || j == (dstw - 1))
				{
					sum = *(scaler_data + i * dstw + j);
				}
				else
				{
					sum = *(scaler_data + i * dstw + j);
					if(sum)
					{
						sum = 9;
					}
					else
					{
						sum += *(scaler_data + i * dstw + j - 1);
						sum += *(scaler_data + i * dstw + j + 1);

						sum += *(scaler_data + i * dstw + j + dstw - 1);
						sum += *(scaler_data + i * dstw + j + dstw);
						sum += *(scaler_data + i * dstw + j + dstw + 1);

						sum += *(scaler_data + i * dstw + j - dstw - 1);
						sum += *(scaler_data + i * dstw + j - dstw);
						sum += *(scaler_data + i * dstw + j - dstw + 1);
					}
				}

				if(sum == 0)
				{
					*(edge_data + i * dstw + j) = 0;
				}
				else if(sum == 9)
				{
					*(edge_data + i * dstw + j) = 1;
				}
				else
				{
					*(edge_data + i * dstw + j) = 2;
				}
			}
		}

		if(edge2pixel)
		{
			for (i = 0; i < dsth ; i++)
			{
				for(j = 0;j < dstw;j ++)
				{
					if(*(edge_data + i * dstw + j) == 0)
					{
						int xl,xr,yu,yd;
						xl = MAX(0,j - 1);
						xr = MIN(dstw - 1,j + 1);
						yu = MAX(0,i - 1);
						yd = MIN(dsth - 1,i + 1);
						if(*(edge_data + i * dstw + xl) == 2 ||
							*(edge_data + i * dstw + xr) == 2 ||
							*(edge_data + yu * dstw + j) == 2 ||
							*(edge_data + yd * dstw + j) == 2)
						{
							*(edge_data + i * dstw + j) = 3;
						}
					}
				}
			}
		}
	}
	else
	{
		memcpy(edge_data,scaler_data,dsth * dstw);
	}

	free(scaler_data);
	return 0;
}

int _logov2_draw_char(LOGOV2_INFO *osd_cfg,char *pchar,unsigned int rotate,
	unsigned int fontcolor, unsigned int edgecolor,unsigned int bkgcolor,
	unsigned int fontw,unsigned int fonth,int sx,int sy,int ex,int ey)
{
	char *startaddr,*curaddr,*font_data;
	unsigned int pixdepth,mask,data;
	unsigned int color[4];
	int x,y,i,j,offset,curoffset;
	x = MIN(sx,ex);
	y = MIN(sy,ey);

	pixdepth = osd_cfg->pixdepth;
	mask = (osd_cfg->pixdepth >= 32) ? 0xffffffff : ((1 << pixdepth) - 1);
	startaddr = osd_cfg->logo_buf.vbase + y * osd_cfg->stride + x*osd_cfg->pixdepth/8/4*4;//按4byte对齐
	offset = (x * osd_cfg->pixdepth & 0x1f);//32取余
	color[0] = (bkgcolor & mask);
	color[1] = (fontcolor & mask);
	color[2] = (edgecolor & mask);
	color[3] = (edgecolor & mask);

	switch(rotate)
	{
		case 0:
		{
			for(j = 0; j < fonth; j ++)
			{
				font_data = pchar + fontw * j;
				curaddr   = startaddr + osd_cfg->stride*j;
				curoffset = offset;
				data      = *(unsigned int *)(curaddr);
				for(i = 0; i < fontw; i ++)
				{
					data &= ~(mask << curoffset);
					data |= (color[CLIP(*font_data,0,3)]) << curoffset;

					curoffset = ((curoffset + osd_cfg->pixdepth) & 0x1f);//32取余
					if(curoffset == 0)
					{
						*(unsigned int *)(curaddr) = data;
						curaddr += 4;
						data = *(unsigned int *)(curaddr);
					}
					font_data ++;
				}
				if(curoffset != 0)
					*(unsigned int *)(curaddr) = data;
			}
			break;
		}
		case 1:
		{
			for(j = 0; j < fontw; j ++)
			{
				font_data = pchar + fontw - j - 1;
				curaddr   = startaddr + osd_cfg->stride*j;
				curoffset = offset;
				data      = *(unsigned int *)(curaddr);
				for(i = 0; i < fonth; i ++)
				{
					data &= ~(mask << curoffset);
					data |= (color[CLIP(*font_data,0,3)]) << curoffset;

					curoffset = ((curoffset + osd_cfg->pixdepth) & 0x1f);//32取余
					if(curoffset == 0)
					{
						*(unsigned int *)(curaddr) = data;
						curaddr += 4;
						data = *(unsigned int *)(curaddr);
					}
					font_data += fontw;
				}
				if(curoffset != 0)
					*(unsigned int *)(curaddr) = data;
			}
			break;
		}
		case 2:
		{
			for(j = 0; j < fonth; j ++)
			{
				font_data = pchar + fontw * fonth - j * fontw - 1;
				curaddr   = startaddr + osd_cfg->stride*j;
				curoffset = offset;
				data      = *(unsigned int *)(curaddr);
				for(i = 0; i < fontw; i ++)
				{
					data &= ~(mask << curoffset);
					data |= (color[CLIP(*font_data,0,3)]) << curoffset;

					curoffset = ((curoffset + osd_cfg->pixdepth) & 0x1f);//32取余
					if(curoffset == 0)
					{
						*(unsigned int *)(curaddr) = data;
						curaddr += 4;
						data = *(unsigned int *)(curaddr);
					}
					font_data --;
				}
				if(curoffset != 0)
					*(unsigned int *)(curaddr) = data;
			}
			break;
		}
		case 3:
		{
			for(j = 0; j < fontw; j ++)
			{
				font_data = pchar + fontw * fonth - fontw + j;
				curaddr   = startaddr + osd_cfg->stride*j;
				curoffset = offset;
				data      = *(unsigned int *)(curaddr);
				for(i = 0; i < fonth; i ++)
				{
					data &= ~(mask << curoffset);
					data |= (color[CLIP(*font_data,0,3)]) << curoffset;

					curoffset = ((curoffset + osd_cfg->pixdepth) & 0x1f);//32取余
					if(curoffset == 0)
					{
						*(unsigned int *)(curaddr) = data;
						curaddr += 4;
						data = *(unsigned int *)(curaddr);
					}
					font_data -= fontw;
				}
				if(curoffset != 0)
					*(unsigned int *)(curaddr) = data;
			}
			break;
		}
		default:
			LOGOV2_DBG_PRT("rotate-%d not exist.\n",rotate);
			break;
	}
	return 0;
}

int logov2_osd_set_mask(LOGOV2_INFO *osd_cfg,unsigned int x,unsigned int y,
	unsigned int w,unsigned int h,unsigned int color_idx)
{
	unsigned int i;
	CHECK_LOGOV2_ISINIT(osd_cfg);
	CHECK_LOGOV2_COLORIDX(color_idx, osd_cfg->pixdepth);
	if(_logov2_check_location(x,y,x+w-1,y+h-1,osd_cfg->w,osd_cfg->h)!=0)
		return -1;

	for (i = 0; i < h; i++)
	{
		_logov2_draw_line_x(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
			color_idx,x,y+i,w);
	}
	return 0;
}

int logov2_osd_set_gbox(LOGOV2_INFO *osd_cfg,unsigned int x,unsigned int y,
	unsigned int w,unsigned int h,unsigned short color_idx)
{
	CHECK_LOGOV2_ISINIT(osd_cfg);
	CHECK_LOGOV2_COLORIDX(color_idx, osd_cfg->pixdepth);
	if(_logov2_check_location(x,y,x+w-1,y+h-1,osd_cfg->w,osd_cfg->h)!=0)
		return -1;

	_logov2_draw_line_x(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x,y,w);
	_logov2_draw_line_x(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x,y+1,w);
	_logov2_draw_line_x(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x,y+h-2,w);
	_logov2_draw_line_x(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x,y+h-1,w);

	_logov2_draw_line_y(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x,y,h);
	_logov2_draw_line_y(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x+1,y,h);
	_logov2_draw_line_y(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x+w-2,y,h);
	_logov2_draw_line_y(osd_cfg->logo_buf.vbase,osd_cfg->stride,osd_cfg->pixdepth,
		color_idx,x+w-1,y,h);
	return 0;
}

int logov2_osd_set_text(LOGOV2_INFO *osd_cfg,unsigned int x,unsigned int y,
	unsigned int * s,unsigned int cnt,int fontsize,unsigned int fontcolor,
	unsigned short edgecolor,unsigned int bkgcolor,unsigned int rotate,unsigned int edge2pixel)
{
	int i;
	int fontw = fontsize;
	unsigned int chartype,ch,isaddedge;
	char *fontaddr,*tmp;
//	unsigned int logoaddrs,logoaddre;
	int sx,sy,ex,ey;
	int ascxstep,ascystep,ascxoffset,ascyoffset;
	int cnxstep,cnystep,cnxoffset,cnyoffset;
	CHECK_LOGOV2_ISINIT(osd_cfg);

	tmp = (char *)malloc(fontw * fontw);
	CHECK_LOGOV2_MALLOC(tmp,fontw * fontw);

	_logov2_get_char_step(fontw,rotate,&ascxstep,&ascystep,&ascxoffset,&ascyoffset,
		&cnxstep,&cnystep,&cnxoffset,&cnyoffset);
	sx = x;
	sy = y;
	isaddedge = (fontcolor != edgecolor);
	for(i = 0; i < cnt;i++)
	{
		ch = *(s + i);
		chartype = _logov2_get_chartype(ch);
		if(chartype == OSD_TYPE_ASCII)
		{
			ex = sx + ascxoffset;
			ey = sy + ascyoffset;
			if(_logov2_check_location(sx,sy,ex,ey,osd_cfg->w,osd_cfg->h)!=0)
				break;
			fontaddr = (char *)(asc16+ch*16);

			_logov2_scaler_char(fontaddr,tmp,chartype,fontw,isaddedge,edge2pixel);
			_logov2_draw_char(osd_cfg,tmp,rotate,fontcolor,edgecolor,bkgcolor,
				fontw/2,fontw,sx,sy,ex,ey);

			sx += ascxstep;
			sy += ascystep;
		}
		else
		{
			ex = sx + cnxoffset;
			ey = sy + cnyoffset;
			// LOGOV2_DBG_PRT("[%d,%d]x[%d,%d]\n",sx,sy,ex,ey);
			if(_logov2_check_location(sx,sy,ex,ey,osd_cfg->w,osd_cfg->h)!=0)
				break;
			fontaddr=(char *)gb2312+_logov2_get_gb2312_offset(ch);

			_logov2_scaler_char(fontaddr,tmp,chartype,fontw,isaddedge,edge2pixel);
			_logov2_draw_char(osd_cfg,tmp,rotate,fontcolor,edgecolor,bkgcolor,
				fontw,fontw,sx,sy,ex,ey);

			sx += cnxstep;
			sy += cnystep;
		}
	}
	free(tmp);
	return 0;
}

int logov2_osd_invert(LOGOV2_INFO *osd_cfg,unsigned int x,unsigned int y,
	unsigned int blkw,unsigned int blkh,unsigned int char_cnt,unsigned int *inv_ctl,
	unsigned int norcolor,unsigned int invcolor,unsigned int rotate)
{
	int i;
	int xstep,ystep,xoffset,yoffset;
	int sx,sy,ex,ey;
	unsigned int color;

	CHECK_LOGOV2_ISINIT(osd_cfg);
	_logov2_get_step(blkw,blkh,rotate,&xstep,&ystep,&xoffset,&yoffset);

	sx = x;
	sy = y;
	for(i = 0; i < char_cnt;i++)
	{
		ex = sx + xoffset;
		ey = sy + yoffset;
		// LOGOV2_DBG_PRT("[%d,%d]x[%d,%d]\n",sx,sy,ex,ey);
		if(_logov2_check_location(sx,sy,ex,ey,osd_cfg->w,osd_cfg->h)!=0)
			break;

		color = inv_ctl[i] ? invcolor : norcolor;
		_logov2_do_char_invert(osd_cfg,sx,sy,ex,ey,color,norcolor,invcolor);
		sx += xstep;
		sy += ystep;
	}
	return 0;
}

int logov2_osd_clear_area_with_rotate(LOGOV2_INFO *osd_cfg,unsigned int x,unsigned int y,
	unsigned int w,unsigned int h,unsigned int color_idx,unsigned int rotate)
{
	int x1,y1,w1,h1;
	switch(rotate)
	{
		case 0:
		{
			x1 = x;
			y1 = y;
			w1 = w;
			h1 = h;
			break;
		}
		case 1:
		{
			x1 = x;
			y1 = (y - w);
			w1 = h;
			h1 = w;
			break;
		}
		case 2:
		{
			x1 = (x - w);
			y1 = (y - h);
			w1 = w;
			h1 = h;
			break;
		}
		case 3:
		{
			x1 = (x - h);
			y1 = y;
			w1 = h;
			h1 = w;
			break;
		}
		default:
			LOGOV2_DBG_PRT("rotate-%d not exist.\n",rotate);
			return -1;
	}
	return logov2_osd_set_mask(osd_cfg,x1,y1,w1,h1,color_idx);
}

int logov2_osd_clear_area(LOGOV2_INFO *osd_cfg,unsigned int x,unsigned int y,
	unsigned int w,unsigned int h,unsigned int color_idx)
{
	return logov2_osd_set_mask(osd_cfg,x,y,w,h,color_idx);
}

unsigned int getrandcolor_32()
{
	unsigned int color;
	color = (CLIP((rand() % 0xff),90,0xff) << 24) | ((rand() % 0xff) << 16) |
		((rand() % 0xff) << 8) | ((rand() % 0xff));
	return color;
}

unsigned int rgb8888torgb1555(unsigned int rgb8888)
{
	unsigned int color,alpha = 1;
	if((rgb8888 >> 24) == 0)
		alpha = 0;
	color = (alpha << 15) | (((rgb8888 >> 19) & 0x1f) << 10) |
		(((rgb8888 >> 11) & 0x1f) << 5) | ((rgb8888 >> 3) & 0x1f);
	return color;
}

unsigned int rgb8888torgb4444(unsigned int rgb8888)
{
	unsigned int color;
	color = (((rgb8888 >> 28) & 0xf) << 12) | (((rgb8888 >> 20) & 0xf) << 8) |
		(((rgb8888 >> 12) & 0xf) << 4) | (((rgb8888 >> 4) & 0xf));
	return color;
}

unsigned int rgb8888torgb0565(unsigned int rgb8888)
{
	unsigned int color;
	color = (((rgb8888 >> 19) & 0x1f) << 11) | (((rgb8888 >> 10) & 0x3f) << 5) |
		(((rgb8888 >> 3) & 0x1f));
	return color;
}

void region_set_info(FY_U32 chn, char* vi_type)
{
    sprintf(g_vi_type[chn],"%s", vi_type);
}

int Region_logov2_osd(unsigned int grp_idx, unsigned int chan, RGN_HANDLE handle,unsigned int pixeldepth,FY_U32 index,FY_U32 invert)
{
	char data[100];
	unsigned int text[100];
    unsigned int color[5];
	int ret,w,cnt,x,y;
	LOGOV2_INFO logo2;
	VPSS_CHN_MODE_S mode;
	FY_U32 time_osd_w;
	MPP_CHN_S stChn;
	RGN_CHN_ATTR_S stChnAttr;
    time_t nSeconds;
    struct tm * pTM;
    MPP_CHN_S stSrcChn;


	RGN_CANVAS_INFO_S stCanvasInfo;

    memset(data,0,100);
    memset(&stSrcChn,0,sizeof(MPP_CHN_S));
	memset(&stCanvasInfo,0,sizeof(RGN_CANVAS_INFO_S));
	CHECK_RET(FY_MPI_RGN_GetCanvasInfo(handle,&stCanvasInfo), "FY_MPI_RGN_GetCanvasInfo");

	//FY_MPI_RGN_GetAttr(handle, &stRegion);
	stChn.enModId = FY_ID_VPSS;
	stChn.s32DevId = grp_idx;
	stChn.s32ChnId = chan;

    FY_MPI_SYS_GetBindbyDest(&stChn,&stSrcChn);

	FY_MPI_RGN_GetDisplayAttr(handle, &stChn,&stChnAttr);

	x = stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X;
	y = 0;//stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y;


	logo2.logo_buf.base = stCanvasInfo.u32PhyAddr;
	logo2.logo_buf.vbase = (void *)stCanvasInfo.u32VirtAddr;
    memset(logo2.logo_buf.vbase,0,stCanvasInfo.u32Stride*stCanvasInfo.stSize.u32Height);
	logo2.stride = stCanvasInfo.u32Stride;
	logo2.pixdepth = pixeldepth;
	logo2.w = stCanvasInfo.stSize.u32Width;
	logo2.h = stCanvasInfo.stSize.u32Height+y;
	logo2.isinit = LOGOV2_MAGIC;

    //SAMPLE_PRT("x,y,w,h is %d,%d,%d,%d,base is 0x%x,vbase:0x%x\n",x,y,stCanvasInfo.stSize.u32Width,logo2.h,logo2.logo_buf.base,logo2.logo_buf.vbase);

	ERR_RETURN("FH_VPU_GetChnMode", FY_MPI_VPSS_GetChnMode(grp_idx,chan,&mode));

	w = mode.u32Width;

	if(w >= 1280)
	{
		//bps_osd_w  = 48;
		time_osd_w = 54;
	}
	else if(w>=640)
	{
		//bps_osd_w  = 30;
		time_osd_w = 24;
	}
    else if(w>400)
        {
		//bps_osd_w  = 16;
		time_osd_w = 16;
    }
    else if(w>200){
		//bps_osd_w  = 12;
		time_osd_w = 12;
    }
    else{
		//bps_osd_w  = 8;
		time_osd_w = 8;
    }
    //printf("bps_osd_w = %d\n", bps_osd_w);
    time(&nSeconds);
    pTM = localtime(&nSeconds);

    if(pixeldepth == 16)
    {
        color[0] = rgb8888torgb1555(0x00000000);
        color[1] = rgb8888torgb1555(0xFF000000);
        color[2] = rgb8888torgb1555(0xFFFFFFFF);
        color[3] = rgb8888torgb1555(getrandcolor_32());
        color[4] = rgb8888torgb1555(getrandcolor_32());
    }

    {
        if(index == 0){
            cnt = sprintf(data,"%02d %02d:%02d:%02d-vi:%s-c:%d",pTM->tm_mday,\
                pTM->tm_hour,pTM->tm_min,pTM->tm_sec,g_vi_type[stSrcChn.s32ChnId%16],stSrcChn.s32ChnId);
    		stringformchartoint(text,data,cnt);
        }
        else if(index != 0){
            cnt = sprintf(data,"Region handle:%d, grp:%d, chn:%d",handle,grp_idx,chan);
            stringformchartoint(text,data,cnt);
        }
        if(pixeldepth == 16){
            if(invert)
                ret = logov2_osd_set_text(&logo2,x,y,text,cnt,time_osd_w,color[1],color[1],color[0],0,0);
            else
                ret = logov2_osd_set_text(&logo2,x,y,text,cnt,time_osd_w,color[2],color[2],color[0],0,0);
        }
        else
            ret = logov2_osd_set_text(&logo2,x,y,text,cnt,time_osd_w,(!invert)+1,0,0,0,0);
		if (ret != 0)
			printf("[Demo-logov2]logov2_osd_set_text ret=%d\n",ret);
	}

	FY_MPI_RGN_UpdateCanvas(handle);
#if 0
	 while(! *cancel)
	 {
	 	FH_VENC_GetChnStatus(chan,&chnstat);
	 	FH_VPSS_GetFrameRate(chan,&fps);
	 	frmrate =(fps.frame_time == 0) ? 0 : fps.frame_count/fps.frame_time;
	 	cnt = sprintf(data,"  Chn:%d %4d kbps %.2f fps  ",chan,chnstat.bps / 1000,frmrate);
	 	stringformchartoint(text,data,cnt);
	 	ret = logov2_osd_set_text(&logo2,(w - bps_osd_w * cnt / 2 - 10),32,text,cnt,bps_osd_w,1,1,0,0,0);
	 	if (ret != 0)
	 		printf("[Bps]logo_osd_set_text ret=%d\n",ret);
	 	localtime_r(&timeT, &ct);
	 	cnt = 0;
	 	cnt_tmp = sprintf(data,"%4d",ct.tm_year + 1900);
	 	stringformchartoint(&osd_time[cnt],data,cnt_tmp);
	 	cnt += cnt_tmp;
	 	osd_time[cnt++] = 0xc4ea;
	 	cnt_tmp = sprintf(data,"%02d",ct.tm_mon + 1);
	 	stringformchartoint(&osd_time[cnt],data,cnt_tmp);
	 	cnt += cnt_tmp;
	 	osd_time[cnt++] = 0xd4c2;
	 	cnt_tmp = sprintf(data,"%02d",ct.tm_mday);
	 	stringformchartoint(&osd_time[cnt],data,cnt_tmp);
	 	cnt += cnt_tmp;
	 	osd_time[cnt++] = 0xc8d5;
	 	osd_time[cnt++] = 0x20;
	 	osd_time[cnt++] = 0xd0c7;
	 	osd_time[cnt++] = 0xc6da;
	 	osd_time[cnt++] = weeksNameChn[ct.tm_wday];
	 	cnt_tmp = sprintf(data," %02d:%02d:%02d",ct.tm_hour,ct.tm_min,ct.tm_sec);
	 	stringformchartoint(&osd_time[cnt],data,cnt_tmp);
	 	cnt += cnt_tmp;
	 	ret = logov2_osd_set_text(&logo2,16,32,osd_time,cnt,time_osd_w,1,1,0,0,0);
	 	if (ret != 0)
	 		printf("[Bps]logo_osd_set_text ret=%d\n",ret);
	 	usleep(400*1000);
	 }
#endif
	return 0;
}
