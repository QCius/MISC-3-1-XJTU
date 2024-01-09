#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int fd;
    char buffer[1024];

    // 打开设备文件进行写入
    fd = open("/dev/chardev0", O_RDWR);
    if (fd == -1) {
        perror("Unable to open device");
        exit(EXIT_FAILURE);
    }

    // 从控制台输入消息并写入设备文件
    while (1) {
        printf("Enter message: ");
        fgets(buffer, sizeof(buffer), stdin);
        write(fd, buffer, strlen(buffer));
    }

    // 关闭设备文件
    close(fd);

    return 0;
}

