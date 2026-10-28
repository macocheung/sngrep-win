#include <pcap.h>
#include <stdio.h>

int main(void)
{
    char err[PCAP_ERRBUF_SIZE];
    pcap_if_t *d, *l;

    printf("%s\n", pcap_lib_version());
    if (pcap_findalldevs(&l, err) == -1) {
        printf("err: %s\n", err);
        return 1;
    }
    for (d = l; d; d = d->next)
        printf("%s | %s | flags=0x%x\n", d->name, d->description ? d->description : "", d->flags);
    return 0;
}
