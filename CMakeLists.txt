CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})
ADD_PARAVIEW_PLUGIN(EMReader "1.0"
    SERVER_MANAGER_XML server_reader.xml
    SERVER_MANAGER_SOURCES vtkEMReader.cxx
    GUI_RESOURCE_FILES client_reader.xml)