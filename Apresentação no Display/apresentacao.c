#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int file;
    char * pipefile = "/tmp/pipefile";
    char buf[256];
    int bytesRead;

    file = open(pipefile, O_RDONLY);

    while(1) {
        bytesRead = read(file, buf, 256);
        if(bytesRead != 0) {
            printf("%s\n", buf);
        }
        sleep(1);
    }

    close(file);

    return 0;
}