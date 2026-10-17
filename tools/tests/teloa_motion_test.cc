#include "teloa_motion.h"
#include <cassert>
#include <cstring>
#include <set>
#include <string>
#include <cmath>
#include <iostream>
using namespace teloa;
int main(){
    std::set<std::string> activities;
    int min_x=100,max_x=-100;
    for(unsigned time=0;time<78000;time+=17){
        const auto frame=SamplePetMotion("idle",time);
        assert(frame.x>=-40&&frame.x<=40&&frame.y>=-14&&frame.y<=12);
        assert(frame.sx>=240&&frame.sx<=270&&frame.sy>=240&&frame.sy<=270&&frame.pose<6);
        activities.insert(frame.activity);min_x=std::min(min_x,frame.x);max_x=std::max(max_x,frame.x);
        const auto repeated=SamplePetMotion("idle",time+26000);
        assert(frame.x==repeated.x&&frame.y==repeated.y&&frame.pose==repeated.pose);
    }
    assert(min_x<=-35&&max_x>=25);
    for(auto name:{"stroll","look","read","rest","stretch"})assert(activities.count(name));
    auto previous=SamplePetMotion("idle",25999),next=SamplePetMotion("idle",26000);
    assert(std::abs(previous.x-next.x)<=1&&std::abs(previous.y-next.y)<=1);
    for(auto mode:{"listening","speaking","thinking","happy","pet","approval","pairing"}){
        for(unsigned time=0;time<5000;time+=13){
            const auto f=SamplePetMotion(mode,time);assert(f.pose<6&&std::abs(f.x)<=40&&std::abs(f.y)<=14);
        }
    }
    const auto working=SamplePetMotion("thinking",500);assert(!strcmp(working.activity,"working")&&!strcmp(working.prop,"desk"));
    assert(!strcmp(SamplePetMotion("pet",1600).activity,"idle"));
    assert(!strcmp(SamplePetMotion("happy",2800).activity,"idle"));
    const auto far=SamplePetMotion("idle",8000),listen=SamplePetMotion("listening",0);
    assert(BlendPetMotion(far,listen,0).x==far.x);
    assert(BlendPetMotion(far,listen,240).x==listen.x);
    assert(ResolvePetTouch(175,160,100,90,true,false,false)==PetTouch::Pet);
    assert(ResolvePetTouch(175,160,100,90,false,false,false)==PetTouch::Ignore);
    assert(ResolvePetTouch(175,160,100,90,true,true,false)==PetTouch::Ignore);
    assert(ResolvePetTouch(120,315,100,90,true,false,false)==PetTouch::Voice);
    assert(ResolvePetTouch(220,315,100,90,true,false,false)==PetTouch::Review);
    assert(ResolvePetTouch(120,315,100,90,true,false,true)==PetTouch::Ignore);
    assert(ResolvePetTouch(5,315,100,90,true,false,false)==PetTouch::Ignore);
    std::cout<<"pet motion: bounds, life cycle, real-state priority, transitions and touch separation passed\n";
}
