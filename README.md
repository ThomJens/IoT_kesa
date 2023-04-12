# Kesa_projekti

kesä 2022

## Projektin tausta

Projektilla oli kolme tavoitetta. Opetella C-kieltä, pystyttää tietokanta ja rakentaa järjestelmä, joka lukee, välittää ja tallentaa dataa tietokantaan.
Jotta nämä tavoitteet saavutettaisin, niin oli selvää, että tarvitsin jonkin sortin mikrokontrollerin tai vastaavan. Opettajan suosituksesta päädyin ESP32 mikrokontrolleriin ja DHT11, jolla mitataan ilmankosteutta ja lämpötilaa. Ohjelmointi ympäristöksi valittiin tietenkin arduino IDE, joka asennettiin ubuntu koneelle. Tietokannaksi valittiin PostgreSQL, koska halusin oppia sen. Datan luku ja tietokantaan kirjoitus sovellus kokeiltiin ohjelmoida C-kielellä, mutta palattiin Pythonin. Pythoniin päädyttiin, koska datan eheys muodostui ongelmaksi ja vasta jälkeenpäin muistin checknullin olemassa olon.

## ESP32 ja DHT11
Tässä projektissa käytettiin kolmea ESP32 ja kahta DHT11. kaksi ESP32 toimi lähettäjänä joihin kiinnitettiin DHT11 ja kolmas ESP32 toimi vastaanottajana joka kiinnitetään tietokoneen USB-porttiin.
ESP32 ja DHT11 yhdistäminen toisiisna oli helppoa. Katsottiin, että maadoitus (gnd) pinnit ovat yhdistetty toisiinsa ja DHT11 vcc pinni on yhdistetty ESP32 3v3 pinniin. Lopuksi liitetään DHT11 data pinni ESP32 numeroituun pinniin. Tässä projektissa käytettiin pinniä neljä. Kun ESP32 ja DHT11 on yhdistetty toisiinsa, niin päästään ohjelmoimaan.

## Arduino IDE
Arduino IDE:n asentaminen onnistui lataamalla Arduinon kotisivuilta asennuspaketin ja käymällä asennusohjeet läpi https://ubuntu.com/tutorials/install-the-arduino-ide#2-installing-via-a-tarball.
Asennuksen jälkeen minulla ilmeni kahdet ongelmat. Ensimmäinen ongelma oli, että IDE ei havainnut porttia. Tämä ratkesi komennolla ´´sudo apt remove brltty´´. Brltty:stä voi lukea täältä https://brltty.app/. Toinen ongelma oli, että minulla ei ollut riittäviä oikeuksia käyttää USB-porttia. Käytin komentoa ´´sudo chmod u=rwx,g=rx,o=r /dev/tty*´´. Kyseinen komento alkaa minulle riittävät oikeudet jokaiseen USB-porttiin.
Tämän jälkeen Arduino IDE pitäisi olla toimintakykyinen ja ESP32 voidaan syöttää koodia. En ala käydä koodia tarkasti läpi, koska koodista löytyy kommentit. Yhden asian tosin mainitsen. Jotta lähettäjä ESP32 voivat lähettää dataa vastaanottimelle, täytyy niiden tietoo mikä on vastaanottajan mac osoite. Linkin takaa löytyy ohjeet sen selvittämiseen https://randomnerdtutorials.com/get-change-esp32-esp8266-mac-address-arduino/.

## Tietokannan pystytys
Asensin PostgreSQL ubuntulle näillä ohjeilla https://www.postgresql.org/download/linux/ubuntu/. Minulla ei ollut suurempia ongelmia asennuksen kanssa. Asennuksen jälkeen loin uuden roolin, koska en halunnut laittaa adminin tietoja scriptiin. Roolia luodessa oikeudet on määriteltävä joko luonnin yhteydessä tai päivittää myöhemmin. Itse loin roolin jolla oli oikeudet luoda tietokantoja ja myöhemmin lisäsin kirjautumisoikeudet. Lisäsin kirjautumisoikeudet myöhemmin, koska en tajunnut, että ne eivät olleet oletuksena päällä.

Kun rooli on luotu, niin loin tietokannan, jonna kaikki data tallennetaan. Alhaalla näkyy kuinka rooli luodaan ja muokataan sekä miten tietokanta luodaa. Tämän lisäksi opettelin kuinka id:hen lisättiin auto incrementti ominaisuu. Tämä onnistui SERIAL komennolla. Tämä n lisäksi selvitin kuinka SERIAL nollataan, joka tapahtuu komennolla ALTER SEQUENCE. Lopuksi tutustuin PostgreSQL perus komentoihin, kuten \d joka on sama kuin mariadb:n DESC.

