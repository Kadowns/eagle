//
// Created by Novak on 25/08/2019.
//

#ifndef EG_ENGINE_RESOURCELOADER_H
#define EG_ENGINE_RESOURCELOADER_H

#include "EditorCore.h"

EG_EDITOR_BEGIN

class ResourceLoader {
public:

    ResourceLoader(const std::string& root);
    ~ResourceLoader();

private:

    void load_file(const boost::filesystem::path& path);


};

EG_EDITOR_END

#endif //EG_ENGINE_RESOURCELOADER_H
