//
// Created by Novak on 15/11/2019.
//

#ifndef EAGLE_MAINLAYER_H
#define EAGLE_MAINLAYER_H

#include "GlobalDefs.h"

EG_ENGINE_BEGIN

class MainLayer : public Layer{
public:
    virtual void handle_attach() override;

    virtual void handle_deattach() override;

    virtual void handle_update() override;

    virtual void handle_event(Event &e) override;
};

EG_ENGINE_END

#endif //EAGLE_MAINLAYER_H
