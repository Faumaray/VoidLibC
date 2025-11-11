import rt.printf;
import rt.errno;
import rt.time;
import rt.timerfd;
import rt.snprintf;

extern "C" int main() {
    rt::time::timespec t{0, 50'000'000}; // 50ms
    rt::time::nanosleep(&t, nullptr);

    int tfd = rt::timerfd::create(rt::timerfd::CLOCK_MONOTONIC, rt::timerfd::TFD_NONBLOCK);
    rt::printf::dprintf(1, "timerfd=%d errno=%d\n", tfd, rt::get_errno());

    char buf[64];
    rt::printf::snprintf(buf, sizeof(buf), "hello %d %x\n", 42, 0xBEEF);
    rt::printf::dprintf(1, "%s", buf);
    return 0;
}
