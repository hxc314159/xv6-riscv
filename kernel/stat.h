#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

struct stat {
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes
};


// 这些宏定义了文件的类型标识符，通常用于操作系统中的文件系统。
// T_DIR：值为 1，表示目录类型。
// T_FILE：值为 2，表示文件类型。
// T_DEVICE：值为 3，表示设备类型。
// 这些常量通常会用在文件的元数据结构中，用来标识文件的类型，例如判断一个文件是否是目录、普通文件或者设备文件。


// stat 结构体用于存储文件的元数据（即描述文件的相关信息），它包含以下字段：

// dev (int dev)：

// 文件所在的设备号，标识该文件属于哪个文件系统设备。例如，在多磁盘系统中，设备号可以帮助操作系统找到文件所在的磁盘。
// ino (uint ino)：

// 文件的 inode 号（索引节点号）。inode 是文件系统中用来存储文件元数据的数据结构。每个文件都有一个唯一的 inode，它包含了文件的权限、所有者、大小、数据块位置等信息。
// type (short type)：

// 文件的类型，通常由文件系统提供。值可以是以下之一：
// T_DIR：目录文件。
// T_FILE：普通文件。
// T_DEVICE：设备文件。
// 在代码中，type 用于区分不同的文件类型。
// nlink (short nlink)：

// 文件的硬链接数量。每个文件都可以有多个硬链接，表示不同的路径指向同一个文件的 inode。
// size (uint64 size)：

// 文件的大小（以字节为单位）。这个字段表示文件的总字节数。如果文件是设备文件或目录，大小可能会有所不同。
