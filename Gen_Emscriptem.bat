C:\Users\simon\Documents\emsciptem\emsdk\emsdk.bat activate latest
C:\Users\simon\Documents\emsciptem\emsdk\emsdk_env.bat

cd D:\Nongine-OpenGL\buildSource

emcmake cmake -G Ninja ..
ninja Game_Engine

python C:\Users\simon\Documents\emsciptem\emsdk\upstream\emscripten\emrun.py --no_browser --port 8080 Bin/Game_Engine.html

pause