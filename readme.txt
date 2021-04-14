+----------------------------------------+
 基于 libfacedetection 的人脸检测测试程序 
+----------------------------------------+

libfacedetection 是一个开源的人脸检测库：

https://github.com/ShiqiYu/libfacedetection

本测试程序，基于 libfacedetection 实现了读取 BMP 文件数据，并调用 libfacedetection 接口进行人脸检测。
检测的结果会打印到控制台，同时在 BMP 图片上绘制出人脸矩形框


编译和运行
----------
在 ubuntu 服务器或者 msys2 环境下，直接执行：
./build.sh
即可完成编译，生成 test 程序

执行：
./test test1.bmp
即可对 test1.bmp 图片中的人脸进行检测



性能测试结果
------------
通过在不同平台上的调试和测试，libfacedetection 的性能还是比 ncnn + mtcnn 差了一些，但是其识别的准确率比较高。



rockcarry
2021-4-14

