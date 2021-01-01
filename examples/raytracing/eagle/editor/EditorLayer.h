//
// Created by Novak on 22/03/2020.
//

#ifndef EAGLE_EDITORLAYER_H
#define EAGLE_EDITORLAYER_H

#include <eagle/editor/EditorGlobalDefinitions.h>
#include <eagle/editor/EditorMaster.h>

EG_EDITOR_BEGIN

class EditorLayer : public Layer {
public:

    EditorLayer();
    ~EditorLayer();

    virtual void handle_attach(EventBus* eventBus) override;

    virtual void handle_detach() override;

    virtual void handle_update() override;

private:
    EditorMaster m_editorMaster;
    EventListener<EditorLayer> m_listener;
};

EG_EDITOR_END

#endif //EAGLE_EDITORLAYER_H
