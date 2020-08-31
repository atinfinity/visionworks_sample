# immediate_vs_graph

## Requirements
- CUDA Toolkit
- NVIDIA VisionWorks
- OpenCV 3.x
- CMake

## Build Instructions
```
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release ..
make
```

## Sample Execution
```
./immediate_vs_graph image_2/000000_10.png image_3/000000_10.png
```

The [Stereo Evaluation 2015](http://www.cvlibs.net/datasets/kitti/eval_scene_flow.php?benchmark=stereo) dataset was used to test this sample code.
