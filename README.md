# Virtual-Paint
A paint app that draws on screen by detecting and tracking specific colour object.

Technology Used - OpenCV and Qt

Features Supported - Editing/Saving Images

# Installation Instructions For OpenCV (Linux)

Run the following commands

1. Installing required packages (last command is optional, run only if you want to test python version)
  
  ```
  sudo apt-get install build-essential
  sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
  sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
  ```
2. Downloading cmake
  
  ```
  sudo add-apt-repository ppa:george-edison55/cmake-3.x
  sudo apt-get update
  sudo apt-get install cmake
  ```
3. cd ~
4. Downloading opencv - wget https://github.com/Itseez/opencv/archive/2.4.13.zip
5. Building OpenCV
  
  ```
  unzip 2.4.13.zip
  cd opencv-2.4.13
  mkdir release
  cd release
  cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
  make
  sudo make install
  ```

# Installation Instructions For Qt (Linux)

1. For python, just run 

  ```
  sudo apt-get install python-qt4
  ```
  
2. For C++,
  * cd ~
  * Dowloading Qt - wget https://download.qt.io/archive/qt/4.8/4.8.6/qt-everywhere-opensource-src-4.8.6.tar.gz
  * Building Qt
    
    ```
    tar -xvzf qt-everywhere-opensource-src-4.8.6.tar.gz
    cd qt-everywhere-opensource-src-4.8.6
    ./configure -prefix /usr/local/qt4
    make
    sudo make install
    PATH=/usr/local/qt4/bin:$PATH
    export PATH
    ```
  * Installing qmake - sudo apt-get install qt4-qmake

# Running Insturctions

1. Running Python Version
  
  ```
  pip install -r requirements.txt
  python draw.py
  ```
2. For C++ Version
  
  ```
  qmake-qt4 -project && qmake-qt4
  make
  ```
  * Append following lines to the generated .pro file:
    
    ```
    CONFIG += c++11
    INCLUDEPATH += "/usr/local/include/opencv2"
    LIBS += `pkg-config --cflags --libs opencv
    ```
