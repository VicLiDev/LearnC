# sockaddr_un 和 sockaddr_in

struct sockaddr_un 和 struct sockaddr_in 是用于 Unix 套接字（包括本地套接字）和
网络套接字编程中的两种重要的地址结构。它们分别用于本地进程间通信（IPC）和通过 IP
网络进行通信。

## struct sockaddr_un

`struct sockaddr_un` 用于 Unix 域套接字（也称为本地套接字），这种套接字提供了一种
在同一台机器上的不同进程之间进行通信的方式。它主要用于 IPC，但也可以跨文件系统挂载
点（在支持该功能的系统上）进行通信。

sockaddr_un 结构定义如下（在 POSIX 兼容系统上）：
```
#include <sys/un.h>

struct sockaddr_un {
    sa_family_t sun_family;  // 地址族，对于 Unix 域套接字，通常是 AF_UNIX
    char        sun_path[108]; // 套接字路径名（以 null 结尾的字符串）
};
```
* sun_family 字段必须设置为 AF_UNIX 或 AF_LOCAL（它们通常是等价的）。
* sun_path 字段包含了套接字的路径名。这个路径名用于唯一标识 Unix 域套接字。对于
  抽象套接字（如果路径名以 null 字节开头），路径名实际上并不指向文件系统中的一个
  路径，而是被存储在内核的命名空间中。

## struct sockaddr_in

`struct sockaddr_in` 用于 IPv4 网络套接字编程。它包含了 IPv4 地址和端口号，使得
应用程序可以指定一个网络地址，并通过该地址进行网络通信。

sockaddr_in 结构定义如下（在包括`<netinet/in.h>`头文件时）：
```
#include <netinet/in.h>

struct sockaddr_in {
    sa_family_t    sin_family; // 地址族，对于 IPv4 网络套接字，通常是 AF_INET
    uint16_t       sin_port;   // 端口号（网络字节顺序）
    struct in_addr sin_addr;   // IPv4 地址
    // 通常还有其他填充字节以符合 sockaddr 的结构对齐要求
};

// 其中，in_addr 结构通常如下定义：
struct in_addr {
    uint32_t s_addr; // IPv4 地址（网络字节顺序）
};
```
* sin_family 字段必须设置为 AF_INET。
* sin_port 字段包含了端口号，这个端口号用于标识接收数据的应用程序。端口号需要以
  网络字节顺序（大端字节序）给出。
* sin_addr 字段包含了 IPv4 地址，该地址同样需要以网络字节顺序给出。

在网络编程中，`sockaddr_in` 结构经常与 sockaddr 结构的联合体一起使用，因为 sockaddr
是 bind()、connect()、accept() 等函数的参数类型，它提供了一个通用的地址结构，而
sockaddr_in 则是用于 IPv4 的具体实现。使用这种结构可以使得网络程序更加灵活，支持
不同类型的网络地址。
