
#ifndef __GRF_CTRL_I_H_
#define __GRF_CTRL_I_H_

/*This code is automatically written by the IDE, do not add user code*/

#include "grf_typedef.h"
#include "grf_view_i.h"

typedef void  grf_ctrl_t;

/********************************public struct**************************************************/
typedef enum 
{
    GRF_TOUCH_NONE,              //禁止触摸
    GRF_TOUCH_CLICK,             //可以点击
}grf_touch_mode_e;
typedef enum 
{
    GRF_SCROLL_NONE,                     // 不做处理
    GRF_SCROLL_LEFT_TOP,                // 滚动后垂直方向置顶，水平方向靠左
    GRF_SCROLL_RIGHT_BOTTOM,             //滚动后垂直方向靠底，水平方向靠右
    GRF_SCROLL_CENTER,                  //滚动后居中
} grf_scroll_dir_e;

typedef enum {
    GRF_STATE_DEFAULT     =  0x0000,         //正常状态
    GRF_STATE_CHECKED     =  0x0001,         //切换或者选中
    GRF_STATE_FOCUSED     =  0x0002,         //点击
    GRF_STATE_FOCUS_KEY   =  0x0004,         //聚焦
    GRF_STATE_EDITED      =  0x0008,         //编辑
    GRF_STATE_PRESSED     =  0x0020,         //已按下
    GRF_STATE_SCROLLED    =  0x0040,         //滚动状态
    GRF_STATE_DISABLED    =  0x0080,         //禁用
}grf_obj_state_e;
typedef enum 
{
    GRF_DROP_DIR_NONE     = 0x00,
    GRF_DROP_DIR_LEFT     = 0x01,        //向左展开
    GRF_DROP_DIR_RIGHT    = 0x02,        //向右展开
    GRF_DROP_DIR_TOP      = 0x04,        //向上展开
    GRF_DROP_DIR_BOTTOM   = 0x08,        //向下展开
}grf_droplist_dir_e;
typedef enum 
{
    GRF_ALIGN_DEFAULT = 0,
    GRF_ALIGN_IN_TOP_LEFT,                  // 在控件内部顶端靠左对齐
    GRF_ALIGN_IN_TOP_MID,                   // 在控件内部顶端中间对齐
    GRF_ALIGN_IN_TOP_RIGHT,                 // 在控件内部顶端靠右对齐
    GRF_ALIGN_IN_BOTTOM_LEFT,               // 在控件内部底端靠左对齐
    GRF_ALIGN_IN_BOTTOM_MID,                // 在控件内部底端中间对齐
    GRF_ALIGN_IN_BOTTOM_RIGHT,              // 在控件内部底端靠右对齐
    GRF_ALIGN_IN_LEFT_MID,                  // 在控件内部靠左居中对齐
    GRF_ALIGN_IN_RIGHT_MID,                 // 在控件内部靠右居中对齐
    GRF_ALIGN_CENTER ,                      // 居中对齐
    GRF_ALIGN_OUT_TOP_LEFT,                 // 在控件外部顶端靠左对齐
    GRF_ALIGN_OUT_TOP_MID,                  // 在控件外部顶端中间对齐
    GRF_ALIGN_OUT_TOP_RIGHT,                // 在控件外部顶端靠右对齐
    GRF_ALIGN_OUT_BOTTOM_LEFT,              // 在控件外部底端靠左对齐
    GRF_ALIGN_OUT_BOTTOM_MID,               // 在控件外部底端中间对齐
    GRF_ALIGN_OUT_BOTTOM_RIGHT,             // 在控件外部底端靠右对齐
    GRF_ALIGN_OUT_LEFT_TOP,                 // 在控件外部靠左顶部对齐
    GRF_ALIGN_OUT_LEFT_MID,                 // 在控件外部靠左居中对齐
    GRF_ALIGN_OUT_LEFT_BOTTOM,              // 在控件外部靠左底部对齐
    GRF_ALIGN_OUT_RIGHT_TOP,                // 在控件外部靠右顶部对齐
    GRF_ALIGN_OUT_RIGHT_MID,                // 在控件外部靠右居中对齐
    GRF_ALIGN_OUT_RIGHT_BOTTOM,             // 在控件外部靠右底部对齐
                                           
}grf_align_e;
typedef enum 
{
    GRF_GESTURE_DIR_NONE     = 0x00,        //无
    GRF_GESTURE_DIR_LEFT     = (1 << 0),    //向左滚动  0001
    GRF_GESTURE_DIR_RIGHT    = (1 << 1),    //向右滚动  0010
    GRF_GESTURE_DIR_TOP      = (1 << 2),    //向上滚动  0100
    GRF_GESTURE_DIR_BOTTOM   = (1 << 3),    //向下滚动  1000
    GRF_GESTURE_DIR_HOR      = GRF_GESTURE_DIR_LEFT | GRF_GESTURE_DIR_RIGHT,    //水平滚动  0011
    GRF_GESTURE_DIR_VER      = GRF_GESTURE_DIR_TOP | GRF_GESTURE_DIR_BOTTOM,    //垂直滚动  1100
    GRF_GESTURE_DIR_ALL      = GRF_GESTURE_DIR_HOR | GRF_GESTURE_DIR_VER,       //任意方向  1111
}grf_dir_gesture_e;
s32 grf_ctrl_set_gesture_bubble(grf_ctrl_t* ctrl_t,grf_bool gesture);
grf_dir_gesture_e grf_ctrl_gesture_check();


