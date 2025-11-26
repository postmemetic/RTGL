# Apply small source fixes automatically so configure-and-build works without manual edits.

set(GLTF_EXPORTER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/GltfExporter.cpp")

if(EXISTS "${GLTF_EXPORTER_FILE}")
    file(READ "${GLTF_EXPORTER_FILE}" GLTF_EXPORTER_CONTENTS)

    if(NOT GLTF_EXPORTER_CONTENTS MATCHES "#include <ranges>")
        string(REPLACE "#include <type_traits>\n"
                       "#include <type_traits>\n#include <ranges>\n"
                       GLTF_EXPORTER_CONTENTS
                       "${GLTF_EXPORTER_CONTENTS}")

        if(GLTF_EXPORTER_CONTENTS MATCHES "#include <ranges>")
            file(WRITE "${GLTF_EXPORTER_FILE}" "${GLTF_EXPORTER_CONTENTS}")
            message(STATUS "Applied <ranges> include patch to Source/GltfExporter.cpp")
        else()
            message(WARNING "Failed to patch Source/GltfExporter.cpp with <ranges> include")
        endif()
    endif()
endif()
