cmake_minimum_required(VERSION 3.10)
project(tracer LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)

set(SRC World/ViewPlane.cpp
        World/World.cpp
        BRDFs/AttributeBasedBrdf.cpp
        BRDFs/AttributeBasedBrdf.hpp
        BRDFs/Microfacet.cpp
        BRDFs/BRDF.cpp
        BRDFs/Lambertian.cpp
        Tracers/MultipleObjects.cpp
        Tracers/RayCast.cpp
        Tracers/SingleSphere.cpp
        Tracers/Tracer.cpp
        Materials/Material.cpp
        Materials/Matte.cpp
        Materials/GlossySpecular.cpp
        Materials/SVAttributeBasedMapping.cpp
        Lights/Ambient.cpp
        Lights/Directional.cpp
        Lights/Light.cpp
        GeometricObjects/Compound.cpp
        GeometricObjects/GeometricObject.cpp
        GeometricObjects/Grid.cpp
        GeometricObjects/Plane.cpp
        GeometricObjects/Sphere.cpp
        GeometricObjects/Instance.cpp
        GeometricObjects/Triangles/FlatMeshTriangle.cpp
        GeometricObjects/Triangles/MeshTriangle.cpp
        GeometricObjects/Triangles/SmoothMeshTriangle.cpp
        GeometricObjects/Triangles/SmoothTriangle.cpp
        GeometricObjects/Triangles/Triangle.cpp
        Cameras/Camera.cpp
        Cameras/Pinhole.cpp
        Utilities/Image.cpp
        Utilities/ImageTexture.hpp
        Utilities/BBox.cpp
        Utilities/ply.cpp
        Utilities/Matrix.cpp
        Utilities/Mesh.cpp
        Utilities/Normal.cpp
        Utilities/Point2D.cpp
        Utilities/Point3D.cpp
        Utilities/Ray.cpp
        Utilities/RGBColor.cpp
        Utilities/ShadeRec.cpp
        Utilities/Vector3D.cpp
        UserInterface/main.cpp
        UserInterface/ui.cpp)

add_executable(tracer ${SRC})
set_property(TARGET tracer PROPERTY CXX_STANDARD 17)

target_link_libraries(tracer
        PRIVATE stb
        PRIVATE json
        PRIVATE tinyply
        PRIVATE glad
        PRIVATE glfw
        PRIVATE glm
        PRIVATE OpenGL::GL)

target_include_directories(tracer
        PUBLIC BRDFs
        PUBLIC build
        PUBLIC Cameras
        PUBLIC GeometricObjects
        PUBLIC GeometricObjects/Triangles
        PUBLIC Lights
        PUBLIC Materials
        PUBLIC Tracers
        PUBLIC Utilities
        PUBLIC UserInterface
        PUBLIC World)

add_custom_command(TARGET tracer PRE_BUILD
        COMMENT "Copying resources..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../assets $<TARGET_FILE_DIR:tracer>/assets)