//根据编号获取控件
grf_ctrl_t* grf_ctrl_get_form_id(u32 view_id,u32 ctrl_id);
#define GCL(vid,cid) grf_ctrl_get_form_id(vid,cid)
//根据控件获取编号
s32 grf_ctrl_get_id_form_ctrl(grf_ctrl_t *ctrl_t);
//显示控件 ,如果控件被删除,将重新创建,如果控件被隐藏,将显示出来
grf_ctrl_t* grf_ctrl_show(u32 view_id,u32 ctrl_id);
//设置控件显示位置
s32 grf_ctrl_set_pos(grf_ctrl_t* ctrl_t,grf_coord_t x,grf_coord_t y);  
//设置控件大小
s32 grf_ctrl_set_size(grf_ctrl_t* ctrl_t,grf_coord_t w,grf_coord_t h);
//获取控件宽度
s32 grf_ctrl_get_width(grf_ctrl_t* ctrl_t);
//获取控件高度
s32 grf_ctrl_get_height(grf_ctrl_t* ctrl_t);
//旋转控件.(x,y)相对于控件左上角的偏移位置,angel-旋转角度
s32 grf_ctrl_rotate_angel(grf_ctrl_t* ctrl_t, grf_coord_t x, grf_coord_t y, u16 angel);
//设置是否隐藏
s32 grf_ctrl_set_hidden(grf_ctrl_t* ctrl_t,grf_bool hidden);       
//获取是否隐藏
grf_bool grf_ctrl_get_hidden(grf_ctrl_t* ctrl_t);
//设置透明度
s32 grf_ctrl_set_alpha(grf_ctrl_t* ctrl_t,u8 scale);       
//设置点击模式
s32 grf_ctrl_set_touch(grf_ctrl_t* ctrl_t,grf_touch_mode_e t_mode);   
//设置 扩大控件触摸点击范围
s32 grf_ctrl_set_ext_click_area(grf_ctrl_t* ctrl_t,grf_coord_t size);    
//设置控件之间的相对位置
s32 grf_ctrl_set_align(grf_ctrl_t* ctrl_t,grf_ctrl_t* base_ctrl,grf_align_e align,grf_coord_t x_offset,grf_coord_t y_offset); 
//设置控件前置
s32 grf_ctrl_move_forground(grf_ctrl_t* ctrl_t);     
//设置控件后置
s32 grf_ctrl_move_background(grf_ctrl_t* ctrl_t);    
//获取控件的子控件数量
u32 grf_ctrl_get_child_sum(grf_ctrl_t* ctrl_t);
//获取控件的指定子控件
grf_ctrl_t *grf_ctrl_get_child(grf_ctrl_t* ctrl_t,u32 child_id);
//获取控件所在区域
s32 grf_ctrl_get_coords(grf_ctrl_t* ctrl_t,grf_area_t *grf_coords); 
//设置控件滑动方向
s32 grf_ctrl_set_scroll_dir(grf_ctrl_t* ctrl_t,grf_dir_gesture_e dir);
//设置控件滚动后子对象对齐位置（垂直方向）
s32 grf_ctrl_set_scroll_dir_y(grf_ctrl_t* ctrl_t,grf_scroll_dir_e scroll_dir);
//设置控件滚动后子对象对齐位置（水平方向）
s32 grf_ctrl_set_scroll_dir_x(grf_ctrl_t* ctrl_t,grf_scroll_dir_e scroll_dir);
//将子控件滚动到父控件可见位置（具体位置可由grf_scroll_dir_e设置）
s32 grf_ctrl_scroll_view(grf_ctrl_t* ctrl_t,grf_bool anim);
//当父控件滚动时此控件不滚动
s32 grf_ctrl_child_not_scroll(grf_ctrl_t* ctrl_t);
//当父控件滚动时判断该子控件是否同步滚动
grf_bool grf_ctrl_child_get_scroll(grf_ctrl_t* ctrl_t);
//获取对象y坐标
s32 grf_ctrl_get_y(grf_ctrl_t* ctrl_t);
//获取对象x坐标
s32 grf_ctrl_get_x(grf_ctrl_t* ctrl_t);
//刷新控件位置
s32 grf_ctrl_update_location(grf_ctrl_t* ctrl_t);
//强制刷新控件
s32 grf_ctrl_force_refresh(grf_ctrl_t* ctrl_t);
//控件添加状态
s32 grf_ctrl_add_state(grf_ctrl_t* ctrl_t, grf_obj_state_e state);
//控件清除状态
s32 grf_ctrl_clear_state(grf_ctrl_t* ctrl_t, grf_obj_state_e state);
//获取控件状态
s32 grf_ctrl_get_state(grf_ctrl_t* ctrl_t);
//改变控件x坐标（默认状态）
s32 grf_ctrl_translate_x(grf_ctrl_t* ctrl_t,grf_coord_t x);
//改变控件y坐标（默认状态）
s32 grf_ctrl_translate_y(grf_ctrl_t* ctrl_t,grf_coord_t y);
//设置控件x坐标
s32 grf_ctrl_set_x(grf_ctrl_t* ctrl_t,grf_coord_t x);
//设置控件y坐标
s32 grf_ctrl_set_y(grf_ctrl_t* ctrl_t,grf_coord_t y);
//计算算术平方根(输入x(正数)，输出res)
s32 grf_square_root(u32 x,grf_sqrt_t *res);
//根据x值匹配输入与输出值映射,输出x对应的值
u32 grf_map_relation(u32 x,u32 min_in,u32 max_in,u32 min_out,u32 max_out);
//设置控件缩放（128一半，256正常，512一倍）(默认状态)
s32 grf_ctrl_zoom_set(grf_ctrl_t* ctrl_t,u32 num);
//设置控件缩放中心(默认状态)
s32 grf_ctrl_zoom_pivot(grf_ctrl_t* ctrl_t,u32 x,u32 y);
//判断控件是否处于当前状态
grf_bool grf_ctrl_state_judge(grf_ctrl_t* ctrl_t,grf_obj_state_e state);
//获取控件x轴滚动量
s32 grf_ctrl_scroll_get_x(grf_ctrl_t* ctrl_t);
//获取控件y轴滚动量
s32 grf_ctrl_scroll_get_y(grf_ctrl_t* ctrl_t);
//设置控件的滚动量，可设置动画
s32 grf_ctrl_scroll_set(grf_ctrl_t* ctrl_t,s32 x,s32 y,grf_bool anim);
//隐藏子控件超出父控件部分  
s32 grf_ctrl_set_child_clip_corner(grf_ctrl_t* ctrl_t,grf_bool hidden);
//设置按下锁定
s32 grf_ctrl_set_presslock(grf_ctrl_t* ctrl_t,grf_bool presslock);
//设置全控件进入的事件函数
typedef void (*grf_ctrl_event_all_fun)(s32 view_id,s32 ctrl_id,u32 event);
void grf_ctrl_set_event_all(grf_ctrl_event_all_fun *event_fun);


