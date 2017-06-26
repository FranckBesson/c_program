#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/fcntl.h>
#include <unistd.h>



int main() {
    int fd;
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    struct curl_slist *list = NULL;
    int hour = 52;

    while (1) {
        printf("\n");
        if ((fd = open("/dev/ttyACM0", O_RDWR)) == -1) {
            perror("open");
            exit(-1);
        }
        int test = 0;
        char buf[255];

        while (test == 0) {
            int nb = read(fd, &buf, 255);
            buf[nb] = '\0';

            if (nb > 0) {
                printf("%s", buf);
                test = 1;
            }

            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://webserverlemonade.herokuapp.com/metrology");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"timestamp\" : "",\"weather\" : {\"dfn\" : \"0\", \"weather\" : \"RAINY\"}}");
                printf("{\"timestamp\" : \"165\",\"weather\" : {\"dfn\" : \"0\", \"weather\" : \"RAINY\"}}\n");

                list = curl_slist_append(list, "content-Type:application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
            }
            curl_global_cleanup();

        }
        close(fd);
    }
    return 0;
}
