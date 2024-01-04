CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -lzmq -lsfml-graphics -lsfml-window -lsfml-system -pthread -lv8 

# Define the target executables
SERVER_TARGET = gameServer
CLIENT_TARGET = gameClient

# List of source files
SERVER_SRC = gameServer.cpp Platforms.h v8helpers.cpp ScriptManager.cpp Timeline.cpp bullet.cpp movingPlatform.cpp landPlatform.cpp playableRec.cpp Event.cpp EventPlayerMovement.cpp EventCollision.cpp EventDeathOffMap.cpp EventNewSpawn.cpp EventHandler.cpp EventManager.cpp 
CLIENT_SRC = gameClient.cpp Platforms.h v8helpers.cpp ScriptManager.cpp Timeline.cpp bullet.cpp movingPlatform.cpp landPlatform.cpp playableRec.cpp Event.cpp EventPlayerMovement.cpp EventCollision.cpp EventDeathOffMap.cpp EventNewSpawn.cpp EventHandler.cpp EventManager.cpp 

# List of object files
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

# Include paths for header files
INCLUDES = -I/usr/include -I/usr/include/v8 -I./src/include 

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -L/usr/lib/aarch64-linux-gnu

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -L/usr/lib/aarch64-linux-gnu

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_TARGET) $(CLIENT_TARGET)