```sql=postgres
CREATE ROLE thomas WITH LOGIN PASSWORD 'kesa2022' CREATEDB;
ALTER ROLE thomas WITH LOGIN;

CREATE TABLE sisa (
    id SERIAL PRIMARY KEY,
    lampotila DECIMAL(4, 2) NOT NULL,
    ilmankosteus DECIMAL(4, 2) NOT NULL, 
    aika TIMESTAMP NOT NULL
);

ALTER SEQUENCE ulko_id_seq RESTART WITH 1;

\q
\d sisa
\du
exit

sudo -i -u postgres
psql -d mittaukset -U thomas
```

## Datan luku ja kirjoitus, python
Datan luku hoidetaan Python skriptillä ja serial sekä psycopg2 kirjastoilla. Serial kirjasto huolehtii USB-portin lukemisesta ja datan eheydestä säästäen aikaa ja vaivaa. Pyscopg2 kirjastolla hoidetaan PostgreSQL tietokannan käsittely.

## Datan luku ja kirjoitus, C-kieli
Tämä ei ollut alkuperäisessä suunnitelmassa, mutta koska asiat etenivät hyvin, päätin kirjoittaa Python skriptin C-kielessä. Vähiten ongelmia minulla oli tietokantaan kirjoittamisen kanssa. Se meni samalla tavalla kuin Pythonilla. Otettiin yhteys tietokantaan valmiilla kirjastolla, joka sai argumentiksi käyttäjä nimen, salasanan ja tietokannan nimen. 
```
PGconn *conn = PQconnectdb("user=thomas password=kesa2022 dbname=mittaukset");

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
```

Suurempi ongelma oli USB-portin luvun kanssa. Minulla ei ollut mitään käsitystä miten se tehtiin. Paljastui, että portti pitää ensiksi alustaa. Alustamisessa hyödynnettiin bitwise toimintoja ja pakko myöntää, että asia ei käynyt ihan selväksi mitä koko prosessissa tapahtui. Alla on pätkä koodia, joka alustaa portin. En nyt ala käymään sitä läpi, koska kuten sanoin, en täysin ymmärrä mitä siinä tapahtuu.
```
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
```

Suurin haaste tässä oli datan eheyden kanssa. Kun sain kaiken toimimaan ja aloin tallentamaan dataa tietokantaan, huomasin, että tallennettu data ei ollut ihan oikea. Asiaa tutkiskelutani, tajusin, että minulta putoaa bittejä pois. Aluksi pohdin, että missä vaiheessa bittejä putoaa, mutta koska Python skriptin kanssa ei ollut ongelmia, niin ainoa vaihtoehto oli, että bittejä putoaa kun luen USB-porttia C-ohjalmallani. Mietin, että johtuuko se siitä, että structini muodostuu useammasta muuttujasta? Ryhdyin tutkiskelemaan asiaa ja päädyin kokeilemaan u_int64 tyyppistä muuttujaa, johon tallensin kaikki datat bitwise menetelmiä hyödyntäen. Myöhemmin tajusin, että tämä ei auta, koska bittejä putosi structin yksittäisen muuttuja sisällä. Tämä ratkaisu ei siis tule auttamaan, mutta ainakin opin liikuttamaan bittejä. Mietin vielä tämän jälkeen miten ongelman voisi ratkaista, mutta en keksinyt mitään. Lopulta päädyin käyttämään jo olemassa olevaa Python skriptiä ja vasta kuukautta myöhemmin muistin checksumman olemassaolon. En tosin enää tässä vaiheessa kauheasti jaksanut ruveta säätämään ja nopeasti lukaisin aiheesta.

## Yhteenveto
Minu tavoitteeni oli oppia C:tä, pystyttää tietokanta ja rakentaa järjestelmä, joka tuottaa tietoa ja tallentaa sen tiedon tietokantaan. Voin sanoa, että kaikki kolme tavoitetta täyttyivät. Vaikka en saanut sovellustani C-versiota toimimaan, opin erittäin paljon mitä asioita datan siirrossa pitää ottaa huomioon. Jos en olisi kokeillut rakentaa C-versiota, niin en usko, että olisin oppinut niitä asioita. Toinen merkittävä asia jonka opin, oli tietokannan pystytys. Tähän asti olin vain käyttänyt Dockerin tietokantoja ja ainostaan mariadb:n  tietokantoja. Ensimmäistä kertaa pystytin ja käytin muuta tietokantaa ja säädin käyttäjän oikeuksia. Olen erittäin tyytyväinen, että tein tämän pienen projektin, koska koen kasvaneeni ohjelmoijana.