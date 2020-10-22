#define assert(condition, error_message, success_message) condition &&printf("[+] %s\n\tin File:%s, Line:%d\n", success_message, __FILE__, __LINE__) || printf("[-] %s\n\tERROR in file %s: line %d\n", error_message, __FILE__, __LINE__) && exitt(1)
int exitt(int n) { exit(n); }

int main(int argc, char const *argv[])
{
    assert(0,"TESTING", "SUCCESS");
    printf("ABHI BHI CHAL RA HAI !!!\n");
    return 0;
}
