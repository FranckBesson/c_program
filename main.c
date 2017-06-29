#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>

void initChaine(char* chaine, int taille);

int main() {

//=============== Variables ================//

    int fd;                                 // Serial Port File
    int cpt = 0;                            // Character counter
    char hour[6];                           // Current Hour buffer
    char current[13] = "            ";      // Current Weather buffer
    char forecast[13] = "            ";     // Forecast Weather buffer
    char newChar[13] = "            ";      // Character receive buffer
    char data[150];                         // Data Send buffer

//=============== Curl init ================//

    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);      // Init Curl global
    struct curl_slist *list = NULL;         // Create list curl


//=============== Program ==================//

    if ((fd = open("/dev/ttyACM0", O_RDWR)) == -1) {    // Opening and check Serial Port File
        perror("open");
        exit(-1);
    }

while (1){                                  // Loop while the time elapses
        curl = curl_easy_init();            // Init Curl easy
        read(fd, &newChar[cpt], 1);         // Read Serial Port
        if (newChar[cpt] == '!'){           // Arduino sent '!' if user push button stop
            close(fd);                      // Closing serial port file
            return 1;                       // Stop C program
        }
        if (newChar[cpt] == '@'){           // '@' is the first character of serial transmission
            cpt = -1;
            for (int i = 0; i < 3; ++i) {
                while (newChar[cpt] != ',' && newChar[cpt] != '#') {
                    cpt++;
                    read(fd, &newChar[cpt],1);
                }
                if (newChar[cpt] == ',' || newChar[cpt] == '#') {
                    newChar[cpt] = '\0';
                    cpt = -1;
                    switch (i){
                        case 0:
                            strncpy(hour, newChar, sizeof(newChar));
                            printf("\nhour : %s\n", hour);
                            break;
                        case 1:
                            strncpy(current, newChar, sizeof(newChar));
                            printf("current weather : %s\n", current);
                            break;
                        case 2:
                            strncpy(forecast, newChar, sizeof(newChar));
                            printf("forecast weather : %s\n", forecast);
                            break;
                        default:
                            break;
                    }
                }
            }
            initChaine(data, 150);
            strcat(data, "{\"timestamp\" : \"");
            strcat(data, hour);
            strcat(data, "\",\"weather\" : [{\"dfn\" : \"0\", \"weather\" : \"");
            strcat(data, current);
            strcat(data, "\"},{\"dfn\" : \"1\", \"weather\" : \"");
            strcat(data, forecast);
            strcat(data, "\"}]}");
            printf("%s\n", data);
            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, "http://webserverlemonade.herokuapp.com/metrology");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

                list = curl_slist_append(list, "content-Type:application/json");
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                curl_slist_free_all(list);
                list = NULL;
            }
        }
    }
    close(fd);
    curl_global_cleanup();
    return 0;
}

void initChaine(char* chaine, int taille){
    for (int i = 0; i < taille; ++i) {
        chaine[i] = 0;
    }
}
