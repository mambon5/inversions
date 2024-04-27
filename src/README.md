- [Linux system admin](#linux-system-admin)
  - [Com posar un servei systemd en marxa periòdicament](#com-posar-un-servei-systemd-en-marxa-periòdicament)
    - [1. On es posa el servei que fa la feina, i el timer que el crida periòdicament?](#1-on-es-posa-el-servei-que-fa-la-feina-i-el-timer-que-el-crida-periòdicament)
    - [2. Estructura del servei.](#2-estructura-del-servei)
    - [3. Estructura del fitxer `timer`.](#3-estructura-del-fitxer-timer)
    - [4. Com activar el servei i el timer](#4-com-activar-el-servei-i-el-timer)
- [Notes sobre la programació:](#notes-sobre-la-programació)


## Linux system admin

### Com posar un servei systemd en marxa periòdicament

#### 1. On es posa el servei que fa la feina, i el timer que el crida periòdicament?

El *servei* i el fitxer *timer* es posen a la carpeta de linux `/etc/systemd/system`.

#### 2. Estructura del servei. 
Pot ser un fitxer així molt simple:
```
[Unit]
Description="Get current daily percentile of stock values"

[Service]
ExecStart=/var/www/escolamatem/cpp/starter_main_stocks.sh
```
On simplement es posa una descripció de què fa, en el tag `[Unit]` i en el tag `[Service]` es dóna la localització del programa que s'executarà quan es cridi aquest servei.

*Nota: Jo he anomenat `AnaliTotTicks.service` i `AnaliMainTicks.service*` als serveis que analitzen tots, i els ticks principals, respectivament.* 

#### 3. Estructura del fitxer `timer`. 
El fitxer `AnaliMainTicks.timer` té una estructura així:

```
[Unit]
Description="run the get main daily stock percentiles every day at 12pm"

[Timer]
OnBootSec=5min
OnCalendar=*-*-* 12:00:00
Unit=AnaliMainTicks.service

[Install]
WantedBy=multi-user.target
```

i el timer `AnaliTotTicks.timer` aquesta:

```
[Unit]
Description="run the get all the daily stock percentiles every 23min"

[Timer]
OnBootSec=5min
#OnUnitActiveSec=23min
OnCalendar=Mon..Fri *-*-* *:00,20,40:00 # each 20 min during stock exchange days
Unit=AnaliTotTicks.service

[Install]
WantedBy=multi-user.target
```


Aquí el tag `[Install]` és força important, perquè el `WantedBy` pot ser: `graphical.target`(inicia una interfaç d'usuari i el `multi-user.target`),  `sysinit.target`(al iniciar el systema) o `multi-user.target`(un servei que no necessita interfaç gràfica). Nosaltres triem l'últim encara que per defecte es posa el gràfical target.

L'altre element important és `OnCalendar` ja que aquí s'ha d'especificar en format especial, l'intèrval de periodicitat que volem per al nostre servei. Llegiu aquí alguns exemples de com posar el format de periodicitat : https://www.freedesktop.org/software/systemd/man/latest/systemd.time.html i https://wiki.archlinux.org/title/systemd/Timers .

L'element `OnBootSec=5min` indica quant temps després de l'inici del systema, es vol executar el servei.

L'element `Unit`, indica quin servei executar. Ha d'estar a la mateixa carpeta que el *timer*.
 
 L'element `OnUnitActiveSec=24h` reinicia el servei després de 24h de l'ultima execució. Crec que aquestà funció es solapa amb l'element `OnCalendar` que ja el crida cada 24h.

*Nota: Jo he anomenat `AnaliTotTicks.timer` i `AnaliMainTicks.timer` als timers que criden cada servei, el de tots, i el dels ticks principals, respectivament.*

#### 4. Com activar el servei i el timer

Primer de tot hem de carregar tots els fitxers unit usant:

```
systemctl daemon-reload
```

Per activar el timer només hem d'executar:

```
sudo systemctl enable TIMER.timer
```
Això farà que al fer boot, el timer s'executi.
Per executar el timer directament, reiniciarlo, o parar-lo, executar respectivament:
```
sudo systemctl start TIMER.timer
sudo systemctl restart TIMER.timer
sudo systemctl stop TIMER.timer
```
Per mirar el seu estat es pot executar:
``` 
systemctl status TIMER.timer
```

Documentació: https://askubuntu.com/questions/1083537/how-do-i-properly-install-a-systemd-timer-and-service

Igualment per al servei, per demanar a l'ordinador que l'executi al inici del ordinador, executeu:
```
sudo systemctl enable SERVEI.service
```
Per executar, reiniciar o parar el servei escriviu:
```
sudo systemctl start SERVEI.service
sudo systemctl restart SERVEI.service
sudo systemctl stop SERVEI.service
```
I després actualitzeu els fitxers unix fent un:
```
systemctl daemon-reload
```

## Notes sobre la programació:

1. Al posar un for loop, i a dins un if(getline(fitxer, string, ",")) es comporta el getline malament, i llegeix 3 cops seguits la última linea si el for loop segueix en peu. 
En canvi si posem un while loop while(getline(fitxer, string, ",")) el getlien es fa adecuadament i no es repeteix la lectura de la última línia del fitxer. Fet extrany però cert.