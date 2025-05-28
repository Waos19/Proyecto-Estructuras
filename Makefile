# Configuración básica
CC = g++
TARGET = programa.exe
SRC = main.cpp Grafo.cpp Nodo.cpp Interfaz.cpp
OBJ = $(SRC:.cpp=.o)

# Flags para SFML (MinGW-w64)
INCLUDE_PATH = -I/mingw64/include
LIBRARY_PATH = -L/mingw64/lib
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Flags de compilación
CXXFLAGS = -std=c++17 -Wall -Wextra $(INCLUDE_PATH)
LDFLAGS = $(LIBRARY_PATH) $(SFML_LIBS)

# Regla principal
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f $(OBJ) $(TARGET)

# Copiar DLLs necesarias (solo en Windows)
dlls:
	cp /mingw64/bin/sfml-graphics-2.dll .
	cp /mingw64/bin/sfml-window-2.dll .
	cp /mingw64/bin/sfml-system-2.dll .

.PHONY: all clean dlls