/*********************************set style  设置样式************************************/
//设置背景颜色
s32 grf_ctrl_style_set_bg_color(grf_ctrl_t* ctrl,grf_color_t color_t,u32 part);	
//设置渐变背景颜色		
s32 grf_ctrl_style_set_bg_grad_color(grf_ctrl_t* ctrl,grf_color_t color_t,u32 part);	
typedef enum {
	GRF_GRAD_DIR_NONE,
	GRF_GRAD_DIR_VER,
	GRF_GRAD_DIR_HOR,
}grf_grad_dir_e;
//设置渐变方向
s32 grf_ctrl_style_set_bg_grad_dir(grf_ctrl_t* ctrl,grf_grad_dir_e grad_dir, u32 part);
//设置背景透明度
s32 grf_ctrl_style_set_bg_opa(grf_ctrl_t* ctrl,u8 opa,u32 part);
//设置圆角(radius = -1全圆)
s32 grf_ctrl_style_set_radius(grf_ctrl_t* ctrl,s32 radius,u32 part);					
//设置边框颜色
s32 grf_ctrl_style_set_border_color(grf_ctrl_t* ctrl,grf_color_t color_t,u32 part);		
//设置边框宽度
s32 grf_ctrl_style_set_border_width(grf_ctrl_t* ctrl,u16 width,u32 part);
//设置边框透明度	
s32 grf_ctrl_style_set_border_opa(grf_ctrl_t* ctrl,u8 opa,u32 part);		
typedef enum {
	GRF_BORDER_SIDE_NONE     = 0x00,
	GRF_BORDER_SIDE_BOTTOM   = 0x01,
	GRF_BORDER_SIDE_TOP      = 0x02,
	GRF_BORDER_SIDE_LEFT     = 0x04,
	GRF_BORDER_SIDE_RIGHT    = 0x08,
	GRF_BORDER_SIDE_FULL     = 0x0F,
	GRF_BORDER_SIDE_INTERNAL = 0x10,
}grf_border_side_e;
//设置边框显示位置
s32 grf_ctrl_style_set_border_side(grf_ctrl_t* ctrl,grf_border_side_e border_side,u32 part);
//设置阴影颜色
s32 grf_ctrl_style_set_shadow_color(grf_ctrl_t* ctrl,grf_color_t color_t,u32 part);	
//设置阴影宽度	
s32 grf_ctrl_style_set_shadow_width(grf_ctrl_t* ctrl,u16 width,u32 part);		
//设置边距
s32 grf_ctrl_style_set_pad(grf_ctrl_t* ctrl,u16 pad_top,u16 pad_bottom,u16 pad_left,u16 pad_right,u32 part);		
//设置文本颜色
s32 grf_ctrl_style_set_text_color(grf_ctrl_t* ctrl,grf_color_t color_t,u32 part);	
//设置文本透明度	
s32 grf_ctrl_style_set_text_opa(grf_ctrl_t* ctrl,u8 opa,u32 part);		
//设置文本字体
s32 grf_ctrl_style_set_text(grf_ctrl_t* ctrl,char* font,u16 size,u32 part);    
//设置文本间距 letter_space-字符间距 line_space-行间距
s32 grf_ctrl_style_set_text_space(grf_ctrl_t* ctrl,u16 letter_space,u16 line_space,u32 part);
//设置图片资源
s32 grf_ctrl_style_set_bg_img(grf_ctrl_t* ctrl,char* img,u32 part);             
/*********************************get style  获取样式************************************/
//获取背景颜色
grf_color_t grf_ctrl_style_get_bg_color(grf_ctrl_t* ctrl,u32 part);    
//获取渐变背景颜色         
grf_color_t grf_ctrl_style_get_bg_grad_color(grf_ctrl_t* ctrl,u32 part);	
//获取渐变方向    
grf_grad_dir_e grf_ctrl_style_get_bg_grad_dir(grf_ctrl_t* ctrl, u32 part);
//获取背景透明度      
u8 grf_ctrl_style_get_bg_opa(grf_ctrl_t* ctrl,u32 part);                    
//获取圆角(radius = -1全圆)    
s32 grf_ctrl_style_get_radius(grf_ctrl_t* ctrl,u32 part);          
//获取边框颜色             
grf_color_t grf_ctrl_style_get_border_color(grf_ctrl_t* ctrl,u32 part);    
//获取边框宽度     
u16 grf_ctrl_style_get_border_width(grf_ctrl_t* ctrl,u32 part);       
//获取边框透明度          
u8 grf_ctrl_style_get_border_opa(grf_ctrl_t* ctrl,u32 part);              
//获取边框显示位置      
grf_border_side_e grf_ctrl_style_get_border_side(grf_ctrl_t* ctrl,u32 part);
//获取阴影颜色  
grf_color_t grf_ctrl_style_get_shadow_color(grf_ctrl_t* ctrl,u32 part);         
//获取阴影宽度
u16 grf_ctrl_style_get_shadow_width(grf_ctrl_t* ctrl,u32 part);
//获取边距
s32 grf_ctrl_style_get_pad(grf_ctrl_t* ctrl,u16 *pad_top,u16 *pad_bottom,u16 *pad_left,u16 *pad_right,u32 part);
//获取文本颜色
grf_color_t grf_ctrl_style_get_text_color(grf_ctrl_t* ctrl,u32 part);
//获取文本透明度
u8 grf_ctrl_style_get_text_opa(grf_ctrl_t* ctrl,u32 part);
/*********************************event 事件************************************/
typedef enum {
    GRF_EVENT_PRESSED=1,                        //控件被按下
    GRF_EVENT_PRESSING,                         //控件被按住时连续发送
    GRF_EVENT_PRESS_LOST,                       //控件按住时手指滑出控件
    GRF_EVENT_SHORT_CLICKED,                    //控件短暂按下后松开
    GRF_EVENT_LONG_PRESSED,                     //控件长按后发送一次
    GRF_EVENT_LONG_PRESSED_REPEAT,              //控件长按后连续发送
    GRF_EVENT_CLICKED,                          //控件按下后松开
    GRF_EVENT_RELEASED,                         //控件触摸释放
    GRF_EVENT_SCROLL_BEGIN,                     //控件滚动开始
    GRF_EVENT_SCROLL_END,                       //控件滚动结束
    GRF_EVENT_SCROLL,                           //检测到滚动
    GRF_EVENT_VALUE_CHANGED=28,                 //控件值改变触发
	GRF_EVENT_SCROLL_END_INDEV=46,				//控件触摸释放滚动结束
	GRF_EVENT_SCROLL_END_ANIM,					//控件动画滚动结束
	GRF_EVENT_SCROLL_END_ANIM_OFF,				//控件无动画滚动结束
}grf_event_e;
//手动发送事件(返回0：控件在事件中被删除，返回1，未被删除)
s32 grf_event_send(grf_ctrl_t* ctrl_t, grf_event_e event, void * data);
/*********************************动画************************************/
typedef enum {
    GRF_ANIM_PATH_NONE=0,
    GRF_ANIM_PATH_LINEAR,               //线性动画
    GRF_ANIM_PATH_START_SLOW,           //开始慢
    GRF_ANIM_PATH_END_SLOW,             //结尾慢
    GRF_ANIM_PATH_START_END_SLOW,       //开始与结尾均慢
	GRF_ANIM_PATH_START_END_FAST,		//开始与结尾均快，中间慢
    GRF_ANIM_PATH_BOUNCE,               //触墙弹跳效果
    GRF_ANIM_PATH_END_BREAK,            //开始不变，结尾突变
    GRF_ANIM_PATH_OVERSHOOT             //超出终点并拉回
}grf_anim_path_e;
typedef struct
{
	u32 time;                   //动画时长
	u32 delay;					//延时时长
	u32 back_time;              //返回动画时长(设置后动画按原路径反向运行还原，若不需要返回动画此项与下一项均设置0)
	u32 back_time_delay;        //返回动画延时
	u32 repeat_count;			//动画重复次数
	u32 repeat_delay;			//动画重复延时
	s32 value_start_a;          //动画开始值
	s32 value_end_a;            //动画结束值
	void *anim_cb_a;            //动画回调函数
	void *anim_start_cb;        //动画开始时调用
	void *anim_reday_cb;        //动画结束后调用（不包含返回动画时间）
	grf_anim_path_e mode;       //动画运行效果
}grf_anim_set_t;
s32 grf_animation_set(grf_ctrl_t* ctrl,grf_anim_set_t *anim_t);
//删除某个控件的某个动画
grf_bool grf_animation_del(grf_ctrl_t* ctrl, void *exec_cb);
//获取正在运行的动画数量
u16 grf_animation_count_running();
//删除全部动画
s32 grf_animation_del_all();

