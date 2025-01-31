all:
	g++ *.cpp -o main -std=c++11 -lXi -lGLEW -lGLU -lm -lGL -lm -lpthread -ldl -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp -ljpeg

windows:
	g++ main.cpp helper.cpp -o main -std=c++11 -lglew32s -lglfw3 -lopengl32 -lglu32 -lgdi32 -ljpeg