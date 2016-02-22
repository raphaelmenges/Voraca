# Voraca
Volume ray-casting powered by OpenGL 3.3 core profile. This project was part of my bachelor thesis in 2014!

### Screenshot
TODO

### Videos
* https://www.youtube.com/watch?v=sM18a2M5_FM
* https://www.youtube.com/watch?v=ZcDHNtyZ3es
* https://www.youtube.com/watch?v=rirZg5DN4OE

## Dependencies
* GLM: http://glm.g-truc.net/0.9.7/index.html (MIT license chosen)
* picoPNG: http://lodev.org/lodepng
* GLFW3: http://www.glfw.org
* AntTweakBar 1.16: http://anttweakbar.sourceforge.net
* RapidXML 1.13: http://rapidxml.sourceforge.net (Boost Software License chosen)

## TODO
* Better description
* 32bit support for Windows / Visual Studio
* Importance volume does not use complete information of volume if volume resolution % downscale !=0
* Histogram is one pixel too wide
* Shader::draw line 174: Reset of textureSlotCounter only works as expected if all textures assigned once or all textures assigned each frame. Mixing would break it.

## Acknowledgements
Thanks to Andre Taulien for supporting in the Linux compatibility!
Thanks to Nils Höhner for supporting in the MacOS compatibility! (not given right now)