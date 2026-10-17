#include "teloa_display.h"
#include "teloa_pairing.h"
#include "lvgl_theme.h"
#include "lvgl_font.h"
#include <cmath>
#include <cstring>
#include "teloa_pages.h"
#include "teloa_strings.h"

extern const uint8_t standing[] asm("_binary_standing_rgba_start");
extern const uint8_t wink[] asm("_binary_wink_rgba_start");
extern const uint8_t curious[] asm("_binary_curious_rgba_start");
extern const uint8_t thinking[] asm("_binary_thinking_rgba_start");
extern const uint8_t reading[] asm("_binary_reading_rgba_start");
extern const uint8_t celebrating[] asm("_binary_celebrating_rgba_start");
LV_FONT_DECLARE(teloa_cjk_20);
namespace teloa {
TeloaDisplay* TeloaDisplay::current_=nullptr;
namespace {
#if CONFIG_LANGUAGE_EN_US
constexpr auto kName="Teloa",kReview="Review",kBack="Back",kNext="Next",kReject="Reject",kApprove="Hold: OK",kPair="Hold screen to pair",kPending="Waiting for you";
#elif CONFIG_LANGUAGE_ZH_TW
constexpr auto kName="小羅同學",kReview="待確認",kBack="返回",kNext="下一頁",kReject="拒絕",kApprove="長按批准",kPair="長按螢幕確認配對",kPending="等待你確認";
#else
constexpr auto kName="小罗同学",kReview="待确认",kBack="返回",kNext="下一页",kReject="拒绝",kApprove="长按批准",kPair="长按屏幕确认配对",kPending="等待你确认";
#endif
lv_obj_t* Label(lv_obj_t* parent,int x,int y,int width,const char* text){auto label=lv_label_create(parent);lv_obj_set_pos(label,x,y);lv_obj_set_width(label,width);lv_label_set_long_mode(label,LV_LABEL_LONG_WRAP);lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_CENTER,0);lv_label_set_text(label,text);return label;}
void Palette(LvglTheme* theme){theme->text_font()->SetFallback(&teloa_cjk_20);const bool dark=theme->name()=="dark";theme->set_background_color(lv_color_hex(dark?0x20221f:0xfcfcfb));theme->set_text_color(lv_color_hex(dark?0xe7e9e3:0x242723));theme->set_assistant_bubble_color(lv_color_hex(dark?0x283c30:0xeaf3ed));}
bool Readable(const std::string& text,const lv_font_t* font){uint32_t point=0;for(unsigned char byte:text){if((byte&0xc0)==0x80){point=(point<<6)|(byte&63);continue;}if(point&&point!='\n'){lv_font_glyph_dsc_t glyph{};if(!lv_font_get_glyph_dsc(font,&glyph,point,0)||glyph.is_placeholder)return false;}point=byte<128?byte:byte&((byte&0xe0)==0xc0?31:(byte&0xf0)==0xe0?15:7);}if(point&&point!='\n'){lv_font_glyph_dsc_t glyph{};if(!lv_font_get_glyph_dsc(font,&glyph,point,0)||glyph.is_placeholder)return false;}return true;}
const lv_image_dsc_t* Image(unsigned index){static lv_image_dsc_t images[6]{};static const uint8_t* data[]={standing,wink,curious,thinking,reading,celebrating};if(!images[0].data){for(unsigned i=0;i<6;i++){images[i].header.magic=LV_IMAGE_HEADER_MAGIC;images[i].header.cf=LV_COLOR_FORMAT_ARGB8888;images[i].header.w=160;images[i].header.h=160;images[i].header.stride=640;images[i].data_size=102400;images[i].data=data[i];}}return &images[index];}
}
TeloaDisplay::~TeloaDisplay(){if(animation_)lv_timer_delete(animation_);if(current_==this)current_=nullptr;}
void TeloaDisplay::SetupUI(){
 if(setup_ui_called_){return;}Display::SetupUI();DisplayLockGuard lock(this);current_=this;
 auto screen=lv_display_get_screen_active(display_);auto theme=static_cast<LvglTheme*>(current_theme_);Palette(theme);lv_obj_set_style_text_font(screen,theme->text_font()->font(),0);lv_obj_set_style_text_color(screen,theme->text_color(),0);
 container_=lv_obj_create(screen);lv_obj_set_size(container_,360,360);lv_obj_set_style_bg_color(container_,theme->background_color(),0);lv_obj_set_style_border_width(container_,0,0);lv_obj_set_style_pad_all(container_,0,0);lv_obj_remove_flag(container_,LV_OBJ_FLAG_SCROLLABLE);
 top_bar_=lv_obj_create(container_);lv_obj_set_pos(top_bar_,77,25);lv_obj_set_size(top_bar_,206,25);lv_obj_set_style_pad_all(top_bar_,0,0);lv_obj_set_style_bg_opa(top_bar_,LV_OPA_TRANSP,0);lv_obj_set_style_border_width(top_bar_,0,0);lv_obj_remove_flag(top_bar_,LV_OBJ_FLAG_SCROLLABLE);
 network_label_=Label(top_bar_,0,0,45,"");battery_label_=Label(top_bar_,165,0,40,"");mute_label_=Label(top_bar_,120,0,40,"");for(auto label:{network_label_,battery_label_,mute_label_})lv_obj_set_style_text_font(label,theme->icon_font()->font(),0);
 name_=Label(container_,65,54,230,kName);status_label_=Label(container_,55,82,250,"");notification_label_=Label(container_,55,82,250,"");lv_obj_add_flag(notification_label_,LV_OBJ_FLAG_HIDDEN);
 emoji_box_=lv_obj_create(container_);lv_obj_set_pos(emoji_box_,100,100);lv_obj_set_size(emoji_box_,160,160);lv_obj_set_style_pad_all(emoji_box_,0,0);lv_obj_set_style_border_width(emoji_box_,0,0);lv_obj_set_style_bg_opa(emoji_box_,LV_OPA_TRANSP,0);lv_obj_remove_flag(emoji_box_,LV_OBJ_FLAG_SCROLLABLE);emoji_image_=lv_image_create(emoji_box_);lv_image_set_src(emoji_image_,Image(0));
 chat_message_label_=Label(container_,45,265,270,"");lv_obj_set_height(chat_message_label_,28);lv_label_set_long_mode(chat_message_label_,LV_LABEL_LONG_DOT);
 caption_=Label(container_,50,107,260,"");lv_obj_set_height(caption_,168);lv_obj_set_style_text_line_space(caption_,0,0);lv_obj_add_flag(caption_,LV_OBJ_FLAG_HIDDEN);
 primary_=Label(container_,166,300,106,kReview);secondary_=Label(container_,88,300,74,"");lv_obj_add_flag(secondary_,LV_OBJ_FLAG_HIDDEN);lv_obj_set_style_text_color(primary_,lv_color_hex(0x2f6c51),0);
 for(auto button:{primary_,secondary_}){lv_obj_set_height(button,32);lv_obj_set_style_radius(button,17,0);lv_obj_set_style_bg_color(button,theme->assistant_bubble_color(),0);lv_obj_set_style_bg_opa(button,LV_OPA_COVER,0);lv_obj_set_style_pad_top(button,5,0);}
 animation_=lv_timer_create([](lv_timer_t* timer){static_cast<TeloaDisplay*>(lv_timer_get_user_data(timer))->Draw();},100,this);
}
void TeloaDisplay::SetStatus(const char* status){if(!status_label_)return;DisplayLockGuard lock(this);lv_label_set_text(status_label_,status?status:"");}
void TeloaDisplay::SetEmotion(const char* emotion){if(!emotion)return;if(!strcmp(emotion,"happy"))SetState("happy");else if(!strcmp(emotion,"thinking"))SetState("thinking");}
void TeloaDisplay::SetChatMessage(const char*,const char* text){if(!chat_message_label_)return;DisplayLockGuard lock(this);lv_label_set_text(chat_message_label_,text?text:"");}
void TeloaDisplay::ClearChatMessages(){SetChatMessage("system","");}
void TeloaDisplay::SetState(const std::string& state){DisplayLockGuard lock(this);state_=state;}
void TeloaDisplay::ApprovalCount(unsigned count){DisplayLockGuard lock(this);count_=count;if(!card_&&primary_){auto label=std::string(kReview)+(count?" ("+std::to_string(count)+")":"");lv_label_set_text(primary_,label.c_str());}}
void TeloaDisplay::ApprovalCard(const std::string& text,bool can_decide,const ApprovalIdentity& identity){
 DisplayLockGuard lock(this);if(!caption_)return;const bool readable=Readable(text,static_cast<LvglTheme*>(current_theme_)->text_font()->font());pages_=ApprovalPages(readable?text:std::string(teloa::device::strings::kReviewWeb));if(pages_.empty())pages_.push_back(kPending);page_=0;approval_=identity;shown_at_=esp_timer_get_time();card_=true;can_decide_=can_decide&&readable;lv_obj_remove_flag(secondary_,LV_OBJ_FLAG_HIDDEN);state_="approval";lv_obj_add_flag(emoji_box_,LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(chat_message_label_,LV_OBJ_FLAG_HIDDEN);lv_obj_remove_flag(caption_,LV_OBJ_FLAG_HIDDEN);Draw();
}
void TeloaDisplay::Pairing(const std::string& host){SetState("pairing");SetStatus(kPair);SetChatMessage("system",host.c_str());}
bool TeloaDisplay::DismissApproval(const std::string& ticket){DisplayLockGuard lock(this);if(!ticket.empty()&&ticket!=approval_.ticket)return false;approval_={};card_=false;pages_.clear();state_="idle";if(!emoji_box_)return true;lv_obj_remove_flag(emoji_box_,LV_OBJ_FLAG_HIDDEN);lv_obj_remove_flag(chat_message_label_,LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(caption_,LV_OBJ_FLAG_HIDDEN);lv_label_set_text(secondary_,"");lv_obj_add_flag(secondary_,LV_OBJ_FLAG_HIDDEN);lv_label_set_text(primary_,kReview);lv_label_set_text(status_label_,"");return true;}
void TeloaDisplay::Draw(){
 if(!emoji_image_)return;
 if(state_=="pairing"&&!UsbPairing::Instance().Pending()){state_="idle";lv_label_set_text(status_label_,"");}
 if(card_){lv_label_set_text(caption_,pages_[page_].c_str());lv_label_set_text(name_,kPending);auto page_label=std::to_string(page_+1)+" / "+std::to_string(pages_.size());lv_label_set_text(status_label_,page_label.c_str());lv_label_set_text(secondary_,can_decide_?kReject:kBack);lv_label_set_text(primary_,page_+1<pages_.size()?kNext:can_decide_?kApprove:kBack);return;}
 auto tick=lv_tick_get();unsigned pose=state_=="thinking"?3:state_=="listening"?2:state_=="approval"?4:state_=="happy"?5:tick%6000<200?1:0;
 lv_image_set_src(emoji_image_,Image(pose));int bounce=static_cast<int>(std::sin(tick/(state_=="speaking"?110.0:700.0))*(state_=="speaking"?4:2));lv_obj_set_y(emoji_box_,100+bounce);lv_label_set_text(name_,kName);
}
bool TeloaDisplay::Touch(int x,int y,uint32_t held_ms,bool held_inside){
 if(UsbPairing::Instance().ConfirmTouch(held_ms))return true;
 ScreenAction command;ApprovalIdentity displayed;bool dispatch=false;
 {DisplayLockGuard lock(this);displayed=approval_;if(card_){if(y<80){card_=false;lv_obj_remove_flag(emoji_box_,LV_OBJ_FLAG_HIDDEN);lv_obj_remove_flag(chat_message_label_,LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(caption_,LV_OBJ_FLAG_HIDDEN);lv_label_set_text(secondary_,"");command=ScreenAction::Back;dispatch=true;}else if(y>=290){if(x<160){command=can_decide_?ScreenAction::Reject:ScreenAction::Back;dispatch=true;}else if(page_+1<pages_.size()){++page_;shown_at_=esp_timer_get_time();Draw();return true;}else if(!can_decide_){command=ScreenAction::Back;dispatch=true;}else if(FreshApprovalHold(shown_at_,esp_timer_get_time(),held_ms,held_inside)){command=ScreenAction::ApproveOnce;dispatch=true;}else return true;}else return true;}
 else if(y>=290){command=ScreenAction::OpenApprovals;dispatch=true;}else return false;}
 if(dispatch&&command==ScreenAction::Back)DismissApproval();
 if(dispatch&&action_){action_({command,displayed});}return true;
}
}

void teloa::TeloaDisplay::SetTheme(Theme* theme){
 Display::SetTheme(theme);if(!container_)return;DisplayLockGuard lock(this);auto current=static_cast<LvglTheme*>(theme);Palette(current);auto screen=lv_display_get_screen_active(display_);lv_obj_set_style_text_font(screen,current->text_font()->font(),0);lv_obj_set_style_text_color(screen,current->text_color(),0);lv_obj_set_style_bg_color(container_,current->background_color(),0);for(auto label:{network_label_,battery_label_,mute_label_})lv_obj_set_style_text_font(label,current->icon_font()->font(),0);for(auto button:{primary_,secondary_})lv_obj_set_style_bg_color(button,current->assistant_bubble_color(),0);
}
