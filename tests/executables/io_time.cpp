import voidlibc.c.io;
import voidlibc.c.time;
import voidlibc.core.types;

extern "C" int main(int, char**, char**) {
    const char msg[] = "io+time ok\n";
    write(1, msg, sizeof(msg)-1);

    // lseek sanity: create/truncate file and write then lseek back to 0
    int fd = open("tmp.vlc", O_CREAT | O_TRUNC | O_RDWR, 0644u);
    if (fd < 0) return 1;
    const char x[] = "abc";
    if (write(fd, x, 3) != 3) return 2;
    if (lseek(fd, 0, SEEK_SET) != 0) return 3;
    char buf[3];
    if (read(fd, buf, 3) != 3) return 4;
    close(fd);

    // time sanity
    static timeval tv{};
    if (gettimeofday(&tv, nullptr) < 0) return 5;
    timespec ts{0, 1000000}; // 1ms
    nanosleep(&ts, nullptr);
    return 0;
}
