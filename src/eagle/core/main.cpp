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
    Eagle::Application app(createInfo);

    try{
        app.run();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif //EAGLE_MAIN_CPP
