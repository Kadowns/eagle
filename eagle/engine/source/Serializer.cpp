//
// Created by Novak on 09/08/2019.
//

#include <fstream>

#include <eagle/engine/Serializer.h>
#include <eagle/engine/components/Renderable.h>
#include <eagle/engine/components/Transform.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

EG_ENGINE_BEGIN

Serializer::Serializer(const std::string &path) : m_path(path) {

}

void Serializer::load(Scene &scene) {
    std::ifstream ifs(m_path);
    boost::archive::text_iarchive ia(ifs);

    size_t numberOfEntities;
    ia >> numberOfEntities;

    auto entities = scene.entities();

    for (size_t i = 0; i < numberOfEntities; i++){
        auto e = entities->create();
        ia >> *e->assign<Transform>();
        ia >> *e->assign<Renderable>();
    }
    auto e = entities->create();
    e->assign<DirectionalLight>();
}

void Serializer::save(const Scene &scene) {
    std::ofstream ofs(m_path);
    //ofs.clear();
    boost::archive::text_oarchive oa(ofs);
    oa << scene.entities()->size();
    for (auto e : scene.entities()->entities()){
        oa << *e->component<Transform>();
        oa << *e->component<Renderable>();
    }
}

EG_ENGINE_END
