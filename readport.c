#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
//#include <libpq-fe.h>
#include </usr/include/postgresql/libpq-fe.h>
#include <stdint.h>
//#include <zlib.h>
#include <time.h>
// gcc -Wall -Wextra -I "/usr/include/postgresql" -L "/usr/posrgresql/libpq" readport.c -lpq -o readport

#define READBUFFER 13
#define TIMEBUFFER 26


void do_exit(PGconn *conn, PGresult *res) {
    fprintf(stderr, "%s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    exit(1);
}



void writeData(const char *sql) {
    PGconn *conn = PQconnectdb("user= password= dbname=");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    PGresult *res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        do_exit(conn, res);
    }
}

int main(void) {
    int serial_port = open("/dev/ttyUSB0", O_RDWR);

    struct termios tty;

    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from open: %s", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= CSTOPB;
    tty.c_cflag &= CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag %= ~ONLCR;

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    // Tietokantaan kirjoitus
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int hour = tm.tm_hour - 1;

    while(1) {
        while (hour == tm.tm_hour) {
            t = time(NULL);
            tm = *localtime(&t);
            usleep(60000000);
        }
        char read_buf[READBUFFER];
        memset(&read_buf, '\0', sizeof(read_buf));
        int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

        if (num_bytes < 0) {
            printf("Error reading: %s", strerror(errno));
            return 1;
        }
        
        char *timestamp = asctime(&tm);

        const char *part1 = "INSERT INTO testi (data, timestamp) VALUES('\0";
        size_t query_size = strlen(part1) + READBUFFER + TIMEBUFFER + 6;
        char *sql = (char*)malloc(query_size);
        memset(sql, '\0', query_size);
        strcat(sql, part1);
        strcat(sql, read_buf);
        strcat(sql, "','");
        strcat(sql, timestamp);
        strcat(sql, "');\0");

        if (sql[45] == ':' && sql[51] == ':' && sql[58] == ',') {
            hour = tm.tm_hour;
            writeData(sql);
            printf("%s", sql);
        }
    }

    

    printf("Done");
    //close(serial_port);
    //free(sql);



    return 0;
}