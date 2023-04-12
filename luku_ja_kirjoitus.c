#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include </usr/include/postgresql/libpq-fe.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

/*void do_exit(PGconn *conn, PGresult *res) {
    fprintf(stderr, "%s\n", PQerrorMessage(conn));
    PQclear(res);
    PQfinish(conn);
    exit(1);
}*/

struct myData {
    int id;
    char taulu[5];
    float lampo;
    float kosteus;
};

int main() {

    // USB portin luku

    int serial_port = open("/dev/ttyUSB0", O_RDWR);

    struct termios tty;

    if (tcgetattr(serial_port, &tty) !=  0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= CSTOPB;
    tty.c_cflag &= CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ECHONL;
    tty.c_lflag &= ~ISIG;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    // Tietokantaan kirjoitus

    /*PGconn *conn = PQconnectdb("user=thomas password=kesa2022 dbname=mittaukset");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    PGresult *res = PQexec(conn, "DROP TABLE IF EXISTS asiakkaat");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        do_exit(conn, res);
    }

    PQclear(res);

    res =PQexec(conn, "CREATE TABLE asiakkaat(id INTEGER PRIMARY KEY, nimi VARCHAR(20), osoite VARCHAR(30))");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        do_exit(conn, res);
    }*/

    while (1) {

        char read_buf [256];

        memset(&read_buf, '\0', sizeof(read_buf));

        int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

        if (num_bytes < 0) {
            printf("Error reading: %s", strerror(errno));   
            return 1;
        }
        //u_int64_t oma = (u_int64_t) read_buf;
        //char oma1[] = oma + '0';
        char testi[100];
        strncpy(testi, read_buf, 80);
        testi[5] = '\0';
        //printf("%s", read_buf); 
        //unsigned char msg[] = "Terve\r\n";
        //write(serial_port, msg, sizeof(msg));
        //usleep((3 + 25) * 100);
        //printf("%s", read_buf);
        //char buf[100];
        //int n = read(serial_port, buf, sizeof(buf));
        //printf("%s", buf);

        //char s[2] = ":";
        //char * sisalto = strtok(read_buf, s);
        //char testi[] = sisalto[0];
        usleep(200);
        printf("%s", testi);
        //testi[6];
        /*

        // PALAUTTAA ASCII ARVOJA
        int alku = 0;
        for (int x = 0; x < (sizeof(read_buf) / sizeof(char)); x++) {
            printf("%i", read_buf[x]);
            if (read_buf[x] == ':') {
                int koko = x - alku;
                char * arvo = malloc(sizeof(char) * koko);
                memset(arvo, '0', sizeof(arvo));
                for (int y = alku; y < x; y++) {
                    arvo[y] = read_buf[y];
                }
                printf("%s", arvo);
            }
        }
        */
        //struct myData data;
        //data.id = read_buf[0];
        //strcpy(data.taulu, "sisa");//{read_buf[2], read_buf[3], read_buf[4], read_buf[5]};
        //data.lampo = 10.36;//{read_buf[7], read_buf[8], read_buf[9], read_buf[10], read_buf[11]};
        //data.kosteus = 56.45;//{read_buf[13], read_buf[14], read_buf[15], read_buf[16], read_buf[17]};
        //printf("%i %s %f %f\n", data.id, data.taulu, data.lampo, data.kosteus);
        //char testi[5] = {read_buf[0], read_buf[1], read_buf[2], read_buf[3]};
        //printf("%i", id);
        //printf("%s", testi);
        //char h = (char) read_buf[0];
        //printf("%s", read_buf);
        /*if (sisalto[0] == '1') {
            printf("ulko");
            //printf("%d\t%d", sisalto[2], sisalto[3]);
        }*/
        /*
        PQclear(res);

        res = PQexec(conn, "INSERT INTO asiakkaat VALUES(1, 'thomas', 'kajaani')");

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            do_exit(conn, res);
        }*/
        
    }

    //PQclear(res);
    //PQfinish(conn);
    close(serial_port);

    return 0;   
}