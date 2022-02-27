# PUPUA NVR UI开发

### UI的选择
本来原来考虑到用的MINIGUI的，但看到那种古老的控件风格，想做现代点可能贴图就搞得不胜其烦了，并且其开源的方式也不太友好！

再看了下各种UI库,看了下LVGL的库的例子，虽然一直没有怎么了解过，但瞬间就爱了！

好吧就用LVGL了！

### 移植
谈不上什么移值了，framebuffer已经足够简单了，在海思平台上，可以具体见上一往篇Hi3536C上使用的介绍.

### 单进程还是多线程
我在想是否需要把UI做成一个独立的进程，有点纠集，如果写成一个独立的进程吧，在UI上同后台录相管理进程交互又会比较多，想到需要服务端与客户端模式，调用及通知、协议格式解码都需要大量的代码量！感觉那种gdbus这些工具也并不好用，工作量不小啊！ 如果用单进程方式，在以前的产品中又常常出现各代码模块偶合调用，维护的人一多，后面又经常不成样了！但多进程方式时，如果前后端存在客户定制进也让人头痛！
考虑到后续交互，及代码量还是先用单进程方式吧！把所有UI的代码限在一个View的目录中，UI与外界面通信，交互都通过一个层来转换，这样后续如果使用多进程方式也可以快速修改!

### View基本说明
我把所有的UI的代码都放在view的工程目录中，对外只提供了一个 view_main.h初始化及一些调用接口!

			/*
			 * UI的初始化操作
			 * */
			int 	view_init();

			/*
			 * UI的退出操作
			 * */
			void 	view_exit();

			/*
			 * 进入UI处理过程
			 * */
			int		view_process();

具体可见main.cpp中的调用，比较简单，初始化后，主线程阻塞在view_process上了!标准的UI使用方式!

在view里定义了一个view_lvgl.h的接口，这里后续定义一些lvgl相关的定义，并头文件引用! 及lvgl的一些相关操作!
			
			/*
			 * lvgl相关的初始调用
			 * */
			int 	view_lvgl_init();
			/*
			 * 退出lvgl
			 * */
			void 	view_lvgl_exit();

			/*
			 * 进入lvgl的UI处理
			 * */
			void	view_lvgl_process();

TView_App.h定义了一个UI APP初始化的入口，lvgl也是从这里正式的初始化的，后续把UI相关最开始的初始化可以放在这里，如开机的一个画机展示.
TView_App::process里创建一个UI主界面，并调用view_lvgl_process正式进入UI的处理流程!

			int TView_App::process()
			{
				TViewMainWindow* appMainWnd = TViewMainWindow::getInstance();

				if(!appMainWnd->init())
				{
					LOG(ERROR) << "TViewMainWindow init failure!" << endl;
					return -1;
				}

				view_lvgl_process();

				return 0;
			}

### TViewObject 对UI窗口对像的一个抽像
看lvgl的文档，在lvgl里并没有什么正式的窗口概念，感觉是有意的弱化了以前各种UI库里的这种窗口的设计！而在现代的UI交互里，如我们的手机，各种屏的交互，都比较少用鼠标这种方式了，窗口的样式反而不合式！个人习惯吧，我也是把这里抽像一个基类，当做一个基类的一个窗口来使用的方式！

			class TViewObject
			{
			public:
				TViewObject();
				TViewObject(ViewHandle parentHandle);
				virtual ~TViewObject();
				/**
				 * 获取窗口对像句柄
				 * */
				virtual ViewHandle 	getViewHandle();
				/**
				 * 设置显示位置
				 * */
				virtual bool		setpos(int x, int y);
				/**
				 * 设置显示大小
				 * */
				virtual bool		setViewSize(int width, int height);

				virtual int			getViewWidth();

				virtual int 		getViewHeight();

				virtual bool 		viewShow();
				virtual bool		viewHide();

				/*事件处理*/
				virtual void		event_process(lv_event_t * event);
			private:
				static void			_event_process(lv_event_t * event);
			protected:
				/*LVGL的一个对像句柄*/
				ViewHandle m_viewHandle;
				/*LVGL的一个对像的基本样式*/
				lv_style_t m_style;
			};
		
#### ViewHandle m_viewHandle	
在View里重定义了一个ViewHandle,只是对 typedef lv_obj_t* 	ViewHandle 重定义，没办法，习惯了对像句柄的概念!!!即然是一个LVGL的对像那一定是有一个lv_obj_t的定义的！
#### lv_style_t m_style
在TViewObject对像里定义了一个lv_style_t m_style, 主要是在使用lvgl创建各个窗口对像时，发现都需要设置一下不同的一点样式，感觉很麻烦，每一个从TViewObject里出来的一个窗口都需要，所认直接放在基类了。
#### virtual void		event_process(lv_event_t * event);
在事件处理上，发现很多窗口都需要处理事件，在处理事件时又都需要定义一个事件处理回调，这里用的C++，又需要每一次都转到对应的窗口对像中来处理，所以也即在基类里直接实现一个静态的方法_event_process，再调用到 event_process这个虚方法上去！这样子类窗口，如果要处理事件，只需要override这个方法即可！不过好像这样LVGL默认的处理响应就不起作用了，刚用LVGL看后续如何操作解决这个问题!

#### 构造处理 TViewObject::TViewObject(ViewHandle parentHandle)

在构造函数里，直接创建好了，lv的对像，初始化style，及设置事件处理!

		TViewObject::TViewObject(ViewHandle parentHandle)
		{
			m_viewHandle = lv_obj_create(parentHandle);
			lv_obj_add_event_cb(m_viewHandle, _event_process, LV_EVENT_ALL, this);
			lv_style_init(&m_style);
			lv_obj_add_style(m_viewHandle, &m_style, 0);
		}
		
### 构建NVR UI中的视频预览
在UI中抽像出了几个对像，TViewMultiVideoManage:一个是多视窗管理器-，这个管理视频的分割，显示模式; TViewPlayer:UI层面上的一个视频播放器;

#### TViewMultiVideoManage
负责TViewPlayer的创建，及模式的显示在系统里定义了几种比较常见的规则的显示模式！

		typedef enum{
			View_Splite_Mode_UNKNOW = 0,
			View_Splite_mode_1 = 1,
			View_Splite_mode_4 = 4,
			View_Splite_mode_9 = 9,
			View_Splite_mode_16 = 16
		}ViewVideoPlayerSplitMode;
		
TViewMultiVideoManage当前做了如下的事:
- TViewMultiVideoManage中初始化创建各个通道的TViewPlayer放在一个map结构中，以视频channel通道号为Key来索引!；
- 在设置视频显示模式时，TViewMultiVideoManage会负责好，那些通道的TViewPlayer显示，那里不显示；
- 设置选中通道的样式；
- 负责视频的轮巡显示；

#### TViewPlayer 
TViewPlayer负责显示的样式，及通道UI的交互业务处理等！如用户单击，表示选中这个通道，改变选择的样式并调用相关的交互！


### LVGL使用感觉
通过上面的很少的代码基本实现了一个NVR UI的视频预览画面，感觉还是很好用的LVGL!!!

简单，层次少，现代，这是LVGL给我的感觉!不过才用二天，看了一天的文档，花了一天的时间试各种控件，效果，做出上面这个NVR的UI主界面，很多细节没有了解，可能还有误用的地方！后续根据经验改进吧！
		
		