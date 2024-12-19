cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(DetectorSimulation)

# Ищем Geant4
find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

# Ищем Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
# Если используется многозадачность:
if(GEANT4_USE_QT)
    message(STATUS "Geant4 is configured with Qt support.")
else()
    message(FATAL_ERROR "Geant4 is not configured with Qt support.")
endif()

# Создаём исполнимый файл
add_executable(detector_simulation
    main.cc
    DetectorConstruction.cc
    PhysicsList.cc
    ActionInitialization.cc
    PrimaryGeneratorAction.cc
    RunAction.cc
    PMTSensitiveDetector.cc
)

# Линкуем Geant4 и Qt5
target_link_libraries(detector_simulation
    ${Geant4_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Устанавливаем стандарт C++ 17
set(CMAKE_CXX_STANDARD 17)

# Указываем нужные флаги для визуализации с Qt
# Если Qt5 найден, то Geant4 должен быть настроен с Qt
if (Qt5_FOUND)
    message(STATUS "Qt5 found, enabling Qt visualization for Geant4")
else()
    message(FATAL_ERROR "Qt5 not found! Please install Qt5 or ensure it is found by CMake.")
endif()

# Если используется многозадачность в Geant4
if(GEANT4_USE_QT)
    message(STATUS "Geant4 is configured with Qt support.")
else()
    message(FATAL_ERROR "Geant4 is not configured with Qt support. Please configure with -DGEANT4_USE_QT=ON")
endif()

