#include <rttr/registration>
#include "Particle.h" 
#include "../Base/Datas/ReflectionMedtaDatas.hpp"

//RTTR_REGISTRATION
//{
//    using namespace rttr;
//
//    registration::class_<Particle>("Particle")
//        // instance data
//        .property("pos",          &Particle::pos)
//        .property("rotation",     &Particle::rotation)
//        .property("size",         &Particle::size)
//        .property("color",        &Particle::color)
//        .property("frame",        &Particle::frame)
//
//        // spawn - moving
//        .property("vel",          &Particle::vel)
//        .property("angularVel",   &Particle::angularVel)
//
//        // life
//        .property("alive",        &Particle::alive)
//            (metadata(META_BOOL, true)) 
//        .property("age",          &Particle::age)            
//        .property("life",         &Particle::life);
//}
