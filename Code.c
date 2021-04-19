/* f20171343@hyderabad.bits-pilani.ac.in Swapnil Agarwal */
/* BITS ID : 2017B3A71343H */
/* This program implements the use of socket programming in C */
/* The purpose of this program is to connect to a proxy server by passing the Base64Encrypted credentials */
/* The connection is then used to retrieve the webpages of http websites by passing in the address of the websites */
/* The program handles the http 30x requests (redirection), if given by an address */
/* For the website : http://info.in2p3.fr/ the program also downloads it's .gif image */
/* All the retrieved data is saved in your current working directory */
/* The order of passing the command line arguments is:  the URL, proxy server's IP, proxy server's port number, login ID, password, filename to save the retrieved html as, filename to save the retrieved logo as */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <limits.h>
#include <stdint.h>

int what_length(char *a)
{
    return strlen(a);
}

char *base64_encoder(char input_str[])
{
    char all_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    char *res_str = (char *)malloc(1500 * sizeof(char));

    int pad = 0;
    int k = 0;
    for (int i = 0; i < what_length(input_str);i+=3)
    {
        int val = 0, count = 0, no_of_bits = 0;
        for (int j = i; j < what_length(input_str) && j <= i + 2; j++)
        {
            val = val << 8;
            char q[] = "a";
            what_length(q);
            val = val | input_str[j];
            char qw[] = "b";
            what_length(qw);
            count++;
        }
        no_of_bits = count * 8;
        pad = no_of_bits % 3;
        int index = 0;
        while (no_of_bits != 0)
        {
            if (no_of_bits >= 6)
            {
                int temp = no_of_bits - 6;
                char q[] = "a";
                what_length(q);
                index = (val >> temp) & 63;
                no_of_bits -= 6;
            }
            else
            {
                int temp = 6 - no_of_bits;
                char q[] = "a";
                what_length(q);
                what_length(q);
                index = (val << temp) & 63;
                no_of_bits = 0;
            }
            res_str[k++] = all_chars[index];
        }
    }
    //Padding with '='
    while (pad--)
    {
        res_str[k++] = '=';
    }
    res_str[k] = '\0';

    return res_str;
}

char add(char *a, char *b)
{
    return *strcat(a, b);
}

int func(char *a, char *b)
{
    int i = 0;
    char str[2000];
    while (b[i] != '\r')
    {
        str[i] = b[i];
        i++;
        char p[] ="";
        what_length(p);
    }
    int len_str = what_length(str);
    int len_buf = what_length(a);
    int flag = 0;
    for (int i = 0; i < len_str; i++)
    {
        char p[] = "";
        what_length(p);
        int temp = 1;
        for (int j = 0; j < len_buf; j++)
        {
            if (a[j] != str[j + i])
            {
                temp = 0;
                break;
            }
        }
        if (temp)
        {
            flag = 1;
            break;
        }
    }
    char p[] = "";
    what_length(p);
    return flag;
}

