//
// Created by Novak on 28/08/2019.
//

#include <eagle/editor/ResourceLoader.h>



EG_EDITOR_BEGIN

ResourceLoader::ResourceLoader(const std::string &root) {
    namespace fs = boost::filesystem;

    fs::recursive_directory_iterator it(root);

    std::vector<fs::path> paths;
    paths.emplace_back(it->path().parent_path());
    for (; it != fs::recursive_directory_iterator(); ++it){
        fs::path path = it->path();

        if (fs::is_regular_file(path)){
            load_file(path);
        }

        paths.emplace_back(path);
    }
}

ResourceLoader::~ResourceLoader() {

}

void ResourceLoader::load_file(const boost::filesystem::path &path) {
    auto extension = path.extension().string();
    if (extension == ".png"){

    }
}


EG_EDITOR_END