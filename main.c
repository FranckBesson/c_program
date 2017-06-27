#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>

char** split(char* chaine, const char* delim);
void initChaine(char* chaine, int taille);

int main() {
    int fd;
    char hour[6];
    char current[13] = "            ";
    char forecast[13] = "            ";
    int cpt = 0;
    char* TabResult[3];
    char newChar[13] = "            ";
    char data[150];
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    struct curl_slist *list = NULL;


    printf("\n");
    if ((fd = open("/dev/ttyACM0", O_RDWR)) == -1) {
        perror("open");
        exit(-1);
    }

    while (1){
        curl = curl_easy_init();
        read(fd, &newChar[cpt], 1);
        if (newChar[cpt] == '!'){
            close(fd);
            return 1;
        }
        if (newChar[cpt] == '@'){
            cpt = -1;
            for (int i = 0; i < 3; ++i) {
                while (newChar[cpt] != ',' && newChar[cpt] != '#') {
                    cpt++;
                    read(fd, &newChar[cpt],1);
                }
                if (newChar[cpt] == ',' || newChar[cpt] == '#') {
                    newChar[cpt] = '\0';
                    TabResult[i] = newChar;
                    cpt = -1;
                    switch (i){
                        case 0:
                            strncpy(hour, newChar, sizeof(newChar));
                            printf("hour : %s\n", hour);
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
            }
        }
    }
    close(fd);
    curl_global_cleanup();
    return 0;
}

char** split(char* chaine, const char* delim){
    char** tab = NULL;
    char* ptr;
    int sizeStr;
    int sizeTab = 0;
    char* largestring;

    int sizeDelim = strlen(delim);

    largestring = chaine;

    while ((ptr = strstr(largestring, delim)) != NULL){
        sizeStr = ptr - largestring;
        sizeTab++;
        tab = (char**) realloc(tab, sizeof(char*)*sizeTab);
        tab[sizeTab-1] = (char*) malloc(sizeof(char)*(sizeStr + 1));
        strncpy(tab[sizeTab - 1], largestring, sizeStr);
        tab[sizeTab - 1][sizeStr] = '\0';
    }
    ptr = ptr + sizeDelim;
    largestring = ptr;
    if ((strlen(largestring)) != 0){
        sizeStr = strlen(largestring);
        sizeTab ++;
        tab= (char**) realloc(tab,sizeof(char*)*sizeTab);
        tab[sizeTab-1]=(char*) malloc( sizeof(char)*(sizeStr+1) );
        strncpy(tab[sizeTab-1],largestring,sizeStr);
        tab[sizeTab-1][sizeStr]='\0';
    }
    sizeTab ++;
    tab = (char**) realloc(tab, sizeof(char*)*sizeTab);
    tab[sizeTab - 1] = NULL;

    return tab;
}

void initChaine(char* chaine, int taille){
    for (int i = 0; i < taille; ++i) {
        chaine[i] = 0;
    }
}