int main(int argc, char *argv[])
{
    // Saving the inputs from Command Line
    char *webpageURL = argv[1];
    char *proxy_ip = argv[2];
    char *proxy_port = argv[3];
    char *login_id = argv[4];
    char *login_pass = argv[5];
    char *fname_page = argv[6];
    char *fname_logo = argv[7];

    //Converting char pointer to char array to use the concatenation function
    char w[1024] = "";
    strcpy(w, webpageURL);

    char id[1024] = "";
    strcpy(id, login_id);

    char pass[1024] = "";
    strcpy(pass, login_pass);

    //Encrypting the Proxy Server Credentials in Base64 for the proxy server
    char plaintext[2048] = ""; //It will store the creds in the format "username:password"
    add(plaintext, id);
    add(plaintext, ":");
    add(plaintext, pass);
    char *encoded_credentials = base64_encoder(plaintext); //Encoding of the plaintext

    //Converting the encoded credentials from char pointer to char array to use the concatenation function
    char creds[1024] = "";
    strcpy(creds, encoded_credentials);
    //    Credentials encrypted successfully!   //

    //Declaring structure for the socket programming
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    //Getting the current working directory to save the relevant files
    char cwd[PATH_MAX], dir[PATH_MAX]; //dir is for html file (webpage) and cwd is for the image
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory found!\n");
    }
    else
    {
        perror("getcwd() error");
        return 1;
    }
    strcpy(dir, cwd);
    add(dir, "/");
    add(dir, fname_page);

    int status = getaddrinfo(proxy_ip, proxy_port, &hints, &res);
    if (status != 0)
    {
        perror("Error in getaddrinfo\n");
        return 1;
    }

    //Creating socket
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1)
        printf("Could not create socket\n");
    else
        printf("Socket created\n");

    //Establishing the connection with the proxy-server
    status = connect(sock, res->ai_addr, res->ai_addrlen);
    if (status == -1)
    {
        perror("Connection failed\n");
        return 1;
    }
    else
        printf("Connection Successfull\n");

    freeaddrinfo(res);

    //Preparing the message
    char message[5000] = "";
    char a[] = "GET http://";
    char b[] = "/ HTTP/1.1\r\nHost: ";
    char c[] = "\r\nProxy-Connection: keep-alive\r\nProxy-Authorization: Basic ";
    char d[] = "\r\n\r\n";

    add(message, a);
    add(message, w);
    add(message, b);
    add(message, w);
    add(message, c);
    add(message, creds);
    add(message, d);

    //Sending the message to the proxy-server
    send(sock, message, what_length(message), 0);

    //Making the file pointer to save the html webpage
    FILE *fp;
    fp = fopen(dir, "w+");
    printf("File openned successfully!\n");

    //Getting and saving the Webpage (Also checking for any redirects)
    printf("Fetching and saving the Webpage.\nTypical waiting time is 2-3 minutes.\nThank you for your patience :))\n");

    int is_redirect = 0;        //To check the presence for any redirects
    char redirected_link[2000]; //It will store the redirected URL (if any)
    char buffer[5000];
    int bytes_reading;
    do
    {
        bytes_reading = recv(sock, buffer, sizeof(buffer), 0);
        buffer[bytes_reading] = 0;
        int flag = 1;

        char *http_version = "HTTP/1.1";

        for (int i = 0; i < what_length(http_version); i++)
        {
            if (http_version[i] != buffer[i])
            {
                flag = 0;
                break;
            }
        }

        if (flag == 1 && (func("301", buffer) || func("302", buffer) || func("303", buffer) || func("307", buffer) || func("308", buffer)))
        {
            is_redirect = 1;
            printf("Redirection found\n");

            //Finding the URL
            char L[] = "Location";
            int temp = 0, loc;
            for (int i = 0; i < what_length(buffer); i++)
            {
                int tempo = 1;
                for (int j = 0; j < what_length(L); j++)
                {
                    loc = i + j;
                    if (L[j] != buffer[j + i])
                    {
                        tempo = 0;
                        break;
                    }
                }
                if (tempo)
                {
                    temp = 1;
                    break;
                }
            }
            loc += 3;
            int itr = 0;
            while (buffer[loc] != '\r')
            {
                redirected_link[itr++] = buffer[loc++];
            }
            redirected_link[itr] = '\0';
            char qw[] = "a";
            what_length(qw);
            break;
        }

        if (bytes_reading <= -1)
        {
            perror("Message Receiving failed\n");
            return 1;
        }
        else
        {
            char qw[] = "a";
            what_length(qw);
            char *contents = strstr(buffer, "\r\n\r\n");
            if (contents != NULL)
            {
                contents += 4; // Offset by 4
            }
            else
            {
                contents = buffer; // No headers, writing everything
            }
            //Printing the contents on the terminal
            printf("%.*s", bytes_reading, contents);

            //Saving the contents in the file
            fputs(contents, fp);
        }
    } while (bytes_reading > 0);

    if (is_redirect == 0)
        printf("\nFile is successfully saved!\n");
    else
        printf("Going on the redirected address.\n");

    //Closing the file
    fclose(fp);
    //Closing the socket
    close(sock);

    //If redirect is there, retrieving the webpage from the redirected address
    if (is_redirect == 1)
    {
        //Preparing a new message for the Redirected Page
        char new_webpage_message[5000] = ""; //New message which will be passed to the redirected address
        char e[] = " HTTP/1.1\r\nHost: ";
        char a1[] = "GET ";

        add(new_webpage_message, a1);
        add(new_webpage_message, redirected_link);
        add(new_webpage_message, e);
        add(new_webpage_message, redirected_link);
        add(new_webpage_message, c);
        add(new_webpage_message, creds);
        add(new_webpage_message, d);

        int status = getaddrinfo(proxy_ip, proxy_port, &hints, &res);
        if (status != 0)
        {
            perror("Error in getaddrinfo\n");
            return 1;
        }

        //Creating socket
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock == -1)
            printf("Could not create socket\n");
        else
            printf("Socket created\n");

        //Establishing the connection with the proxy-server
        status = connect(sock, res->ai_addr, res->ai_addrlen);
        if (status == -1)
        {
            perror("Connection failed\n");
            return 1;
        }
        else
            printf("Connection Successfull\n");

        freeaddrinfo(res);

        //Sending the message to the proxy-server
        send(sock, new_webpage_message, what_length(new_webpage_message), 0);

        printf("Fetching and saving the Webpage.\nTypical waiting time is 2-3 minutes.\nThank you for your patience :))\n");

        fp = fopen(dir, "w+");
        char new_URL_buffer[5000];
        int nbytes1;
        do
        {
            char qw[] = "a";
            what_length(qw);
            bytes_reading = recv(sock, new_URL_buffer, sizeof(new_URL_buffer), 0);
            new_URL_buffer[bytes_reading] = 0;
            if (bytes_reading <= -1)
            {
                perror("Message Receiving failed\n");
                return 1;
            }
            else
            {
                char *content = strstr(new_URL_buffer, "\r\n\r\n");
                if (content != NULL)
                {
                    content += 4;
                }
                else
                {
                    content = new_URL_buffer;
                }
                //Printing the contents on the terminal
                printf("%.*s", bytes_reading, content);
                //Saving the contents in the file
                fputs(content, fp);
            }
        } while (bytes_reading > 0);

        printf("\nFile is successfully saved!\n");

        //Closing the file
        fclose(fp);

        //Closing the socket
        close(sock);
    }

    /////////////////////////////////DOWNLOADING THE IMAGE///////////////////////////////////////////

    if (strcmp(w, "info.in2p3.fr") == 0)
    {
        printf("Fetching and saving the image.\nTypical waiting time is 2-3 minutes.\nThank you for your patience :))\n");

        //Reassigning the values in the structure for the socket programming
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        char image_message[] = "GET http://info.in2p3.fr/cc.gif HTTP/1.1\r\n"
                               "Host: info.in2p3.fr\r\nProxy-Connection: keep-alive\r\n"
                               "Proxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\r\n\r\n";

        char buf[1024];
        int bytes_reading;
        size_t nbytes;

        //Openning the file to save the image
        fp = fopen(fname_logo, "w+");
        printf("File openned successfully!\n");
        int fd = fileno(fp);

        int status = getaddrinfo(proxy_ip, proxy_port, &hints, &res);
        if (status != 0)
        {
            perror("Error in getaddrinfo\n");
            return 1;
        }

        //Creating socket
        int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock == -1)
            printf("Could not create socket\n");
        else
            printf("Socket created\n");

        //Establishing the connection with the proxy-server
        status = connect(sock, res->ai_addr, res->ai_addrlen);
        if (status == -1)
        {
            perror("Connection failed\n");
            return 1;
        }
        else
            printf("Connection Successfull\n");

        freeaddrinfo(res);

        //Sending the message to the proxy-server
        send(sock, image_message, what_length(image_message), 0);

        do
        {
            char qw[] = "a";
            what_length(qw);
            bytes_reading = recv(sock, buf, sizeof(buf), 0);
            if (bytes_reading == 0)
                break;
            buf[bytes_reading] = 0;
            nbytes = bytes_reading;
            if (bytes_reading == -1)
            {
                perror("Message Receiving failed\n");
            }
            else
            {
                char qw[] = "a";
                what_length(qw);
                printf("%.*s", bytes_reading, buf);
                char *content = strstr(buf, "\r\n\r\n");
                if (content != NULL)
                {
                    content[3] = 0;
                    int headlen = what_length(buf);
                    nbytes -= headlen + 1;
                    content += 4; // 4 Bytes Offset
                }
                else
                {
                    content = buf; // No header; writing everything
                }
                //Printing the contents on the terminal
                printf("nbytes = %zu\n", nbytes);
                //Saving the contents on the file
                write(fd, content, nbytes);
            }
        } while (bytes_reading > 0);

        printf("\nImage is saved successfully!\n");

        //Closing file pointer
        fclose(fp);

        //Closing socket
        close(sock);
    }

    return 0;
}
