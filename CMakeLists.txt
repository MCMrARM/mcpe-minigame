cmake_minimum_required(VERSION 3.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
set(CMAKE_CXX_STANDARD 14)

include_directories(minecraft-headers/)
add_library(minigame SHARED src/main.cpp src/minigame/MinigameDimension.h src/minigame/MinigameDimension.cpp src/minigame/Minigame.h src/minigame/Minigame.cpp src/minigame/MapConfig.h src/minigame/MapConfig.cpp src/minigame/MinigameManager.h src/minigame/MinigameManager.cpp src/util/Log.h src/util/Log.cpp src/util/PlayerHelper.h src/util/PlayerHelper.cpp src/util/PlayerData.h src/minigame/skywars/SkyWarsMinigame.h src/minigame/skywars/SkyWarsMinigame.cpp src/minigame/commands/ForceStartMinigameCommand.cpp src/minigame/commands/ForceStartMinigameCommand.h src/minigame/commands/MinigameCommandHelper.h src/minigame/commands/JoinMinigameCommand.cpp src/minigame/commands/JoinMinigameCommand.h src/util/PlayerPosLimitHelper.cpp src/util/PlayerPosLimitHelper.h src/lobby/LobbyManager.cpp src/lobby/LobbyManager.h src/lobby/NoSavePlayersPatch.cpp src/minigame/commands/ReturnToLobbyCommand.cpp src/minigame/commands/ReturnToLobbyCommand.h src/minigame/FakeNetherDimension.cpp src/minigame/FakeNetherDimension.h src/minigame/EmptyWorldGenerator.cpp src/minigame/EmptyWorldGenerator.h src/minigame/MinigamePlayerHooks.cpp)

target_link_libraries(minigame dl ${CMAKE_SOURCE_DIR}/dep-libs/libminecraftpe.so)