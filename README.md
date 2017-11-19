# GLRenderer
###### Continuous learning of OpenGL through academic assignments and curiosity using GLM and GLFW

***

**Pacman3D**

A 3D Pacman game. Features enemies with basic pathfinding, camera and player movement, little red balls to pickup and an axis indicator. The grid size is user-defined. Camera movements include yaw, pitch, rotating the world around different axes, zooming and translating.

![](http://g.recordit.co/64lEUztk35.gif)

***

**Heightmap & Catmull-Rom Splines**

A height map visualizer. Uses stb_image to parse image information. Once the image is loaded, 4 different meshes are generated. The original unchanged picture, a version with skipsized points (user-defined), a version with one Catmull-Rom pass and a version with two Catmull-Rom passes. You can see the different renders using GL_POINTS and GL_TRIANGLES. The meshes are rendered using GL_TRIANGLE_STRIP and primitive restarting.

![](http://g.recordit.co/v2fSybv8Hk.gif)

***

**Raytracer**

A basic raytracer with lighting and shadows. It can compute intersections for spheres, planes, triangles and .obj's. It can produce images like the following.

![](https://s18.postimg.org/ygr7765ux/Capture.jpg)