/*********************************cont 容器控件************************************/
typedef enum
{
    GRF_CONT_PART_MAIN,
}grf_cont_style_part;
/*********************************label 文本控件************************************/

typedef enum
{
    GRF_LABEL_PART_MAIN,
}grf_label_style_part;
typedef enum 
{
    GRF_LABEL_DIS_BREAK,        //控件宽不变,超过区域换行,改变高度
    GRF_LABEL_DIS_DOT,          //控件宽高不变,文本超过区域,显示...
    GRF_LABEL_DIS_SROLL,        //控件宽高不变,文本超过区域,自动循环向前向后滚动文本
    GRF_LABEL_DIS_SROLL_CIRC,   //控件宽高不变,文本超过区域,自动循环环形滚动文本
    GRF_LABEL_DIS_CROP,         //控件宽高不变,超过的文本内容将会被剪切掉
}grf_label_dis_mode_e;
typedef enum 
{
    GRF_LABEL_ALIGN_HOR_LEFT = 0x01,    //文本内部水平方向左对齐
    GRF_LABEL_ALIGN_HOR_MID,      //文本内部水平方向居中对齐
    GRF_LABEL_ALIGN_HOR_RIGHT,       //文本内部水平方向右对齐


	GRF_LABEL_ALIGN_VER_TOP = 0x10,       //文本内部竖直方向上对齐
	GRF_LABEL_ALIGN_VER_MID = 0x20,       //文本内部竖直方向居中对齐
	GRF_LABEL_ALIGN_VER_BOTTOM = 0x30,       //文本内部竖直方向下对齐
}grf_label_align_e;
//获取文本内容
char *grf_label_get_text(grf_ctrl_t* label);
//设置显示文本
s32 grf_label_set_txt(grf_ctrl_t* label,const char* txt);
//设置文字颜色
s32 grf_label_set_txt_color(grf_ctrl_t* label,grf_color_t color_t);
//设置文字大小
s32 grf_label_set_txt_size(grf_ctrl_t* label,u32 font_size);
//设置对齐方式
s32 grf_label_set_align(grf_ctrl_t* label,grf_label_align_e align_e);
//设置显示模式
s32 grf_label_set_dis_mode(grf_ctrl_t* label,grf_label_dis_mode_e dis_mode);
//设置长文本滚动速度
s32 grf_label_set_anim_speed(grf_ctrl_t* label,u16 speed);

/*********************************button 按钮控件************************************/
typedef enum
{
    GRF_BTN_RELEASE,
	GRF_BTN_CHECKED = 0x0001,
	GRF_BTN_PRESSED = 0x0020,
	GRF_BTN_DISABLED = 0x0080,
}grf_btn_style_part;
//设置按钮文字
s32 grf_btn_set_txt(grf_ctrl_t* btn,const char* txt);
//设置按钮文本颜色txt_state文本状态
s32 grf_btn_set_txt_color(grf_ctrl_t* btn,grf_color_t color_t,grf_obj_state_e txt_state);
//设置按钮文字大小
s32 grf_btn_set_txt_size(grf_ctrl_t* btn,u32 font_size);
//设置按钮显示模式  0-按钮模式 1-切换模式
s32 grf_btn_set_mode(grf_ctrl_t* btn,u8 mode);
//获取按钮状态
u32 grf_btn_get_state(grf_ctrl_t* btn);
//设置按钮状态
s32 grf_btn_set_state(grf_ctrl_t* btn,grf_obj_state_e state);
/*********************************image 图片控件************************************/
//图片覆盖颜色和透明度
s32 grf_img_recolor(grf_ctrl_t *img,grf_color_t color,u8 opa);
//修改显示图片
s32 grf_img_set_src(grf_ctrl_t * img,char* img_src);
//获取显示图片
char* grf_img_get_src(grf_ctrl_t * img);
//设置旋转角度(顺时针 0-3600 单位0.1度)
s32 grf_img_set_rotate_angel(grf_ctrl_t *img,s16 angel);
//设置旋转或缩放中心
s32 grf_img_set_pivot(grf_ctrl_t *img,grf_coord_t x,grf_coord_t y);
//设置图片缩放128缩小一倍，256原始大小，512放大一倍
s32 grf_img_zoom_setting(grf_ctrl_t *img,u16 size);
//获取图片旋转角度
u16 grf_img_get_rotate_angel(grf_ctrl_t *img);
/*********************************image button 图片按钮控件************************************/

