cmake_minimum_required(VERSION 3.10)
project(tracer LANGUAGES CXX)

set(wxWidgets_USE_UNICODE)

find_package(wxWidgets COMPONENTS net core base REQUIRED)
include(${wxWidgets_USE_FILE})

set(SRC World/ViewPlane.cpp
        World/ViewPlane.h
        World/World.cpp
        World/World.h
        UserInterface/wxraytracer.cpp
        UserInterface/wxraytracer.h
        BRDFs/Microfacet.h
        BRDFs/Microfacet.cpp
        BRDFs/BRDF.h
        BRDFs/BRDF.cpp
        BRDFs/Lambertian.h
        BRDFs/Lambertian.cpp
        Tracers/MultipleObjects.cpp
        Tracers/MultipleObjects.h
        Tracers/RayCast.cpp
        Tracers/RayCast.h
        Tracers/SingleSphere.cpp
        Tracers/SingleSphere.h
        Tracers/Tracer.cpp
        Tracers/Tracer.h
        Materials/Material.cpp
        Materials/Material.h
        Materials/Matte.cpp
        Materials/Matte.h
        Materials/GlossySpecular.cpp
        Materials/GlossySpecular.h
        Lights/Ambient.cpp
        Lights/Ambient.h
        Lights/Directional.cpp
        Lights/Directional.h
        Lights/Light.cpp
        Lights/Light.h
        GeometricObjects/GeometricObject.cpp
        GeometricObjects/GeometricObject.h
        GeometricObjects/Plane.cpp
        GeometricObjects/Plane.h
        GeometricObjects/Sphere.cpp
        GeometricObjects/Sphere.h
        Cameras/Camera.cpp
        Cameras/Camera.h
        Cameras/Pinhole.cpp
        Cameras/Pinhole.h
        Utilities/Constants.h
        Utilities/Maths.h
        Utilities/Matrix.cpp
        Utilities/Matrix.h
        Utilities/Mesh.cpp
        Utilities/Mesh.h
        Utilities/Normal.cpp
        Utilities/Normal.h
        Utilities/Point2D.cpp
        Utilities/Point2D.h
        Utilities/Point3D.cpp
        Utilities/Point3D.h
        Utilities/Ray.cpp
        Utilities/Ray.h
        Utilities/RGBColor.cpp
        Utilities/RGBColor.h
        Utilities/ShadeRec.cpp
        Utilities/ShadeRec.h
        Utilities/Vector3D.cpp
        Utilities/Vector3D.h)

add_executable(tracer WIN32 ${SRC})
target_link_libraries(tracer PUBLIC ${wxWidgets_LIBRARIES})
target_include_directories(tracer PUBLIC ${wxWidgets_INCLUDE_DIRS}
        PUBLIC BRDFs
        PUBLIC build
        PUBLIC Cameras
        PUBLIC GeometricObjects
        PUBLIC Lights
        PUBLIC Materials
        PUBLIC Tracers
        PUBLIC UserInterface
        PUBLIC Utilities
        PUBLIC World)