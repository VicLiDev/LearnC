针对每个库在 **Linux（Ubuntu/Debian）/Windows/macOS** 下的安装方式。

---

## 1. nlohmann/json （C++ JSON库）

> 这是一个 Header-only 库，**不需要安装，只需下载头文件即可使用。**

### 安装方式：

#### 方法一：使用包管理器安装

* **Ubuntu/Debian：**

  ```bash
  sudo apt install nlohmann-json3-dev
  ```
* **macOS（Homebrew）：**

  ```bash
  brew install nlohmann-json
  ```
* **Windows（vcpkg）：**

  ```bash
  vcpkg install nlohmann-json
  ```

#### 方法二：手动下载

只需下载 [`json.hpp`](https://github.com/nlohmann/json/releases) 文件放到你的项目中。

---

## 2. tinyxml2 （C++ XML库）

### 安装方式：

* **Ubuntu/Debian：**

  ```bash
  sudo apt install libtinyxml2-dev
  ```
* **macOS：**

  ```bash
  brew install tinyxml2
  ```
* **Windows（vcpkg）：**

  ```bash
  vcpkg install tinyxml2
  ```

---

## 3. yaml-cpp （C++ YAML库）

### 安装方式：

* **Ubuntu/Debian：**

  ```bash
  sudo apt install libyaml-cpp-dev
  ```
* **macOS：**

  ```bash
  brew install yaml-cpp
  ```
* **Windows（vcpkg）：**

  ```bash
  vcpkg install yaml-cpp
  ```

---

## 4. jansson （C JSON库）

### 安装方式：

* **Ubuntu/Debian：**

  ```bash
  sudo apt install libjansson-dev
  ```
* **macOS：**

  ```bash
  brew install jansson
  ```
* **Windows（vcpkg）：**

  ```bash
  vcpkg install jansson
  ```

---

## 5. libxml2 （C XML库）

### 安装方式：

* **Ubuntu/Debian：**

  ```bash
  sudo apt install libxml2-dev
  ```
* **macOS：**

  ```bash
  brew install libxml2
  ```
* **Windows（vcpkg）：**

  ```bash
  vcpkg install libxml2
  ```

> Windows 下使用 libxml2 时，注意配置 include/lib 路径和链接 `xml2` 库。

---

## 6. libfyaml （C YAML库）

### 安装方式：

#### Ubuntu 手动安装：

```bash
sudo apt install cmake libtool pkg-config
git clone https://github.com/pantoniou/libfyaml.git
cd libfyaml
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build
```

#### macOS（Homebrew 可能暂无）：

建议手动编译安装。

#### Windows：

目前没有官方支持，可以尝试用 MSYS2/Cygwin 编译源码。

---

## 使用 vcpkg 统一管理（推荐 Windows 用户）

```bash
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg.sh  # or .\bootstrap-vcpkg.bat on Windows

# 安装你需要的库
./vcpkg install nlohmann-json tinyxml2 yaml-cpp jansson libxml2
```