typedef enum {
    GRF_IMGBTN_STATE_REL,       //按钮的正常释放状态
    GRF_IMGBTN_STATE_PR,        //按钮的正常按下状态
    GRF_IMGBTN_STATE_DISABLED,          //正常禁用
    GRF_IMGBTN_STATE_CHECKED_RELEASED,    //按钮的切换(Toggle)释放状态
    GRF_IMGBTN_STATE_CHECKED_PRESSED,      //按钮的切换(Toggle)按下状态
    GRF_IMGBTN_STATE_CHECKED_DISABLED,       //切换的禁用的状态
    GRF_LV_IMGBTN_STATE_NUM,
}grf_imgbtn_state_e;
typedef enum
{
    GRF_IMGBTN_RELEASE,
	GRF_IMGBTN_CHECKED_RELEASED = 0x0001,
	GRF_IMGBTN_PRESSED = 0x0020,
	GRF_IMGBTN_CHECKED_PRESSED = 0x0021,
	GRF_IMGBTN_DISABLED = 0x0080,
	GRF_IMGBTN_CHECKED_DISABLED = 0x0081,
}grf_imgbtn_style_part;
//设置图片按钮文字
s32 grf_imgbtn_set_txt(grf_ctrl_t* imgbtn,const char* txt);
//设置图片按钮文字颜色
s32 grf_imgbtn_set_txt_color(grf_ctrl_t* imgbtn,grf_color_t color_t,grf_imgbtn_state_e txt_state);
//设置图片按钮文字大小
s32 grf_imgbtn_set_txt_size(grf_ctrl_t* imgbtn,u32 font_size);
//手动设置图片按钮状态(根据图片按钮事件手动设置状态)
s32 grf_imgbtn_set_mode(grf_ctrl_t* imgbtn,grf_imgbtn_state_e mode);
//设置图片按钮不同状态的显示图片
s32 grf_imgbtn_set_image(grf_ctrl_t* imgbtn,grf_imgbtn_state_e state,char* img_src);
/*********************************tileview 控件************************************/
typedef enum
{
    GRF_TILEVIEW_PART_MAIN,
	GRF_TILEVIEW_PART_SCROLLBAR = 0x010000,
}grf_tileview_style_part;
//根据id手动切换tile
s32 grf_tileview_set_tile_id(grf_ctrl_t* tileview,u32 row_id,u32 col_id,grf_bool anim_en);
//获取当前tile id
s32 grf_tileview_get_tile_id(grf_ctrl_t* tileview,u32 *row_id,u32 *col_id);
//获取当前显示的tile对象
grf_ctrl_t* grf_tileview_get_tile_act(grf_ctrl_t* tileview);
/*********************************line 线条控件************************************/
typedef struct
{
    grf_color_t color;      //线条颜色
    grf_coord_t width;      //线条宽度
    u8 opa;                 //透明度
    u8 rounded;             //圆角
} grf_line_disp_t;//线条
//设置线条显示样式
s32 grf_line_set_dis(grf_ctrl_t* line,grf_line_disp_t disp_t);
//设置线条点,可以多点折线显示
s32 grf_line_set_points(grf_ctrl_t* line, grf_point_t point_a[], u16 point_num);
//设置线条为虚线的比例(仅限横/竖)
s32 grf_line_set_dash(grf_ctrl_t* line,u16 dash_width,u16 dash_gap);
/*********************************arc 弧形控件************************************/
//         270°
//     180° +  0°
//          90°
//设置弧形显示样式 part:0-背景,1-前景
s32 grf_arc_set_dis(grf_ctrl_t* arc,u8 part,grf_line_disp_t dis_t);
//设置滑块颜色
s32 grf_arc_set_knob(grf_ctrl_t* arc,grf_color_t color_t);
//设置值范围
s32 grf_arc_set_value_range(grf_ctrl_t* arc,s16 minvalue,s16 maxvalue);
//设置值
s32 grf_arc_set_value(grf_ctrl_t* arc,s16 value);
//得到值
s32 grf_arc_get_value(grf_ctrl_t* arc);
//设置旋转
s32 grf_arc_set_rotation(grf_ctrl_t* arc,u16 angle);
//设置起始角度和终止角度
s32 grf_arc_set_angles(grf_ctrl_t* arc,u16 start,u16 end);
//设置线条宽度(0-背景，1-前景)
s32 grf_arc_set_width(grf_ctrl_t* arc,u16 width,u8 part);

/*********************************bar 进度条控件************************************/
typedef enum
{
    GRF_BAR_PART_MAIN,
	GRF_BAR_PART_INDICATOR = 0x020000,
}grf_bar_style_part;
//设置进度条的颜色
s32 grf_bar_set_bar_color(grf_ctrl_t* bar,grf_color_t color_t);
//设置进度值
s32 grf_bar_set_value(grf_ctrl_t* bar,s16 value);
//设置进度范围
s32 grf_bar_set_range(grf_ctrl_t* bar,s16 min,s16 max);
//获得进度值
s32 grf_bar_get_value(grf_ctrl_t* bar);

/*********************************slider 滑块进度条控件************************************/
typedef enum
{
    GRF_SLIDER_PART_MAIN,
	GRF_SLIDER_PART_INDICATOR = 0x020000,
	GRF_SLIDER_PART_KNOB = 0x030000,
}grf_slider_style_part;
//设置进度条的颜色
s32 grf_slider_set_bar_color(grf_ctrl_t* slider,grf_color_t color_t);
//设置进度值
s32 grf_slider_set_value(grf_ctrl_t* slider,s16 value);
//设置进度范围
s32 grf_slider_set_range(grf_ctrl_t* slider,s16 min,s16 max);
//获得进度值
s32 grf_slider_get_value(grf_ctrl_t* slider);

/*********************************sw 开关控件************************************/
typedef enum
{
    GRF_SW_PART_MAIN,
	GRF_SW_PART_INDICATOR = 0x020001,
	GRF_SW_PART_KNOB_OFF = 0x030000,
	GRF_SW_PART_KNOB_ON = 0x030001,
}grf_sw_style_part;
//设置开关状态
s32 grf_sw_set_state(grf_ctrl_t* sw,grf_bool state);
//获取开关状态
grf_bool grf_sw_get_state(grf_ctrl_t* sw);

