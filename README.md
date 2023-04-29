# Popit Decorations

A simple and easy-to-use 3D editor, which allows the user to attach "decorations" to a cube in a scene.
![presentation-monster](https://user-images.githubusercontent.com/112534115/235277268-a51781dd-b4ff-43f0-bbb8-ae2b07dda4b7.png)

## How to build


*Windows*

Open git bash to the directory containing this repository.

```
project-template $ mkdir build
project-template $ cd build
project-template/build $ cmake ..
project-template/build $ start project-template.sln
```

Your solution file should contain four projects.
To run from the git bash command shell, 

```
project-template/build $ ../bin/Debug/demo.exe
```

*macOS*

Open terminal to the directory containing this repository.

```
project-template $ mkdir build
project-template $ cd build
project-template/build $ cmake ..
project-template/build $ make
```

To run each program from build, you would type

```
project-template/build $ ../bin/demo
```


## Demo of basic features

*Camera controls*

https://user-images.githubusercontent.com/112534115/235279837-349f8e36-7010-4892-8d97-82346ae1c0df.mp4

The user scrolls their mouse to orbit the center of the scene, and holds control to zoom in and out while scrolling.

*Mouse raycasting*

https://user-images.githubusercontent.com/112534115/235279993-803f1ea9-e578-4f55-a24d-ecdd407a7420.mp4

If your mouse goes in front of a cube, your selected decoration will be casted onto the surface of the object.
Multiple cubes have the intersection distance compared to determine the correct one.

## Unique features 

*Color control*

https://user-images.githubusercontent.com/112534115/235280999-9d222a62-844c-45c5-b162-67d47a10f033.mp4

Through the use of the keys r, g, b, i, and k, you can change the red, green, blue, and intensity values of a decoration (r, g, and b increasing respective values, and increasing intensity, while k decreases intensity). Some meshes have textures that only allow part of the object's color to change, which allows them to stay recognizable.

*Decoration Controls*

https://user-images.githubusercontent.com/112534115/235281129-5e646f04-2cef-4c6e-b5ba-1b468cfde837.mp4

The w, a, s, and d keys can be used to manipulate decorations. w and s scale the decoration up and down. The a key rotates the mesh counter-clockwise, while the d key rotates it clockwise. Pressing e cycles through the available meshes. Clicking the mouse adds the mesh to the scene as it appears while following the mouse. 

*Adding Cubes*

https://user-images.githubusercontent.com/112534115/235283172-d63a56ce-76ac-45b8-8e45-e04c20f502b8.mp4

One decoration that can be used is a cube. This cube can have decorations placed on it, which allows for many creative scenes. Note that the video has been cut near the middle to reduce file size. Also, another feature of the camera controls is demonstrated here. While the camera is being moved in any way, or the scroll pad/wheel is being interacted with, the last position of the mouse will be kept for the current decoration. This allows the user to see the decoration from different angles before placing it, as shown is this video when creating the "ground" in the scene.

Here are various scenes made with this tool:

<img width="983" alt="action-demoman" src="https://user-images.githubusercontent.com/112534115/235283037-e89c3092-bcb5-4c9f-923d-a161f50902e1.png">

<img width="988" alt="hungry-caterpillar" src="https://user-images.githubusercontent.com/112534115/235282607-83fe0fed-0e42-4f57-8b99-b128f47bd4ef.png">

<img width="984" alt="buff" src="https://user-images.githubusercontent.com/112534115/235282621-a4acc9ef-c481-4fdc-af0d-e16e949ddfb7.png">

<img width="978" alt="dog" src="https://user-images.githubusercontent.com/112534115/235282624-55416d9c-daab-4d95-b2d3-ae5c2e5f5362.png">
