#include "TViewSysSetFrame.h"
#include "TViewStyle.h"
#include "TViewFontUtils.h"

#include "TViewConfigWinBaseFrame.h"

typedef struct{
    int     menuItemID;
    char    menuItemText[256];
    char    menuItemImage[256];
    char    menuItemFontAwesome[10];
    StViewConfigWinBaseFrameParamsItemInfo* itemConfigInfo;
}StMainMenuItemInfo;


static StViewConfigWinBaseFrameParamsItemInfo testiteminfo[] = {
    {"基本设置", NULL},
    {"网络设置", NULL},
    {"测试", NULL},
    {"关于信息", NULL},
    {"", NULL}
};

StMainMenuItemInfo gStMainMenuItemInfoList[] = {
    {1, "基本设置",     "",     "\uf2c3", testiteminfo},
    {2, "通道管理",     "",     "\uf03d", NULL},
    {4, "录相回放",     "",     "\uf1c8", NULL},
    
    {4, "存储管理",     "",     "\uf1c0", NULL},
    {5, "报警设置",     "",     "\uf071", NULL},
    
    {6, "网络设置",     "",     "\uf26b", NULL},
    {7, "云平台",       "",     "\uf0ee", NULL},

    {8, "设备设置",     "",     "\uf085", NULL},
};


static StMainMenuItemInfo* getMainMenuItemInfoByID(int id)
{
    for(unsigned int i = 0; i < sizeof(gStMainMenuItemInfoList) / sizeof(gStMainMenuItemInfoList[0]); i++)
    {
        if(gStMainMenuItemInfoList[i].menuItemID == id)
        {
            return &gStMainMenuItemInfoList[i];
        }
    }

    return NULL;
}

TViewSysSetFrame::TViewSysSetFrame(ViewHandle parentHandle): TViewObject(parentHandle)
{
    viewInit();
    
}

TViewSysSetFrame::~TViewSysSetFrame()
{

}

bool TViewSysSetFrame::viewInit()
{
    int width = 800;
    int height = 500;
    setViewSize(width, height);
	lv_obj_center(m_viewHandle);

    TViewStyle::getInstance()->setDalogModelViewWindowStyle(this);

    //lv_obj_t* plabTitle = TViewStyle::getInstance()->frameTitleCreater(m_viewHandle, 40, "系统设置");
    viewMenuItemInit();

    
    return true;
}

