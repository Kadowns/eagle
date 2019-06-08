//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_MAIN_CPP
#define EAGLE_MAIN_CPP

#include <eagle/Eagle.h>
#include <iostream>

extern Eagle::ApplicationCreateInfo Eagle::create_application_info();

int main(){

    Eagle::ApplicationCreateInfo createInfo = Eagle::create_application_info();
    std::unique_ptr<Eagle::Application> app = std::make_unique<Eagle::Application>(createInfo);

    try{
        app->run();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

#endif //EAGLE_MAIN_CPP
