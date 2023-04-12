import serial
import psycopg2
from datetime import datetime
from time import sleep

# USB-portin tiedot.
ser = serial.Serial("/dev/ttyUSB0", 115200, timeout = None)
# Maaritetaan sisa ja ulko mittaukset.
edelliset = {"sisa": (None, None), "ulko": (None, None)}
taulu = "mittaukset"
sarakkeet = ("lampotila", "ilmankosteus", "aika")
x = 0

while True:
    # Kun sisa ja ulko mittaukset on luettu, odoteaan 60 sekunttia ennen kuin luetaan seuraavat mittaukset.
    if x == 2:
        print(datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
        x = 0
        sleep(60)
    # Luetaan data USB-portista.
    data = ser.readline()
    data = data.decode("utf-8")
    data = data.replace("\r\n", "").split(":")
    # Jaetaan data muuttujiin.
    taulu = str(data[1])
    lampo = float(data[2])
    kosteus = float(data[3])
    aika = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    # Maaritellaan edellisen lukeman tiedot, jotta ei tule tuplia.
    edel = (lampo, kosteus)
    if edelliset[taulu] == edel:
        x += 1 
        continue
    edelliset[taulu] = edel

    # Yhdistetaan tietokantaan.
    yhteys = psycopg2.connect(
        host = "localhost",
        database = "mittaukset",
        user = "thomas",
        password = "kesa2022"
    )
    kursori = yhteys.cursor()
    # Kirjoitetaan tietokantaan
    sql = f"INSERT INTO {taulu} ({sarakkeet[0]}, {sarakkeet[1]}, {sarakkeet[2]}) VALUES ({lampo}, {kosteus}, \'{aika}\');"
    kursori.execute(sql)
    yhteys.commit()
    kursori.close()
    yhteys.close()
    x += 1
    # \u2103 on celsius merkki
    print(f"{taulu}:\tlampotila: {lampo} \u2103\tilmankosteus: {kosteus} %\taika: {aika}")