void TViewSysSetFrame::viewMenuItemInit()
{
    unsigned int i = 0;

    int itemWidth = 120;
    int itemHeight = 120;
    int colNumber = 4;
    //int rowNumber = 2;

    int col = 0;
	int row = 0;

	static lv_coord_t col_dsc[] = {itemWidth, itemWidth, itemWidth, itemWidth, LV_GRID_TEMPLATE_LAST};
	static lv_coord_t row_dsc[] = {itemHeight, itemHeight, LV_GRID_TEMPLATE_LAST};

    StMainMenuItemInfo* pItem = NULL;

    lv_obj_set_style_pad_all(m_viewHandle, 0, 0);

    lv_obj_t* lvObjMenuPanel = lv_obj_create(m_viewHandle);

    int ctwidth = lv_obj_get_content_width(m_viewHandle) - 20;
    int ctheight = lv_obj_get_content_height(m_viewHandle) * 0.8;
    
    lv_obj_set_size(lvObjMenuPanel, ctwidth, ctheight);
    lv_obj_center(lvObjMenuPanel);
    lv_obj_set_style_border_width(lvObjMenuPanel, 0, 0);
    lv_obj_set_style_radius(lvObjMenuPanel, 0, 0);
    lv_obj_set_style_bg_color(lvObjMenuPanel, TViewStyle::DialogFrameBgColor, 0);
    lv_obj_set_layout(lvObjMenuPanel, LV_LAYOUT_GRID);

    lv_obj_set_style_grid_column_dsc_array(lvObjMenuPanel, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(lvObjMenuPanel, row_dsc, 0);

    lv_obj_set_grid_align(lvObjMenuPanel, LV_GRID_ALIGN_SPACE_AROUND, LV_GRID_ALIGN_SPACE_AROUND);

    for(i = 0; i < sizeof(gStMainMenuItemInfoList) / sizeof(gStMainMenuItemInfoList[0]); i++)
    {
        col = i % colNumber;
        row = i / colNumber;
        
        pItem = &gStMainMenuItemInfoList[i];
        
        //item 
        lv_obj_t* lvObjItem = lv_obj_create(lvObjMenuPanel);
        lv_obj_set_style_bg_color(lvObjItem, TViewStyle::DialogFrameBgColor, 0);
        lv_obj_set_scrollbar_mode(lvObjItem, LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_border_width(lvObjItem, 0, 0);
        lv_obj_set_style_pad_all(lvObjItem, 0, 0);
        /*设置布局*/
        lv_obj_set_layout(lvObjItem, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(lvObjItem, LV_FLEX_FLOW_COLUMN);
        /*让项目中的文本和图片分别都居中排列,这样设置后，图片，文本项都自动在水平，垂直上都居中自动分配空间了*/
        lv_obj_set_flex_align(lvObjItem, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_set_user_data(lvObjItem, pItem);

        lv_obj_add_event_cb(lvObjItem, _menuitem_event_process, LV_EVENT_PRESSED, this);
        lv_obj_add_event_cb(lvObjItem, _menuitem_event_process, LV_EVENT_RELEASED, this);
        lv_obj_add_event_cb(lvObjItem, _menuitem_event_process, LV_EVENT_CLICKED, this);

        //image
        lv_obj_t* plabItemImg = lv_label_create(lvObjItem);
        lv_obj_set_style_text_color(plabItemImg, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_font(plabItemImg, TViewFontUtils::getInstance()->getViewFont(VF_FONT_FontAwesome, 50), 0);
        lv_label_set_text(plabItemImg, pItem->menuItemFontAwesome);
    

        //text
        lv_obj_t* plabItemText = lv_label_create(lvObjItem);
        lv_obj_set_style_text_color(plabItemText, lv_color_hex(0xffffff), 0);    
        lv_obj_set_style_text_font(plabItemText, TViewFontUtils::getInstance()->getDefaultFont(16, 0), 0);
        lv_label_set_text(plabItemText, pItem->menuItemText);

        /*设置项目在行，列位置*/
        lv_obj_set_grid_cell(lvObjItem, LV_GRID_ALIGN_STRETCH, col, 1,
            LV_GRID_ALIGN_STRETCH, row, 1);
    }

    //TViewFontUtils::getInstance()->releaseViewFont(VF_FONT_FontAwesome, 50, 0);
}

void TViewSysSetFrame::__menuitem_event_process(lv_event_t *event)
{
    lv_obj_t *obj = event->target;  
    if(event->code == LV_EVENT_PRESSED)
    {
        lv_obj_set_style_bg_color(obj, lv_palette_darken(LV_PALETTE_BLUE, 2), 0);
    }else if(event->code == LV_EVENT_RELEASED){  
        lv_obj_set_style_bg_color(obj, TViewStyle::DialogFrameBgColor, 0);
    }else if(event->code == LV_EVENT_CLICKED){  
        StMainMenuItemInfo* pItem = (StMainMenuItemInfo*)obj->user_data;
        if(pItem)
        {
            _menuitem_clicked_process(pItem->menuItemID);     
        }
    }
}

void TViewSysSetFrame::_menuitem_event_process(lv_event_t *event)
{
    TViewSysSetFrame* obj = (TViewSysSetFrame*)event->user_data;    
    obj->__menuitem_event_process(event);
}

void TViewSysSetFrame::_menuitem_clicked_process(int id)
{
    TViewConfigWinBaseFrame* win = NULL;

    StMainMenuItemInfo* itemInfo = getMainMenuItemInfoByID(id);
    if(itemInfo)
    {
        win = new TViewConfigWinBaseFrame();
        win->setViewConfigWinTitle(itemInfo->menuItemText);
        win->setViewConfigWinBaseFrameParams(itemInfo->itemConfigInfo);
        win->viewShow();
    }
    
}
