#include "types.h"
#include <error.h>
#include <vulkan/vulkan_core.h>



VkVertexInputBindingDescription GetVertexBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {0};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    return bindingDescription; 
}

VertexAttrDescription GetVertexAttributeDescriptions() {
    VkVertexInputAttributeDescription attrs[2];

    attrs[0].binding = 0;
    attrs[0].location = 0;
    attrs[0].format = VK_FORMAT_R32G32_SFLOAT;
    attrs[0].offset = offsetof(Vertex, position);

    attrs[1].binding = 0;
    attrs[1].location = 1;
    attrs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrs[1].offset = offsetof(Vertex, color);

    return (VertexAttrDescription){{attrs[0], attrs[1]}};
}
