//
// Created by Novak on 14/07/2019.
//

#ifndef EAGLE_VULKANCOMMANDLIST_H
#define EAGLE_VULKANCOMMANDLIST_H

#include <memory>
#include <vector>

#include "VulkanCore.h"
#include "VulkanCommand.h"

EG_BEGIN

class VulkanCommandList {

public:

    VulkanCommandList() = default;
    ~VulkanCommandList() = default;

    inline void push(Reference<VulkanCommand> cmd){
        if (m_secondaryPass){
            m_commands.emplace(m_commands.begin() + m_secondaryPassIndex, cmd);
            m_secondaryPassIndex++;
        }
        else {
            m_commands.emplace_back(cmd);
        }
    }

    inline void execute() {
        for (const auto& cmd : m_commands){
            cmd->operator()();
        }
    }

    inline void clear(){
        m_commands.clear();
        m_secondaryPass = false;
        m_secondaryPassIndex = 0;
    }

    inline void begin_secondary_pass() {m_secondaryPass = true;}
    inline void end_secondary_pass() {m_secondaryPass = false;}

private:

    std::vector<Reference<VulkanCommand>> m_commands;
    uint32_t m_secondaryPassIndex = 0;
    bool m_secondaryPass = false;

};

EG_END

#endif //EAGLE_VULKANCOMMANDLIST_H