/*********************************cb 复选框控件************************************/
typedef enum
{
    GRF_CB_PART_MAIN,
	GRF_CB_PART_KNOB_OFF = 0x020000,
	GRF_CB_PART_KNOB_ON = 0x020001,
	GRF_CB_PART_KNOB_DISABLED = 0x020080,
}grf_cb_style_part;
//设置显示的文字
s32 grf_cb_set_txt(grf_ctrl_t* cb, const char * txt);
//设置是否被选中
s32 grf_cb_set_checked(grf_ctrl_t* cb, grf_bool checked);
//判断是否被选中
grf_bool grf_cb_is_checked(grf_ctrl_t* cb);
//设置是否禁用控件
s32 grf_cb_set_disable(grf_ctrl_t* cb,grf_bool checked);
//判断是否被禁用
grf_bool grf_cb_is_inactive(grf_ctrl_t* cb);
/*********************************mbox 消息提示控件************************************/
typedef enum
{
    GRF_MBOX_PART_MAIN,
	GRF_MBOX_PART_BTN_RELEASED = 0x050000,
	GRF_MBOX_PART_BTN_PRESSED = 0x050020,
}grf_mbox_style_part;
//设置标题显示内容
s32 grf_title_set_txt(grf_ctrl_t* mbox, const char * txt);
//设置消息显示内容
s32 grf_mbox_set_txt(grf_ctrl_t* mbox, const char * txt);
//获得被点击的按钮序号
u16 grf_mbox_get_clicked_btn_num(grf_ctrl_t* mbox);
//设置延时自动关闭时间
s32 grf_mbox_start_auto_close(grf_ctrl_t* mbox,u16 delay_time);
//关闭弹窗
s32 grf_mbox_s_close(grf_ctrl_t* mbox);
//打开弹窗
s32 grf_mbox_s_open(grf_ctrl_t* mbox);
/*********************************txtbox 文本框控件************************************/
typedef enum
{
    GRF_TXTBOX_PART_MAIN,
	GRF_TXTBOX_PART_SCROLLBAR = 0x010000,
	GRF_TXTBOX_PART_PLACEHOLDER_TEXT = 0x080000,
}grf_txtbox_style_part;
typedef enum 
{
    GRF_CURSOR_RIGHT,       //光标向右移动一步
    GRF_CURSOR_LEFT,        //光标向左移动一步
    GRF_CURSOR_DOWN,        //光标向下移动一步,文本域中有多行文本时才起作用
    GRF_CURSOR_UP,          //光标向上移动一步,文本域中有多行文本时才起作用
}grf_cursor_move_e;
//添加字符串
s32 grf_txtbox_add_txt(grf_ctrl_t* txtbox,const char * txt);
//添加单个字符
s32 grf_txtbox_add_char(grf_ctrl_t* txtbox,u32 txt);
//删除光标左侧一个字符
s32 grf_txtbox_del_char(grf_ctrl_t* txtbox);
//删除光标右侧一个字符
s32 grf_txtbox_del_char_forward(grf_ctrl_t* txtbox);
//设置文本内容
s32 grf_txtbox_set_text(grf_ctrl_t* txtbox, const char * txt);
//获取文本内容
char* grf_txtbox_get_text(grf_ctrl_t* txtbox);
//切换文本密码显示模式
s32 grf_txtbox_pwd_mode(grf_ctrl_t* txtbox,grf_bool en);
//光标移动一步
s32 grf_txtbox_cursor_move(grf_ctrl_t* txtbox,grf_cursor_move_e move_dir);
/********************************list 列表控件************************************/
typedef enum
{
    GRF_LIST_PART_MAIN,
	GRF_LIST_PART_SCROLLBAR = 0x010000,
}grf_list_style_part;
typedef enum
{
    GRF_LIST_BTN_PART_MAIN,
	GRF_LIST_BTN_PART_PRESSED = 0x20,
	GRF_LIST_BTN_PART_DISABLED = 0x80,
}grf_list_btn_style_part;
//添加按钮
grf_ctrl_t* grf_list_add_btn(grf_ctrl_t* list,char* img_src, const char *txt);
//删除某个按钮
s32 grf_list_del_btn(grf_ctrl_t* list,u16 btn_num);
//删除全部按钮
s32 grf_list_remove_all(grf_ctrl_t* list);
//得到列表的指定子对象
grf_ctrl_t* grf_list_get_index_btn(grf_ctrl_t* list,u16 btnNum);
//得到按钮的选中项id
u32 grf_list_get_select_index(grf_ctrl_t* ctrl);
//设置指定按钮的文本
s32 grf_list_set_index_str(grf_ctrl_t* list,u16 index,const char *txt);
//得到指定按钮的文本
const char* grf_list_get_index_str(grf_ctrl_t* list,u16 index);
//得到指定按钮的图片对象
//grf_ctrl_t* grf_list_get_index_img(grf_ctrl_t* list,u16 index);
//得到指定按钮的标签对象
grf_ctrl_t* grf_list_get_index_label(grf_ctrl_t* list,u16 index);
//将某列表项按钮处于聚焦状态
s32 grf_list_focus(grf_ctrl_t* list,u16 index);
/********************************droplist 下拉列表控件************************************/
typedef enum
{
    GRF_DROPLIST_PART_MAIN,
	GRF_DROPLIST_PART_SCROLLBAR = 0x010000,
	GRF_DROPLIST_PART_SELECTED = 0x040000,
	GRF_DROPLIST_PART_SELECTED_PRESSED = 0x040001,
}grf_droplist_style_part;
typedef enum
{
    GRF_DROPLIST_OPTION_PART_MAIN,
}grf_droplist_option_style_part;
//增加下拉菜单的某一项
s32 grf_droplist_add_option(grf_ctrl_t* droplist, const char * option, u16 index);
//修改已有选项的文本
s32 grf_droplist_modify_option(grf_ctrl_t* droplist,const char *options,u16 index);
//删除某一项
s32 grf_droplist_del_option(grf_ctrl_t* droplist,u16 index);
//设置所有下拉菜单项
s32 grf_droplist_set_options(grf_ctrl_t* droplist, const char * options[],u16 options_num);
//选中某个选项
s32 grf_droplist_set_selected(grf_ctrl_t* droplist,u16 sel_opt);
//获取选中项的编号
u16 grf_droplist_get_selected(grf_ctrl_t* droplist);
//获取被选中项的内容
s32 grf_droplist_get_selected_str(grf_ctrl_t* droplist,char* buf,u16 bufsize);
//将下拉框展开
s32 grf_droplist_open(grf_ctrl_t* droplist);
//将下拉框收缩
s32 grf_droplist_close(grf_ctrl_t* droplist);
/********************************rollist 滚轮列表控件************************************/
typedef enum
{
    GRF_ROLLIST_PART_MAIN,
	GRF_ROLLIST_PART_SELECTED = 0x040000,
}grf_rollist_style_part;
typedef enum {
    GRF_ROLLER_MODE_NORMAL,          //正常滚动模式
    GRF_ROLLER_MODE_INIFINITE,       //循环滚动模式
}grf_rollist_mode_t;
//增加或者插入某个选项
s32 grf_rollist_add_option(grf_ctrl_t* rollist, const char *options,u16 index);
//修改已有选项的文本
s32 grf_rollist_modify_option(grf_ctrl_t* rollist,const char *options,u16 index);
//删除某个选项
s32 grf_rollist_del_option(grf_ctrl_t* rollist,u16 index);
//设置选择项
s32 grf_rollist_set_options(grf_ctrl_t* rollist, const char *options[],u16 options_num,grf_rollist_mode_t mode);
//选中某个选项
s32 grf_rollist_set_selected(grf_ctrl_t* rollist, u16 sel_opt,grf_bool anim_en);
//获取选中项的编号
s32 grf_rollist_get_selected(grf_ctrl_t* rollist);
//获取被选中项的内容
s32 grf_rollist_get_selected_str(grf_ctrl_t* rollist,char* buf,u16 bufsize);
//设置选项行间距
s32 grf_rollist_set_line_space(grf_ctrl_t* rollist, u16 line_space);

