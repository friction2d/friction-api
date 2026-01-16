# Part of Friction <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

add_library(dummy-plugin SHARED dummyplugin.cpp dummyplugin.h dummy.json)
target_compile_definitions(dummy-plugin PRIVATE QT_DEPRECATED_WARNINGS)
target_link_libraries(dummy-plugin PRIVATE ${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(dummy-plugin PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)
