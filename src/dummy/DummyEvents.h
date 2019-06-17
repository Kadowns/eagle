//
// Created by Novak on 15/06/2019.
//

#ifndef EAGLE_DUMMYEVENTS_H
#define EAGLE_DUMMYEVENTS_H

#include <eagle/Eagle.h>

class ButtonClickedEvent : public Eagle::Event {
public:
    EVENT_CLASS_TYPE(Eagle::EVENT_TYPE::CUSTOM)
    EVENT_CLASS_CATEGORY(Eagle::EVENT_CATEGORY_CUSTOM)
};



#endif //EAGLE_DUMMYEVENTS_H
