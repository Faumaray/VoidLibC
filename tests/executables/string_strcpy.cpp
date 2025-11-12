import voidlibc.c.io;
import voidlibc.c.string;

static int fail(const char* msg){
    write(2, msg, strlen(msg));
    return 1;
}

extern "C" int main(int, char**, char**){
    char buf[16];
    for (int i=0;i<16;++i){ buf[i]=0x7f; }

    char* ret = strcpy(buf, "copy");
    if (ret != buf) {
        return fail("strcpy return mismatch\n");
    }
    if (strcmp(buf, "copy") != 0){
        return fail("strcpy content mismatch\n");
    }
    if (buf[5] != '\0'){
        return fail("strcpy missing terminator\n");
    }
    if (buf[6] != char(0x7f)){
        return fail("strcpy overrun\n");
    }

    static const char ok[] = "strcpy ok\n";
    write(1, ok, sizeof(ok)-1);
    return 0;
}
