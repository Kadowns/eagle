//
// Created by Ricardo on 11/29/2020.
//
#include "RaytracingApp.h"

int main(){

    Eagle::Log::init(Eagle::Log::TRACE, Eagle::Log::TRACE);

    RaytracingApp app;

    try{
        app.run();
    }catch(std::exception& e){
        EG_CORE_FATAL_F("Exception: {0}", e.what());
        return 1;
    }

    return 0;
}