/********************************chart 图表控件************************************/
typedef void  grf_chart_series_t;
typedef enum 
{
    GRF_CHART_AXIS_PRIMARY_Y     = 0x00,        //左轴
    GRF_CHART_AXIS_SECONDARY_Y   = 0x01,        //右轴
    GRF_CHART_AXIS_PRIMARY_X     = 0x02,        //底部轴
    GRF_CHART_AXIS_SECONDARY_X   = 0x04,        //顶部轴
    GRF_CHART_AXIS_LAST
}grf_chart_axis_t;
typedef enum
{
    GRF_CHART_PART_MAIN,
	GRF_CHART_PART_ITEMS = 0x050000,
}grf_chart_style_part;
//添加线条
grf_chart_series_t *grf_chart_add_series(grf_ctrl_t * chart, grf_color_t color,grf_chart_axis_t axis);
//给线条设置点
s32 grf_chart_set_points(grf_ctrl_t* chart,grf_chart_series_t* ser, grf_coord_t y_array[]);
//添加新的数据点
s32 grf_chart_set_next(grf_ctrl_t* chart,grf_chart_series_t* ser,grf_coord_t value);
//当图标类型设置为散点图时使用,设置点的X,Y坐标
s32 grf_chart_set_next_value2(grf_ctrl_t* chart, grf_chart_series_t * ser,grf_coord_t x_value,grf_coord_t y_value);
//刷新图表
s32 grf_chart_refresh(grf_ctrl_t* chart);
//删除线条
s32 grf_chart_series_remove(grf_ctrl_t* chart,grf_chart_series_t * ser);
//设置点和连线宽度
s32 grf_chart_set_line_width(grf_ctrl_t* chart, u16 point_width,u16 line_width);
//获取数组中x轴起点的索引
s32 grf_chart_get_x_start_point(grf_ctrl_t* chart,grf_chart_series_t * ser);
//修改某个数据点的值
s32 grf_chart_set_value_by_id(grf_ctrl_t* chart,grf_chart_series_t * ser, s32 id, s32 value);
//设置x轴缩放(放大显示不全打开滑动使能)
s32 grf_chart_set_zoom_x(grf_ctrl_t* chart,u16 zoom);
//设置y轴缩放(放大显示不全打开滑动使能)
s32 grf_chart_set_zoom_y(grf_ctrl_t* chart,u16 zoom);
//设置数据点数
s32 grf_chart_set_point_count(grf_ctrl_t* chart,u16 cnt);
//设置值范围
s32 grf_chart_set_range(grf_ctrl_t* chart,grf_chart_axis_t axis,grf_coord_t min_value,grf_coord_t max_value);
//根据x轴值值自动设置y轴范围
s32 grf_chart_set_range_auto(grf_ctrl_t* chart, grf_chart_series_t* ser);
//设置坐标轴标尺参数
s32 grf_chart_set_set_axis(grf_ctrl_t* chart,grf_chart_axis_t axis,grf_coord_t major_len, grf_coord_t minor_len,
						grf_coord_t major_cnt, grf_coord_t minor_cnt,  grf_coord_t draw_size,grf_bool label_en);
/********************************table 表格控件************************************/
typedef enum
{
    GRF_TABLE_PART_MAIN,
	GRF_TABLE_PART_ITEMS = 0x050000,
}grf_table_style_part;
//设置单元格内容
s32 grf_table_set_cell_value(grf_ctrl_t* table, u16 row, u16 col, const char* txt);
//获取单元格内容
const char* grf_table_get_cell_value(grf_ctrl_t * table, u16 row, u16 col);
//单独设置某一列表格宽度
s32 grf_table_set_col_width(grf_ctrl_t* table,u16 col, grf_coord_t w);
//合并右侧单元格
s32 grf_table_set_cell_merge_right(grf_ctrl_t* table,u16 row, u16 col);
//表格样式 (表格线条颜色，宽度，透明度，表格内部填充透明度，颜色)
s32 grf_table_set_cell_wrap(grf_ctrl_t * table, grf_color_t line_color_t,u8 table_line_width,u8 table_line_opa, u8 table_opa,grf_color_t color_t);
//设置表格选中颜色 mode-0:单元格 mode-1:行 mode-2:列
s32 grf_table_set_value_changed_color(grf_ctrl_t * table,grf_color_t color,u8 mode,u16 row,u16 col);
//获取单元格背景色
u32 grf_table_get_cell_color(grf_ctrl_t * table, u16 row, u16 col);
//获取表格行数
u16 grf_table_get_row_cnt(grf_ctrl_t * table);
//获取表格列数
u16 grf_table_get_col_cnt(grf_ctrl_t * table);
//获取选中单元格
s32 grf_table_get_selected_cell(grf_ctrl_t* table, u16 * row, u16 * col);
/********************************spinbox 数据框控件************************************/
typedef enum
{
    GRF_SPINBOX_PART_MAIN,
}grf_spinbox_style_part;
//设置数值
s32 grf_spinbox_set_value(grf_ctrl_t* spinbox, s32 value);
//获取当前的值
s32 grf_spinbox_get_value(grf_ctrl_t* spinbox);
//光标右移
s32 grf_spinbox_step_next(grf_ctrl_t* spinbox);
//光标左移
s32 grf_spinbox_step_prev(grf_ctrl_t* spinbox);
//光标位数据增加step
s32 grf_spinbox_increment(grf_ctrl_t* spinbox);
//光标位数据递减step
s32 grf_spinbox_decrement(grf_ctrl_t* spinbox);

/*************************************GIF控件****************************************/
//GIF重置（重新开始播放）
s32 grf_gif_reset_play(grf_ctrl_t *gif);
//暂停gif播放
s32 grf_gif_pause(grf_ctrl_t *gif);
//恢复gif播放
s32 grf_gif_resume(grf_ctrl_t *gif);
//设置播放次数
s32 grf_gif_play_count(grf_ctrl_t *gif,u32 count);
//设置gif图片
s32 grf_gif_set_src(grf_ctrl_t * ctrl_t,char* gif_src);
/*************************************二维码控件****************************************/
//更新二维码数据
s32 grf_qrcode_updata_data(grf_ctrl_t* ctrl_t,char* txt);
/*************************************video控件****************************************/
typedef enum {
    GRF_VIDEO_PLAY,          //开始播放
    GRF_VIDEO_STOP,            //停止播放
    GRF_VIDEO_PAUSE,           //暂停
    GRF_VIDEO_RESUME,          //恢复播放
}grf_player_state_e;
s32 grf_video_set_pos(grf_ctrl_t* ctrl_t,u16 left_x,u16 left_y,u16 width,u16 height);
s32 grf_video_set_src(grf_ctrl_t* ctrl_t,char* video_file);
s32 grf_video_set_state(grf_ctrl_t* ctrl_t,grf_player_state_e state);
grf_player_state_e grf_video_get_state(grf_ctrl_t* ctrl_t);
s32 grf_video_set_volum(grf_ctrl_t* ctrl_t,u8 volum);
s32 grf_video_set_one_replay(grf_ctrl_t* ctrl_t,grf_bool play_en);
/*************************************audio****************************************/
s32 grf_audio_set_src(char* video_file);
s32 grf_audio_set_state(grf_player_state_e state);
grf_player_state_e grf_audio_get_state(void);
s32 grf_audio_set_volum(u8 volum);

