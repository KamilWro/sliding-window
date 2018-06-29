# Sliding-window
#### Opis:
Aplikacja implementuje algorytm przesuwnego okna łącząc się z określonym serwerem i wysyłając pakiety UDP. Program powinien przyjmować trzy argumenty: ``port``, ``nazwa pliku`` i ``rozmiar w bajtach``. Celem
jest pobranie od specjalnego serwera UDP nasłuchującego na porcie ``port`` komputera o adresie
``156.17.4.30`` pierwszych ``rozmiar`` bajtów pliku i zapisanie ich w pliku ``nazwa pliku`` wynikowego. 
W celu komunikacji z serwerem UDP wysyła mu datagramy UDP zawierające następujący napis: <br>
``GET start długość \n ``<br>

Wartosć start powinna być liczbą całkowitą z zakresu [0, 10 000 000], zaś długość z zakresu
[1, 1 000]. Znak `\n` jest uniksowym końcem wiersza, zaś odstępy są pojedynczymi spacjami.
Jedyną zawartością datagramu musi być powyższy napis: serwer zignoruje datagramy, które nie
spełniają tej specyfikacji. W odpowiedzi serwer wyśle datagram, na którego początku będzie
znajdować się napis:   
``DATA start długość \n``  

Wartości start i długość są takie, jakich zażądał klient. Po tym napisie znajduje się długość
bajtów pliku: od bajtu o numerze start do bajtu o numerze start+długość-1. Uwaga: bajty
pliku numerowane są od zera.

#### Kompilacja:
Za pomocą przygotowanego Makefile:  
`make`

#### Przykład użycia:
`./transport port fileName fileSize`
