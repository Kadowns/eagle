//
// Created by Novak on 09/08/2019.
//

#ifndef EAGLE_SERIALIZER_H
#define EAGLE_SERIALIZER_H

#include "EditorCore.h"

EG_EDITOR_BEGIN

class Serializer {

public:
    Serializer(const std::string& path);

    void load(Scene &scene);
    void save(const Scene &scene);

private:
    std::string m_path;

};

EG_EDITOR_END

#endif //EAGLE_SERIALIZER_H