/*************************************英文键盘控件****************************************/
typedef enum
{
    GRF_KEYBOARD_PART_MAIN,
	GRF_KEYBOARD_PART_ITEMS = 0x050000,
	GRF_KEYBOARD_PART_ITEMS_PRESSED = 0x050020,
}grf_keyboard_style_part;
//将键盘与文本框匹配（写在txtbox事件函数中，event-GRF_EVENT_FOCUSED）
s32 grf_keyboard_set_txtbox(grf_ctrl_t* keyboard,grf_ctrl_t* txtbox);
//获取键盘活动按钮的编号
s32 grf_keyboard_get_selected_btn(grf_ctrl_t* keyboard);
//通过编号获取键盘按钮的字符串
const char* grf_keyboard_get_btn_text(grf_ctrl_t* keyboard,u16 btn_id);
//中文模式下隐藏中文栏
s32 grf_keyboard_set_panel_hidden(grf_ctrl_t* keyboard,grf_bool hidden);
/*************************************btnmatrix 按钮矩阵控件****************************************/
typedef enum
{
    GRF_BTNMATRIX_PART_MAIN,
	GRF_BTNMATRIX_PART_ITEMS = 0x050000,
	GRF_BTNMATRIX_PART_ITEMS_CHECKED = 0x050001,
	GRF_BTNMATRIX_PART_ITEMS_PRESSED = 0x050020,
	GRF_BTNMATRIX_PART_ITEMS_DISABLED = 0x050080,
}grf_btnmatrix_style_part;
typedef enum
{
	GRF_BTNMATRIX_CTRL_HIDDEN       = 0x0010, //将按钮设为隐藏（隐藏的按钮仍会占用布局中的空间，它们只是不可见或不可单击）
	GRF_BTNMATRIX_CTRL_NO_REPEAT    = 0x0020, //长按按钮时禁用重复
	GRF_BTNMATRIX_CTRL_DISABLED     = 0x0040, //禁用按钮
	GRF_BTNMATRIX_CTRL_CHECKABLE    = 0x0080, //按钮设置为切换模式
	GRF_BTNMATRIX_CTRL_CHECKED      = 0x0100, //按钮设置为切换状态
	GRF_BTNMATRIX_CTRL_CLICK_TRIG   = 0x0200, //启用：在点击时发送 GRF_EVENT_VALUE_CHANGE ；禁用：在按下时发送 GRF_EVENT_VALUE_CHANGE
	GRF_BTNMATRIX_CTRL_POPOVER      = 0x0400, //按下此键时在弹出框中显示按钮标签
}grf_btnmatrix_ctrl_t;
//获取选中的按钮编号
s32 grf_btnmatrix_get_selected_btn(grf_ctrl_t* btnmatrix);
//获取编号按钮的文本
char* grf_btnmatrix_get_btn_text(grf_ctrl_t* btnmatrix,u16 btn_id);
//设置编号按钮控制选项
s32 grf_btnmatrix_set_btn_ctrl(grf_ctrl_t* btnmatrix,u16 btn_id,grf_btnmatrix_ctrl_t ctrl);
//清除编号按钮控制选项
s32 grf_btnmatrix_clear_btn_ctrl(grf_ctrl_t* btnmatrix,u16 btn_id,grf_btnmatrix_ctrl_t ctrl);
//设置全部按钮控制选项
s32 grf_btnmatrix_set_btn_ctrl_all(grf_ctrl_t* btnmatrix,grf_btnmatrix_ctrl_t ctrl);
//清除全部按钮控制选项
s32 grf_btnmatrix_clear_btn_ctrl_all(grf_ctrl_t* btnmatrix,grf_btnmatrix_ctrl_t ctrl);
//判断按钮是否有某个选项
grf_bool grf_btnmatrix_has_btn_ctrl(grf_ctrl_t* btnmatrix, u16 btn_id, grf_btnmatrix_ctrl_t ctrl);
//设置按钮宽度
s32 grf_btnmatrix_set_btn_width(grf_ctrl_t* btnmatrix, u16 btn_id, u8 width);
//设置矩阵中的按钮具有check唯一性
s32 grf_btnmatrix_set_one_checked(grf_ctrl_t* btnmatrix, grf_bool en);
//判断矩阵中的按钮是否具有check唯一性
grf_bool grf_btnmatrix_get_one_checked(grf_ctrl_t* btnmatrix);
//设置矩阵中按钮的文本
s32 grf_btnmatrix_set_btn_txt(grf_ctrl_t* btnmatrix,u16 btn_id, char* txt);
/*************************************animimg 图片动画控件****************************************/
//图片动画重置
s32 grf_animimg_reset_play(grf_ctrl_t *animimg);
//图片动画暂停
s32 grf_animimg_pause(grf_ctrl_t* animimg);
//图片动画恢复播放
s32 grf_animimg_resume(grf_ctrl_t* animimg);
//图片动画设置间隔时间(ms)
s32 grf_animimg_set_frame_rate(grf_ctrl_t* animimg,u32 rate);
//图片动画开启循环播放
s32 grf_animimg_set_repeat_infinite(grf_ctrl_t* animimg,grf_bool open);
//图片动画设置重复次数
s32 grf_animimg_set_repeat_count(grf_ctrl_t* animimg,u16 count);
//图片动画获取重复次数
s32 grf_animimg_get_repeat_count(grf_ctrl_t* animimg);
//图片动画获取播放状态
grf_player_state_e grf_animimg_get_state(grf_ctrl_t* animimg);

/*************************************scrollcont 滑动列表控件****************************************/
typedef enum
{
    GRF_SCROLLCONT_PART_MAIN,
}grf_scrollcont_style_part;
//获取中心显示的对象编号
s32 grf_scrollcont_get_display_index(grf_ctrl_t* scrollcont);

/*************************************sketchpad 画板控件****************************************/
typedef enum
{
    GRF_SKETCHPAD_PART_MAIN,
}grf_sketchpad_style_part;
//设置线条半透明
s32 grf_sketchpad_set_transp(grf_ctrl_t* sketchpad,grf_bool a);
//清空画板
s32 grf_sketchpad_clean_bg(grf_ctrl_t* sketchpad);
//设置线条宽度
s32 grf_sketchpad_set_width(grf_ctrl_t* sketchpad,u32 width);
#endif
