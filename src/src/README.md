- [Linux system admin](#linux-system-admin)
    - [Com posar un servei systemd en marxa periòdicament](#com-posar-un-servei-systemd-en-marxa-periòdicament)
      - [1. On es posa el servei que fa la feina, i el timer que el crida periòdicament?](#1-on-es-posa-el-servei-que-fa-la-feina-i-el-timer-que-el-crida-periòdicament)
      - [2. Estructura del servei.](#2-estructura-del-servei)
      - [3. Estructura del fitxer `timer`.](#3-estructura-del-fitxer-timer)
        - [Verificar que el timer i el servei tenen una sintaxi correcta](#verificar-que-el-timer-i-el-servei-tenen-una-sintaxi-correcta)
      - [4. Estructura del fitxer servei](#4-estructura-del-fitxer-servei)
      - [5. Com activar el servei i el timer](#5-com-activar-el-servei-i-el-timer)
  - [Notes sobre la programació:](#notes-sobre-la-programació)
- [Instalar la app d'analisi d'accions de borsa](#instalar-la-app-danalisi-daccions-de-borsa)
  - [1. Descargar-se el repositori github](#1-descargar-se-el-repositori-github)
  - [2. Canviar la contrasenya del servidor](#2-canviar-la-contrasenya-del-servidor)
      - [3. Canviar els permisos del directori.](#3-canviar-els-permisos-del-directori)
  - [4. Copiar els timers i serveis](#4-copiar-els-timers-i-serveis)
- [Servir més d'una pàgina web al mateix servidor](#servir-més-duna-pàgina-web-al-mateix-servidor)
      - [Forçar a fer git pull i borrar canvis locals](#forçar-a-fer-git-pull-i-borrar-canvis-locals)


# Linux system admin

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
# each 20 min during stock exchange days
OnCalendar=Mon..Fri *-*-* *:00,20,40:00 
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

##### Verificar que el timer i el servei tenen una sintaxi correcta

*Important:* Per comprovar que la sintaxi tant del *timer* com del *servei* siguin correctes, és molt recomanable executar el següent codi abans d'activar el servei i el timer:
```
systemd-analyze verify SERVEI.*
```
#### 4. Estructura del fitxer servei

És més senzilla, només conté dues línies pràcticament:

Estructura del fitxer de servei `AnaliTotTicks.service`:

```
[Unit]
Description="Get current daily percentile of stock values"

[Service]
ExecStart=/var/www/inversions/src/src/starter_all_stocks.sh
```

Estructura del fitxer de servei `AnaliMainTicks.service`:

```
[Unit]
Description="Get current daily percentile of main stock values"

[Service]
ExecStart=/var/www/inversions/src/src/starter_main_stocks.sh
```

L'element principal és el `ExecStart` que ens diu quin arxiu executar al iniciar el servei.



#### 5. Com activar el servei i el timer

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

# Instalar la app d'analisi d'accions de borsa

## 1. Descargar-se el repositori github

Descargar de https://github.com/mambon5/inversions_borsa.git

## 2. Canviar la contrasenya del servidor

El arxius `starter_all_stocks.sh` i l'arxiu `starter_main_stocks.sh` tenen a dins seu, una paràmetre que és la contrasenya del servidor. S'ha de canviar per tal de poder fer els canvis pertinents.

#### 3. Canviar els permisos del directori.

Per a tal que el programa pugui funcionar i modificar i crear arxius del directori és necesari donar permissos `RWX` a tota la carpeta i recursivament.

Usant la commanda:

```
sudo chmod -R 777 inversions
```

es pot fer un cop clonat el repositori.


## 4. Copiar els timers i serveis

Cal copiar els serveis i timers del systemd de linux a la carpeta correcta per a que tot funcioni. En particular, cal copiar els arxius.

```
AnaliTotTicks.service
AnaliTotTicks.timer
AnaliMainTicks.service
AnaliMainTicks.timer
```

A la carpeta `/etc/systemd/system/`. Un cop copiats cal entrar a dita carpeta i executar:

```
systemd-analyze verify AnaliTotTicks.*
systemd-analyze verify AnaliMainTicks.*
```
Per veure que no continguin cap error,

```
systemctl daemon-reload
```
Per reiniciar el servei `systemctl` i que llegeixi els unit files dels nous serveis creats.

```
sudo systemctl enable AnaliTotTicks.timer
sudo systemctl enable AnaliMainTicks.timer
```
Per a que s'activi el timer al reniniciar l'ordinador.

```
sudo systemctl start AnaliTotTicks.timer
sudo systemctl start AnaliMainTicks.timer
```
Per a iniciar immediatament els dos timers.

I ja estaria iniciat el procès d'anàlisi periòdica dels valors de borsa diari.

# Servir més d'una pàgina web al mateix servidor

Bàsicament s'han de seguir aquests passos per les dues pàgines diferents a crear, i funcionarà tot correcte > https://dev.to/lloyds-digital/how-you-can-host-websites-from-home-4pke

El pas clau és que possis el nom de una adreça web com "nescolam.com" (la teva adreça que usaràs) on posa `ServerName` al host virtual que poses a al carpeta `/etc/apache2/sites-available/`. I que compris un domini u creis un a "no-ip.com" que lligui el nom de la direcció web `nescolam.com` amb la teva IP pública del servidor que pot ser `58.231.2.12`.

Un cop feu els canvis al fitxer del host virtual, executeu:

```
sudo a2ensite mywebsite.conf 
sudo apache2ctl configtest
sudo systemctl restart apache2
```

#### Forçar a fer git pull i borrar canvis locals

Executeu
```
git fetch origin main
git reset --hard origin/main
```

Bon hacking!