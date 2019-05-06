#include <iostream>

#include <eagle/core/Application.h>

#include <vector>

int main() {

    Eagle::Application app("Eagle", 1024, 512);
    app.run();

    return 0;
}